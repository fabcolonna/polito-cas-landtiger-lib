#ifndef __CL_LIST_H
#define __CL_LIST_H

#include "allocator.h"
#include "cl_types.h"
#include "types.h"

#include <stdbool.h>

// FORWARD DECLARATIONS
typedef struct __List CL_List;

// MACROS

/// @brief Macro to iterate over the elements of the list.
/// @param __type The type of the elements in the list.
/// @param __name The name of the variable to hold the current element.
/// @param __list The list to iterate over.
/// @param ... The code to execute for each element.
#define CL_LIST_FOREACH(__type, __name, __list, ...)                                                                   \
    for (u32 __i = 0; __i < CL_ListSize(__list); __i++)                                                                \
    {                                                                                                                  \
        __type __name;                                                                                                 \
        CL_ListGet(__list, __i, &(__name));                                                                            \
        __VA_ARGS__                                                                                                    \
    }

/// @brief Macro to iterate over the elements of the list with a pointer to the current element.
/// @param __type The type of the elements in the list.
/// @param __name The name of the variable to hold the current element.
/// @param __list The list to iterate over.
/// @param ... The code to execute for each element.
#define CL_LIST_FOREACH_PTR(__type, __name, __list, ...)                                                               \
    for (u32 __i = 0; __i < CL_ListSize(__list); __i++)                                                                \
    {                                                                                                                  \
        __type *const __name;                                                                                          \
        CL_ListGetPtr(__list, __i, (void **)&(__name));                                                                \
        __VA_ARGS__                                                                                                    \
    }

// FUNCTIONS

/// @brief Initializes a new doubly-linked list with tail, with the allocated memory provided by the user.
/// @param alloc The allocator to use for memory allocation.
/// @param elem_sz The size of each element in the list.
/// @return Pointer to the new list or NULL on error.
CL_List *CL_ListAlloc(MEM_Allocator *const alloc, u32 elem_sz);

/// @brief Frees the memory previously assigned to the list.
/// @param list The list to release the memory of.
/// @note CL_List does not take ownership of the elements, so it's the user's responsibility to free them.
void CL_ListFree(CL_List *const list);

/// @brief Adds an element at the end of the list.
/// @param list The list to add the element to.
/// @param elem The element to add.
/// @return CL_Error The error code.
CL_Error CL_ListPushBack(CL_List *const list, const void *const elem);

/// @brief Adds an element at the front of the list.
/// @param list The list to add the element to.
/// @param elem The element to add.
/// @return CL_Error The error code.
CL_Error CL_ListPushFront(CL_List *const list, const void *const elem);

/// @brief Removes the last element from the list, optionally returning it.
/// @param list The list to remove the last element from.
/// @param out_elem [OUTPUT] The removed element, if not NULL.
void CL_ListPopBack(CL_List *const list, void *out_elem);

/// @brief Removes the first element from the list, optionally returning it.
/// @param list The list to remove the first element from.
/// @param out_elem [OUTPUT] The removed element, if not NULL.
void CL_ListPopFront(CL_List *const list, void *out_elem);

/// @brief Gets an element at the specified index.
/// @param list The list to get the element from.
/// @param index The index of the element to get.
/// @param out_elem [OUTPUT] The element at the specified index.
/// @return CL_Error The error code.
/// @note This operation is not O(1) for lists. Consider using a vector if you need random access.
CL_Error CL_ListGet(const CL_List *const list, u32 index, void *out_elem);

/// @brief Gets an element at the specified index as a pointer, so the user can modify it.
/// @param list The list to get the element from.
/// @param index The index of the element to get.
/// @param out_elem [OUTPUT] A pointer to the element at the specified index.
/// @return CL_Error The error code.
/// @note This operation is not O(1) for lists. Consider using a vector if you need random access.
CL_Error CL_ListGetPtr(const CL_List *const list, u32 index, void **out_elem);

/// @brief Gets the last element from the list.
/// @param list The list to get the last element from.
/// @param out_elem [OUTPUT] The last element.
/// @return CL_Error The error code.
CL_Error CL_ListGetLast(const CL_List *const list, void *out_elem);

/// @brief Gets the last element from the list as a pointer, so the user can modify it.
/// @param list The list to get the last element from.
/// @param out_elem [OUTPUT] The last element, as a pointer.
/// @return CL_Error The error code.
CL_Error CL_ListGetLastPtr(const CL_List *const list, void **out_elem);

/// @brief Inserts an element at the specified index.
/// @param list The list to insert the element to.
/// @param index The index to insert the element at.
/// @param elem The element to insert.
/// @return CL_Error The error code.
/// @note This operation is not O(1) for lists. Consider using a vector if you need random access.
CL_Error CL_ListInsertAt(CL_List *const list, u32 index, const void *const elem);

/// @brief Removes an element at the specified index.
/// @param list The list to remove the element from.
/// @param index The index of the element to remove.
/// @param out_elem [OUTPUT] The removed element, if not NULL.
/// @return CL_Error The error code.
/// @note This operation is not O(1) for lists. Consider using a vector if you need random access.
CL_Error CL_ListRemoveAt(CL_List *const list, u32 index, void *out_elem);

/// @brief Searches for an element in the list.
/// @param list The list to search in.
/// @param elem The element to search for.
/// @param compare_fn The comparison function.
/// @param out_elem [OUTPUT] The index of the element, if found and not NULL.
/// @return true if the element was found, false otherwise.
bool CL_ListSearch(const CL_List *const list, const void *const elem, CL_CompareFn compare_fn, u32 *out_index);

/// @brief Removes all elements from the list.
/// @param list The list to clear.
void CL_ListClear(CL_List *const list);

/// @brief Gets the number of elements in the list.
/// @param list The list to get the size of.
/// @return The number of elements in the list.
u32 CL_ListSize(const CL_List *const list);

/// @brief Checks if the list is empty.
/// @param list The list to check.
/// @return true if the list is empty, false otherwise.
bool CL_ListIsEmpty(const CL_List *const list);

#endif