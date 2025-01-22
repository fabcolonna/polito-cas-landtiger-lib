#include "cl_list.h"

#include <stdlib.h>
#include <string.h>

// TYPE DEFINITION

struct __ListNode
{
    void *data;
    struct __ListNode *prev, *next;
};

struct __List
{
    struct __ListNode *head, *tail;
    u32 size, elem_size;
    MEM_Allocator *allocator;
};

// PRIVATE FUNCTIONS

_PRIVATE inline struct __ListNode *create_node(CL_List *list, const void *const elem)
{
    struct __ListNode *node = MEM_Alloc(list->allocator, sizeof(struct __ListNode));
    if (!node)
        return NULL;

    node->data = MEM_Alloc(list->allocator, list->elem_size);
    if (!node->data)
    {
        MEM_Free(list->allocator, node);
        return NULL;
    }

    memcpy(node->data, elem, list->elem_size);
    node->prev = node->next = NULL;
    return node;
}

// PUBLIC FUNCTIONS

CL_List *CL_ListAlloc(MEM_Allocator *const alloc, u32 elem_sz)
{
    if (!alloc || !elem_sz)
        return NULL;

    CL_List *const list = MEM_Alloc(alloc, sizeof(CL_List));
    if (!list)
        return NULL;

    *list = (CL_List){
        .head = NULL,
        .tail = NULL,
        .size = 0,
        .elem_size = elem_sz,
        .allocator = alloc,
    };

    return list;
}

void CL_ListFree(CL_List *const list)
{
    if (!list)
        return;

    struct __ListNode *current = list->head, *copy;
    while (current)
    {
        copy = current;
        current = current->next;
        MEM_Free(list->allocator, copy->data);
        MEM_Free(list->allocator, copy);
    }

    MEM_Free(list->allocator, list);
}

CL_Error CL_ListPushBack(CL_List *const list, const void *const elem)
{
    if (!list || !elem)
        return CL_ERR_INVALID_PARAMS;

    struct __ListNode *new_node = create_node(list, elem);
    if (!new_node)
        return CL_ERR_NO_MEMORY;

    if (!list->tail) // Empty list, set head and tail to the new node.
        list->head = list->tail = new_node;
    else
    {
        new_node->prev = list->tail; // Set the new node's previous to the current tail.
        list->tail->next = new_node; // Set the current tail's next to the new node.
        list->tail = new_node;       // The new node is now the tail.
    }

    list->size++;
    return CL_ERR_OK;
}

CL_Error CL_ListPushFront(CL_List *const list, const void *const elem)
{
    if (!list || !elem)
        return CL_ERR_INVALID_PARAMS;

    struct __ListNode *new_node = create_node(list, elem);
    if (!new_node)
        return CL_ERR_NO_MEMORY;

    if (!list->head) // Empty list, set head and tail to the new node.
        list->head = list->tail = new_node;
    else
    {
        new_node->next = list->head; // Set the new node's next to the current head.
        list->head->prev = new_node; // Set the current head's previous to the new node.
        list->head = new_node;       // The new node is now the head.
    }

    list->size++;
    return CL_ERR_OK;
}

void CL_ListPopBack(CL_List *const list, void *out_elem)
{
    if (!list || !list->size)
        return;

    struct __ListNode *last = list->tail;
    if (out_elem)
        memcpy(out_elem, last->data, list->elem_size);

    if (list->size == 1) // Only one element in the list, reset head and tail.
        list->head = list->tail = NULL;
    else
    {
        list->tail = last->prev; // Set the tail to the previous node.
        list->tail->next = NULL; // The new tail's next is NULL.
    }

    MEM_Free(list->allocator, last->data);
    MEM_Free(list->allocator, last);
    list->size--;
}

void CL_ListPopFront(CL_List *const list, void *out_elem)
{
    if (!list || !list->size)
        return;

    struct __ListNode *first = list->head;
    if (out_elem)
        memcpy(out_elem, first->data, list->elem_size);

    if (list->size == 1) // Only one element in the list, reset head and tail.
        list->head = list->tail = NULL;
    else
    {
        list->head = first->next; // Set the head to the next node;
        list->head->prev = NULL;  // The new head's previous is NULL.
    }

    MEM_Free(list->allocator, first->data);
    MEM_Free(list->allocator, first);
    list->size--;
}

