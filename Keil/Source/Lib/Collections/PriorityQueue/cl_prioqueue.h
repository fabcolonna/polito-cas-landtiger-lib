#ifndef __CL_PRIOQUEUE_H
#define __CL_PRIOQUEUE_H

#include "allocator.h"
#include "cl_types.h"
#include "types.h"

#include <stdbool.h>

// FORWARD DECLARATIONS
typedef struct __PQueue CL_PQueue;

// MACROS

#define CL_PQUEUE_FOREACH(__type, __name, __queue, ...)                                                                \
    for (u32 __i = 0; __i < CL_PQueueSize(__queue); __i++)                                                             \
    {                                                                                                                  \
        __type *__pq_arr = (__type *)CL_PQueueAsArray(__queue);                                                        \
        __type __name = __pq_arr[__i];                                                                                 \
        __VA_ARGS__                                                                                                    \
    }

// TYPES

/// @brief The comparison function for handling priorities, so the user
///        can choose whether to use a min-heap (i.e. ascending PQ) or a max-heap (i.e. descending PQ).
/// @example For a min-heap, the comparison function should return a negative value if a < b.
///          For a max-heap, the comparison function should return a positive value if a < b.
typedef int (*CL_PQueueCompareFn)(const void *, const void *);

// FUNCTIONS

/// @brief Initializes a priority queue with the allocated memory provided by the user.
/// @param allocator The memory allocator to use.
/// @param capacity The initial capacity of the priority queue.
/// @param elem_sz The size of each element.
/// @param compare_fn The comparison function to use for handling priorities.
/// @return Pointer to the new priority queue or NULL on error.
CL_PQueue *CL_PQueueAllocWithCapacity(MEM_Allocator *const alloc, u32 capacity, u32 elem_sz,
                                      CL_PQueueCompareFn compare_fn);

/// @brief Initializes a priority queue with a default capacity.
/// @param alloc The memory allocator to use.
/// @param elem_sz The size of each element.
/// @param compare_fn The comparison function to use for handling priorities.
/// @return Pointer to the new priority queue or NULL on error.
CL_PQueue *CL_PQueueAlloc(MEM_Allocator *const alloc, u32 elem_sz, CL_PQueueCompareFn compare_fn);

/// @brief Frees the memory previously assigned to the priority queue.
/// @param pq The priority queue to release the memory of.
/// @note CL_PQueue does not take ownership of the elements, so it's the user's responsibility to free them.
void CL_PQueueFree(CL_PQueue *const pq);

/// @brief Adds an element to the priority queue.
/// @param pq The priority queue to add the element to.
/// @param elem The element to add.
/// @return CL_Error The error code.
CL_Error CL_PQueueEnqueue(CL_PQueue *const pq, const void *const elem);

/// @brief Removes the element with the highest priority from the priority queue.
/// @param pq The priority queue to remove the element from.
/// @param out_elem [OUTPUT] The element with the highest priority.
/// @return CL_Error The error code.
CL_Error CL_PQueueDequeue(CL_PQueue *const pq, void *out_elem);

/// @brief Gets the element with the highest priority from the priority queue without removing it.
/// @param pq The priority queue to get the element from.
/// @param out_elem [OUTPUT] The element with the highest priority.
/// @return CL_Error The error code.
CL_Error CL_PQueuePeek(const CL_PQueue *const pq, void *out_elem);

/// @brief Gets the number of elements in the priority queue.
/// @param pq The priority queue to get the number of elements from.
/// @return The number of elements in the priority queue.
u32 CL_PQueueSize(const CL_PQueue *const pq);

/// @brief Checks if the priority queue is empty.
/// @param pq The priority queue to check.
/// @return Whether the priority queue is empty.
bool CL_PQueueIsEmpty(const CL_PQueue *const pq);

/*
// ARRAY ACCESS

/// @brief Returns the priority queue as an array.
/// @param pq The priority queue to get as an array.
/// @return Pointer to the priority queue as an array.
void *CL_PQueueAsArray(const CL_PQueue *const pq);
*/

#endif