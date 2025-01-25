#ifndef __CL_VECTOR_H
#define __CL_VECTOR_H

#include "allocator.h"
#include "cl_types.h"
#include "types.h"

#include <stdbool.h>

// FORWARD DECLARATIONS
typedef struct __Vector CL_Vector;

// MACROS

/// @brief Macro to iterate over the elements of the vector.
/// @param __type The type of the elements in the vector.
/// @param __name The name of the variable to hold the current element.
/// @param __arr The vector to iterate over.
/// @param ... The code to execute for each element.
#define CL_VECTOR_FOREACH(__type, __name, __arr, ...)                                                                  \
    for (u32 __i = 0; __i < CL_VectorSize(__arr); __i++)                                                               \
    {                                                                                                                  \
        __type __name;                                                                                                 \
        CL_VectorGet(__arr, __i, (void *)&__name);                                                                     \
        __VA_ARGS__                                                                                                    \
    }

/// @brief Macro to iterate over the elements of the vector with a pointer to the current element.
/// @param __type The type of the elements in the vector.
/// @param __name The name of the variable to hold the current element.
/// @param __arr The vector to iterate over.
/// @param ... The code to execute for each element.
#define CL_VECTOR_FOREACH_PTR(__type, __name, __arr, ...)                                                              \
    for (u32 __i = 0; __i < CL_VectorSize(__arr); __i++)                                                               \
    {                                                                                                                  \
        __type *__name;                                                                                                \
        CL_VectorGetPtr(__arr, __i, (void **)&__name);                                                                 \
        __VA_ARGS__                                                                                                    \
    }

// FUNCTIONS

/// @brief Initializes the vector with allocated memory provided by the user.
/// @param alloc The allocator to use for memory allocation.
/// @param capacity The initial capacity of the vector.
/// @param elem_sz The size of each element in the vector.
/// @return Pointer to the new vector or NULL on error.
CL_Vector *CL_VectorAllocWithCapacity(MEM_Allocator *const alloc, u32 capacity, u32 elem_sz);

/// @brief Initializes the vector with a default capacity.
/// @param alloc The allocator to use for memory allocation.
/// @param elem_sz The size of each element in the vector.
/// @return Pointer to the new vector or NULL on error.
CL_Vector *CL_VectorAlloc(MEM_Allocator *const alloc, u32 elem_sz);

/// @brief Frees the memory previously assigned to the vector.
/// @param arr The vector to release the memory of.
/// @note CL_Vector does not take ownership of the elements, so it's the user's responsibility to free them.
void CL_VectorFree(CL_Vector *const arr);

/// @brief Adds an element at the end of the vector.
/// @param arr The vector to add the element to.
/// @param elem The element to add.
/// @param out_index [OUTPUT] The index of the added element, if not NULL.
/// @return CL_Error The error code.
CL_Error CL_VectorPushBack(CL_Vector *const arr, const void *const elem, u32 *out_index);

/// @brief Adds an element at the front of the vector.
/// @param arr The vector to add the element to.
/// @param elem The element to add.
/// @return CL_Error The error code.
/// @note This operation is not O(1) for vectors. Consider using a list if you need frequent insertions at the front.
CL_Error CL_VectorPushFront(CL_Vector *const arr, const void *const elem);

/// @brief Removes the last element from the vector, optionally returning it.
/// @param arr The vector to remove the last element from.
/// @param out_elem [OUTPUT] The removed element, if not NULL.
void CL_VectorPopBack(CL_Vector *const arr, void *out_elem);

/// @brief Removes the first element from the vector, optionally returning it.
/// @param arr The vector to remove the first element from.
/// @param out_elem [OUTPUT] The removed element, if not NULL.
/// @note This operation is not O(1) for vectors. Consider using a list if you need frequent removals at the front.
void CL_VectorPopFront(CL_Vector *const arr, void *out_elem);

/// @brief Gets an element at the specified index.
/// @param arr The vector to get the element from.
/// @param index The index of the element to get.
/// @param out_elem [OUTPUT] The element at the specified index.
/// @return CL_Error The error code.
CL_Error CL_VectorGet(const CL_Vector *const arr, u32 index, void *out_elem);

/// @brief Gets a pointer to an element at the specified index.
/// @param arr The vector to get the element from.
/// @param index The index of the element to get.
/// @param out_elem [OUTPUT] The pointer to the element at the specified index.
/// @return CL_Error The error code.
CL_Error CL_VectorGetPtr(const CL_Vector *const arr, u32 index, void **out_elem);

/// @brief Gets the last element from the vector.
/// @param arr The vector to get the last element from.
/// @param out_elem [OUTPUT] The last element.
/// @return CL_Error The error code.
CL_Error CL_VectorGetLast(const CL_Vector *const arr, void *out_elem);

/// @brief Gets a pointer to the last element from the vector.
/// @param arr The vector to get the last element from.
/// @param out_elem [OUTPUT] The pointer to the last element.
/// @return CL_Error The error code.
CL_Error CL_VectorGetLastPtr(const CL_Vector *const arr, void **out_elem);

/// @brief Inserts an element at the specified index.
/// @param arr The vector to insert the element to.
/// @param elem The element to insert.
/// @param index The index to insert the element at.
/// @return CL_Error The error code.
/// @note This operation is not O(1) for vectors. Consider using a list if you need frequent insertions.
CL_Error CL_VectorInsert(CL_Vector *const arr, const void *const elem, u32 index);

/// @brief Removes an element at the specified index.
/// @param arr The vector to remove the element from.
/// @param index The index to remove the element from.
/// @return CL_Error The error code.
/// @note This operation is not O(1) for vectors. Consider using a list if you need frequent removald in the middle.
CL_Error CL_VectorRemove(CL_Vector *const arr, u32 index);

/// @brief Clears the vector.
/// @param arr The vector to clear.
void CL_VectorClear(CL_Vector *const arr);

/// @brief Checks if the vector is empty.
/// @param arr The vector to check.
/// @return true if the vector is empty, false otherwise.
bool CL_VectorIsEmpty(const CL_Vector *const arr);

/// @brief Gets the size of the vector.
/// @param arr The vector to get the size of.
/// @return The size of the vector.
u32 CL_VectorSize(const CL_Vector *const arr);

/// @brief Gets the capacity of the vector.
/// @param arr The vector to get the capacity of.
/// @return The capacity of the vector.
u32 CL_VectorCapacity(const CL_Vector *const arr);

/// @brief Sorts the vector.
/// @param arr The vector to sort.
/// @param compare_fn The comparison function.
void CL_VectorSort(CL_Vector *arr, CL_CompareFn compare_fn);

/// @brief Searches for an element in the vector.
/// @param arr The vector to search in.
/// @param elem The element to search for.
/// @param compare_fn The comparison function.
/// @param out_index [OUTPUT] The index of the element, if found and not NULL.
/// @return true if the element was found, false otherwise.
bool CL_VectorSearch(const CL_Vector *const arr, const void *elem, CL_CompareFn compare_fn, u32 *out_index);

#endif