CL_Error CL_ListGet(const CL_List *const list, u32 index, void *out_elem)
{
    if (!list || !out_elem)
        return CL_ERR_INVALID_PARAMS;

    if (index >= list->size)
        return CL_ERR_OUT_OF_BOUNDS;

    u32 idx = 0;
    struct __ListNode *current = list->head;
    while (idx++ < index)
        current = current->next;

    // Now current is the node at the specified index.
    memcpy(out_elem, current->data, list->elem_size);
    return CL_ERR_OK;
}

CL_Error CL_ListGetPtr(const CL_List *const list, u32 index, void **out_elem)
{
    if (!list || !out_elem)
        return CL_ERR_INVALID_PARAMS;

    if (index >= list->size)
        return CL_ERR_OUT_OF_BOUNDS;

    u32 idx = 0;
    struct __ListNode *current = list->head;
    while (idx++ < index)
        current = current->next;

    // Now current is the node at the specified index.
    *out_elem = current->data;
    return CL_ERR_OK;
}

CL_Error CL_ListGetLast(const CL_List *const list, void *out_elem)
{
    if (!list || !out_elem)
        return CL_ERR_INVALID_PARAMS;

    if (!list->size)
        return CL_ERR_EMPTY;

    memcpy(out_elem, list->tail->data, list->elem_size);
    return CL_ERR_OK;
}

CL_Error CL_ListGetLastPtr(const CL_List *const list, void **out_elem)
{
    if (!list || !out_elem)
        return CL_ERR_INVALID_PARAMS;

    if (!list->size)
        return CL_ERR_EMPTY;

    *out_elem = list->tail->data;
    return CL_ERR_OK;
}

CL_Error CL_ListInsertAt(CL_List *const list, u32 index, const void *const elem)
{
    if (!list || !elem)
        return CL_ERR_INVALID_PARAMS;

    if (index > list->size) // list->size is a valid index to insert at (insert at the back).
        return CL_ERR_OUT_OF_BOUNDS;

    if (index == 0) // Insert at the front.
        return CL_ListPushFront(list, elem);
    else if (index == list->size) // Insert at the back.
        return CL_ListPushBack(list, elem);

    // If here, we need to insert in the middle.
    struct __ListNode *new_node = create_node(list, elem), *current = list->head;
    if (!new_node)
        return CL_ERR_NO_MEMORY;

    u32 idx = 0;
    while (idx++ < index)
        current = current->next;

    // Now current is the node at the specified index.
    new_node->prev = current->prev;
    new_node->next = current;
    current->prev = new_node;
    new_node->prev->next = new_node;

    list->size++;
    return CL_ERR_OK;
}

CL_Error CL_ListRemoveAt(CL_List *const list, u32 index, void *out_elem)
{
    if (!list || !out_elem)
        return CL_ERR_INVALID_PARAMS;

    if (index >= list->size)
        return CL_ERR_OUT_OF_BOUNDS;

    if (index == 0) // Remove from the front.
        CL_ListPopFront(list, out_elem);
    else if (index == list->size - 1) // Remove from the back.
        CL_ListPopBack(list, out_elem);
    else
    {
        // If here, we need to remove from the middle.
        struct __ListNode *current = list->head;
        u32 idx = 0;
        while (idx++ < index)
            current = current->next;

        // Now current is the node at the specified index.
        current->prev->next = current->next;
        current->next->prev = current->prev;

        if (out_elem)
            memcpy(out_elem, current->data, list->elem_size);

        MEM_Free(list->allocator, current);
        list->size--;
    }

    return CL_ERR_OK;
}

bool CL_ListSearch(const CL_List *const list, const void *const elem, CL_CompareFn compare_fn, u32 *out_index)
{
    if (!list || !elem || !compare_fn)
        return false;

    struct __ListNode *current = list->head;
    u32 idx = 0;
    while (current)
    {
        if (compare_fn(current->data, elem) == 0)
        {
            if (out_index)
                *out_index = idx;
            return true;
        }

        current = current->next;
    }

    return false;
}

void CL_ListClear(CL_List *const list)
{
    if (!list || !list->size)
        return;

    struct __ListNode *current = list->head, *copy;
    while (current)
    {
        copy = current;
        current = current->next;
        MEM_Free(list->allocator, copy->data);
        MEM_Free(list->allocator, copy);
    }

    list->head = list->tail = NULL;
    list->size = 0;
}

u32 CL_ListSize(const CL_List *const list)
{
    return list ? list->size : 0;
}

bool CL_ListIsEmpty(const CL_List *const list)
{
    return !list || !list->size;
}