#include "cl_prioqueue.h"

#include <alloca.h>
#include <stdlib.h>
#include <string.h>

// TYPE DEFINITION

struct __PQueue
{
    void *data;
    u32 size, capacity, elem_size;
    CL_PQueueCompareFn compare_fn; ///< The comparison function.
    MEM_Allocator *allocator;      ///< The memory allocator.
};

#define RESZ_FACTOR 2
#define BASE_CAPACITY 10

// PRIVATE FUNCTIONS

// The priority queue can be visualized as a binary tree, where the parent node has only
// two children (left, right). These macros help to navigate through the tree (which is
// acutually linear, i.e. an array).
#define PARENT(i) ((i - 1) / 2) // The parent of the node at index i. E.g. for i = 0, parent = 0.
#define LEFT(i) (2 * i + 1)     // The left child of the node at index i. E.g. for i = 0, left = 1.
#define RIGHT(i) (2 * i + 2)    // The right child of the node at index i. E.g. for i = 0, right = 2.

_PRIVATE inline void pSwap(void *const a, void *const b, u32 size)
{
    void *temp = alloca(size);
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
}

// When adding an element into the queue, it gets added at the end of the array.
// If the element has a higher priority than its parent node, it needs to be bubbled
// up until it reaches the correct position (i.e. the parent node has a HIGHER priority).
_PRIVATE void bubble_up_leaf(CL_PQueue *const pq)
{
    if (!pq)
        return;

    u32 new_idx = pq->size - 1;
    u8 *parent_elem, *new_elem;
    while (new_idx > 0)
    {
        parent_elem = (u8 *)(pq->data) + (PARENT(new_idx) * pq->elem_size);
        new_elem = (u8 *)(pq->data) + (new_idx * pq->elem_size);
        if (pq->compare_fn(new_elem, parent_elem) < 0)
            pSwap(parent_elem, new_elem, pq->elem_size);

        new_idx = PARENT(new_idx); // Moving up the tree.
    }
}

// When removing an element from the queue, the first element (i.e. the root of the tree)
// gets removed. After that, the last element of the array gets moved to the root. This may
// break the heap property, so we need to bubble down the new root until the heap property is restored.
_PRIVATE void bubble_down_root(CL_PQueue *const pq)
{
    if (!pq)
        return;

    u32 idx = 0, smallest_idx = 0;
    u8 *root_elem, *left_elem, *smallest_elem;

    // Comparing the root with its children. If the root has a lower priority
    // than one of its children, we need to swap them and continue bubbling down.
    // The parent node is the first element of the array, i.e. at index 0. The left
    // one follows at index 1, and the right one at index 2, etc. We iterate until
    // we reach the leaf nodes (i.e. the last level of the tree). Leaf nodes DO NOT
    // have children, so LEFT (and RIGHT) will be out of bounds. Since LEFT is lower
    // than RIGHT, we use it as stopping condition.
    while (LEFT(idx) < pq->size)
    {
        root_elem = (u8 *)(pq->data) + (idx * pq->elem_size);
        left_elem = (u8 *)(pq->data) + (LEFT(idx) * pq->elem_size);
        smallest_idx = idx;

        // If the left child has a higher priority than the root
        if (pq->compare_fn(left_elem, root_elem) < 0)
            smallest_idx = LEFT(idx);

        // Comparing the current smallest (could be the root or the left child) with the right child.
        if (RIGHT(idx) < pq->size && pq->compare_fn((u8 *)(pq->data) + (RIGHT(idx) * pq->elem_size),
                                                    (u8 *)(pq->data) + (smallest_idx * pq->elem_size)) < 0)
            smallest_idx = RIGHT(idx);

        // If the root has a higher priority than both children
        if (smallest_idx == idx)
            break; // We're done.

        // Swapping the root with the child that has a higher priority.
        smallest_elem = (u8 *)(pq->data) + (smallest_idx * pq->elem_size);
        pSwap(root_elem, smallest_elem, pq->elem_size);
        idx = smallest_idx; // Moving down the tree.
    }
}

