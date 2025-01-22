#include "cl_vector.h"

#include <stdlib.h>
#include <string.h>

// TYPE DEFINITION

struct __Vector
{
    void *data;
    u32 size, capacity, elem_size;
    MEM_Allocator *allocator;
};

#define RESZ_FACTOR 2
#define BASE_CAPACITY 10

// PRIVATE FUNCTIONS

_PRIVATE inline void *realloc_data(CL_Vector *const arr, u32 new_capacity)
{
    void *new_data = MEM_Realloc(arr->allocator, arr->data, new_capacity * arr->elem_size);
    if (!new_data)
        return NULL;

    arr->data = new_data;
    arr->capacity = new_capacity;
    return new_data;
}

// PUBLIC FUNCTIONS

CL_Vector *CL_VectorAllocWithCapacity(MEM_Allocator *const alloc, u32 capacity, u32 elem_sz)
{
    if (!alloc || !capacity || !elem_sz)
        return NULL;

    CL_Vector *const arr = MEM_Alloc(alloc, sizeof(CL_Vector));
    if (!arr)
        return NULL;

    *arr = (CL_Vector){
        .data = MEM_Alloc(alloc, capacity * elem_sz),
        .size = 0,
        .capacity = capacity,
        .elem_size = elem_sz,
        .allocator = alloc,
    };

    if (!arr->data)
    {
        MEM_Free(alloc, arr);
        return NULL;
    }

    return arr;
}

CL_Vector *CL_VectorAlloc(MEM_Allocator *const alloc, u32 elem_sz)
{
    return CL_VectorAllocWithCapacity(alloc, BASE_CAPACITY, elem_sz);
}

void CL_VectorFree(CL_Vector *arr)
{
    if (!arr)
        return;

    MEM_Free(arr->allocator, arr->data);
    MEM_Free(arr->allocator, arr);
}

CL_Error CL_VectorPushBack(CL_Vector *const arr, const void *const elem, u32 *out_index)
{
    if (!arr || !elem)
        return CL_ERR_INVALID_PARAMS;

    // Checking if the array can hold the element
    if (arr->size >= arr->capacity)
    {
        const u32 new_cap = arr->capacity * RESZ_FACTOR;
        if (!realloc_data(arr, new_cap))
            return CL_ERR_NO_MEMORY;
    }

    const u32 offset = arr->size * arr->elem_size;
    memcpy((u8 *)(arr->data) + offset, elem, arr->elem_size);
    arr->size++;

    if (out_index)
        *out_index = arr->size - 1;

    return CL_ERR_OK;
}

CL_Error CL_VectorPushFront(CL_Vector *const arr, const void *const elem)
{
    // Inserting at the front is equivalent to inserting at index 0
    return CL_VectorInsert(arr, elem, 0);
}

void CL_VectorPopBack(CL_Vector *const arr, void *out_elem)
{
    if (!arr || !arr->size)
        return;

    const u32 offset = (arr->size - 1) * arr->elem_size;
    if (out_elem)
        memcpy(out_elem, (u8 *)(arr->data) + offset, arr->elem_size);

    // memset((u8 *)(arr->data) + offset, 0, arr->elem_size); // Not necessary
    arr->size--;
}

void CL_VectorPopFront(CL_Vector *const arr, void *out_elem)
{
    if (!arr || !arr->size)
        return;

    if (out_elem)
        CL_VectorGet(arr, 0, out_elem);

    CL_VectorRemove(arr, 0);
}

CL_Error CL_VectorGet(const CL_Vector *const arr, u32 index, void *out_elem)
{
    if (!arr || !out_elem)
        return CL_ERR_INVALID_PARAMS;

    if (index >= arr->size || index < 0)
        return CL_ERR_OUT_OF_BOUNDS;

    if (arr->size == 0)
        return CL_ERR_EMPTY;

    const u32 offset = index * arr->elem_size;
    memcpy(out_elem, (u8 *)(arr->data) + offset, arr->elem_size);
    return CL_ERR_OK;
}

