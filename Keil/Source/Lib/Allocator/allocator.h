#ifndef __ALLOCATOR_H
#define __ALLOCATOR_H

#include "types.h"

// TYPES

// Base memory pool structure.
typedef struct __Allocator MEM_Allocator;

// Specifies the alignment of the memory pool (4 bytes)
#define _MEM_POOL_ALIGN4(__name, __sz) __attribute__((aligned(4))) u8 __name[__sz] = {0}

// FUNCTIONS

/// @brief Creates a new allocator from the given memory pool.
/// @param pool Pointer to the memory pool.
/// @param pool_size Size of the memory pool in bytes.
/// @return Pointer to the new allocator or NULL on error.
MEM_Allocator *MEM_Init(void *pool, u32 pool_size);

/// @brief Allocate a block of memory from the pool with at least 'size' bytes.
/// @param ma Pointer to the memory allocator.
/// @param size Size of the block to allocate.
/// @return Pointer to the allocated memory or NULL if no block is available.
void *MEM_Alloc(MEM_Allocator *ma, u32 size);

/// @brief Free a previously allocated block.
/// @param ma Pointer to the memory allocator.
/// @param ptr Pointer to the block to free.
void MEM_Free(MEM_Allocator *ma, void *ptr);

/// @brief Reallocate a previously allocated block to a new size.
/// @param ma Pointer to the memory allocator.
/// @param ptr Pointer to the block to reallocate. If NULL, acts like MEM_Alloc().
/// @param new_size New size of the block. If 0, acts like MEM_Free(); if less
///                 than the current size, shrinks the block; if greater, allocates
///                 a new block, copies the data, and frees the old block.
/// @return Pointer to the reallocated block or NULL on error.
void *MEM_Realloc(MEM_Allocator *ma, void *ptr, u32 new_size);

#endif