_PRIVATE inline void *realloc_data(CL_PQueue *const pq, u32 new_capacity)
{
    void *new_data = MEM_Realloc(pq->allocator, pq->data, new_capacity * pq->elem_size);
    if (!new_data)
        return NULL;

    pq->data = new_data;
    pq->capacity = new_capacity;
    return new_data;
}

// PUBLIC FUNCTIONS

CL_PQueue *CL_PQueueAllocWithCapacity(MEM_Allocator *const alloc, u32 capacity, u32 elem_sz,
                                      CL_PQueueCompareFn compare_fn)
{
    if (!alloc || !capacity || !elem_sz || !compare_fn)
        return NULL;

    CL_PQueue *const pq = MEM_Alloc(alloc, sizeof(CL_PQueue));
    if (!pq)
        return NULL;

    *pq = (CL_PQueue){
        .data = MEM_Alloc(alloc, capacity * elem_sz),
        .size = 0,
        .capacity = capacity,
        .elem_size = elem_sz,
        .compare_fn = compare_fn,
        .allocator = alloc,
    };

    if (!pq->data)
    {
        MEM_Free(alloc, pq);
        return NULL;
    }

    return pq;
}

CL_PQueue *CL_PQueueAlloc(MEM_Allocator *const alloc, u32 elem_sz, CL_PQueueCompareFn compare_fn)
{
    return CL_PQueueAllocWithCapacity(alloc, BASE_CAPACITY, elem_sz, compare_fn);
}

void CL_PQueueFree(CL_PQueue *const pq)
{
    if (!pq)
        return;

    MEM_Free(pq->allocator, pq->data);
    MEM_Free(pq->allocator, pq);
}

CL_Error CL_PQueueEnqueue(CL_PQueue *const pq, const void *const elem)
{
    if (!pq || !elem)
        return CL_ERR_INVALID_PARAMS;

    if (pq->size >= pq->capacity)
    {
        const u32 new_capacity = pq->capacity * RESZ_FACTOR;
        if (!realloc_data(pq, new_capacity))
            return CL_ERR_NO_MEMORY;
    }

    // Adding the element at the end (queues are FIFO)
    const u32 offset = pq->size * pq->elem_size;
    memcpy((u8 *)(pq->data) + offset, elem, pq->elem_size);
    pq->size++;

    // Bubbling up the new element to its correct position.
    bubble_up_leaf(pq);
    return CL_ERR_OK;
}

CL_Error CL_PQueueDequeue(CL_PQueue *const pq, void *out_elem)
{
    if (!pq || !out_elem)
        return CL_ERR_INVALID_PARAMS;

    if (!pq->size)
        return CL_ERR_EMPTY;

    // Getting the element with the highest priority (i.e. the root of the tree).
    memcpy(out_elem, pq->data, pq->elem_size);
    pq->size--;

    // Moving the last element to the root and bubbling it down.
    const u32 last_elem_offset = pq->size * pq->elem_size;
    memcpy(pq->data, (u8 *)(pq->data) + last_elem_offset, pq->elem_size);
    bubble_down_root(pq);
    return CL_ERR_OK;
}

CL_Error CL_PQueuePeek(const CL_PQueue *const pq, void *out_elem)
{
    if (!pq || !out_elem)
        return CL_ERR_INVALID_PARAMS;

    if (!pq->size)
        return CL_ERR_EMPTY;

    // Getting the element with the highest priority (i.e. the root of the tree).
    memcpy(out_elem, pq->data, pq->elem_size);
    return CL_ERR_OK;
}

u32 CL_PQueueSize(const CL_PQueue *const pq)
{
    return pq ? pq->size : 0;
}

bool CL_PQueueIsEmpty(const CL_PQueue *const pq)
{
    return pq ? pq->size == 0 : true;
}

/*
// ARRAY

void *CL_PQueueAsArray(const CL_PQueue *const pq)
{
    return pq ? pq->data : NULL;
}
*/