CL_Error CL_VectorGetPtr(const CL_Vector *const arr, u32 index, void **out_elem)
{
    if (!arr || !out_elem)
        return CL_ERR_INVALID_PARAMS;

    if (index >= arr->size || index < 0)
        return CL_ERR_OUT_OF_BOUNDS;

    if (arr->size == 0)
        return CL_ERR_EMPTY;

    const u32 offset = index * arr->elem_size;
    *out_elem = (u8 *)(arr->data) + offset;
    return CL_ERR_OK;
}

CL_Error CL_VectorGetLast(const CL_Vector *const arr, void *out_elem)
{
    if (arr->size == 0)
        return CL_ERR_EMPTY;

    return CL_VectorGet(arr, arr->size - 1, out_elem);
}

CL_Error CL_VectorGetLastPtr(const CL_Vector *const arr, void **out_elem)
{
    if (arr->size == 0)
        return CL_ERR_EMPTY;

    return CL_VectorGetPtr(arr, arr->size - 1, out_elem);
}

CL_Error CL_VectorInsert(CL_Vector *const arr, const void *const elem, u32 index)
{
    if (!arr || !elem)
        return CL_ERR_INVALID_PARAMS;

    if (index > arr->size || index < 0)
        return CL_ERR_OUT_OF_BOUNDS;

    if (arr->size >= arr->capacity)
    {
        const u32 new_cap = arr->capacity * RESZ_FACTOR;
        if (!realloc_data(arr, new_cap))
            return CL_ERR_NO_MEMORY;
    }

    const u32 where_offset = index * arr->elem_size;
    const u32 next_offset = where_offset + arr->elem_size;

    // Shift the elements to the right, in order to make space for the new element
    memmove((u8 *)arr->data + next_offset, (u8 *)arr->data + where_offset, (arr->size - index) * arr->elem_size);
    memcpy((u8 *)arr->data + where_offset, elem, arr->elem_size);
    arr->size++;
    return CL_ERR_OK;
}

CL_Error CL_VectorRemove(CL_Vector *const arr, u32 index)
{
    if (!arr)
        return CL_ERR_INVALID_PARAMS;

    if (index >= arr->size || index < 0)
        return CL_ERR_OUT_OF_BOUNDS;

    if (arr->size == 0)
        return CL_ERR_EMPTY;

    const u32 where_offset = index * arr->elem_size;
    const u32 next_offset = where_offset + arr->elem_size;

    // Shift the elements to the left, in order to remove the element
    memmove((u8 *)arr->data + where_offset, (u8 *)arr->data + next_offset, (arr->size - index - 1) * arr->elem_size);
    arr->size--;
    return CL_ERR_OK;
}

void CL_VectorClear(CL_Vector *const arr)
{
    if (!arr || !arr->size)
        return;

    // memset(arr->data, 0, arr->size * arr->elem_size); // Not necessary
    arr->size = 0;
}

bool CL_VectorIsEmpty(const CL_Vector *const arr)
{
    return arr ? arr->size == 0 : true;
}

u32 CL_VectorSize(const CL_Vector *const arr)
{
    return arr ? arr->size : 0;
}

u32 CL_VectorCapacity(const CL_Vector *const arr)
{
    return arr ? arr->capacity : 0;
}

void CL_VectorSort(CL_Vector *const arr, CL_CompareFn cmp)
{
    if (!arr || !cmp || arr->size < 2)
        return;

    qsort(arr->data, arr->size, arr->elem_size, cmp);
}

bool CL_VectorSearch(const CL_Vector *const arr, const void *elem, CL_CompareFn compare_fn, u32 *out_index)
{
    if (!arr || !elem || !compare_fn)
        return false;

    u32 offset;
    for (u32 i = 0; i < arr->size; i++)
    {
        offset = i * arr->elem_size;
        if (!compare_fn(elem, (u8 *)(arr->data) + offset))
        {
            if (out_index)
                *out_index = i;

            return true;
        }
    }

    return false;
}