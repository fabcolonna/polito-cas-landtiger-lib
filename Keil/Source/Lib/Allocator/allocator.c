#include "allocator.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// PRIVATE TYPES

// Each allocated block in the pool begins with one of these headers.
typedef struct __BlockHeader
{
    u32 size;                   // Size of the block (payload only), in bytes.
    bool free;                  // True if this block is free.
    struct __BlockHeader *next; // Pointer to the next block in the pool.
} __BlockHeader;

struct __Allocator
{
    void *pool;          // Pointer to the memory pool.
    u32 pool_size;       // Total size of the pool in bytes.
    u32 pool_used;       // Total size of the pool used.
    __BlockHeader *head; // Pointer to the first block.
};

// PRIVATE VARIABLES

_PRIVATE MEM_Allocator sAllocator = {0};

// PUBLIC FUNCTIONS

MEM_Allocator *MEM_Init(void *pool, u32 pool_size)
{
    if (!pool || pool_size < sizeof(__BlockHeader))
        return NULL;

    sAllocator = (struct __Allocator){
        .pool = pool,
        .pool_size = pool_size,
        .pool_used = 0,
        .head = (__BlockHeader *)pool,
    };

    sAllocator.head->size = pool_size - sizeof(__BlockHeader);
    sAllocator.head->free = true;
    sAllocator.head->next = NULL;

    return &sAllocator;
}

#define ALIGN4_VAL(x) (((x) + 3) & ~3)

void *MEM_Alloc(MEM_Allocator *ma, u32 size)
{
    if (!ma || ma != &sAllocator || size == 0)
        return NULL;

    // Optional: Align requested size to 4 bytes.
    size = ALIGN4_VAL(size);

    __BlockHeader *current_blk = ma->head, *new_blk;
    const u32 min_split_size = sizeof(__BlockHeader) + 4; // minimal payload of 4 bytes
    while (current_blk)
    {
        if (current_blk->free && current_blk->size >= size)
        {
            // Found a suitable free block. If the remaining space after allocation
            // is large enough for a new block header and a minimal payload (4 bytes),
            // split the block.
            if (current_blk->size >= size + min_split_size)
            {
                // Create the new block header in the remaining space after the allocation.
                new_blk = (__BlockHeader *)((u8 *)current_blk + sizeof(__BlockHeader) + size);
                *new_blk = (__BlockHeader){
                    .size = current_blk->size - size - sizeof(__BlockHeader), // Remaining size.
                    .free = true,
                    .next = current_blk->next,
                };

                // Update current block.
                current_blk->size = size;
                current_blk->free = false;
                current_blk->next = new_blk;
            }
            else
                current_blk->free = false;

            ma->pool_used += current_blk->size + sizeof(__BlockHeader);
            return (void *)((u8 *)current_blk + sizeof(__BlockHeader));
        }

        current_blk = current_blk->next;
    }

    // No suitable block found.
    return NULL;
}

void MEM_Free(MEM_Allocator *ma, void *ptr)
{
    if (!ma || ma != &sAllocator || !ptr)
        return;

    // Get the block header stored just before the user pointer.
    __BlockHeader *const blk = (__BlockHeader *)((u8 *)ptr - sizeof(__BlockHeader));
    blk->free = true;
    ma->pool_used -= blk->size + sizeof(__BlockHeader);

    // Coalesce with the next block if it's free.
    if (blk->next && blk->next->free)
    {
        blk->size += sizeof(__BlockHeader) + blk->next->size; // Add the size of the next block.
        blk->next = blk->next->next;                          // Skip the next block, pointing to the one after it.
    }

    // Also try to coalesce with any previous block.
    // Moving to the block before the one to be freed.
    __BlockHeader *current_blk = ma->head, *prev;
    while (current_blk && current_blk != blk)
    {
        prev = current_blk;
        current_blk = current_blk->next;
    }

    if (prev && prev->free)
    {
        prev->size += sizeof(__BlockHeader) + blk->size; // Add the size of the block to be freed.
        prev->next = blk->next;                          // Skip the block to be freed, pointing to the one after it.
    }
}

void *MEM_Realloc(MEM_Allocator *ma, void *ptr, u32 new_size)
{
    if (!ma || ma != &sAllocator)
        return NULL;

    if (!new_size)
    {
        // If size is 0, free the block and return NULL.
        MEM_Free(ma, ptr);
        return NULL;
    }

    if (!ptr) // If ptr is NULL, simply allocate a new block.
        return MEM_Alloc(ma, new_size);

    // Align the requested new size to 4 bytes.
    new_size = ALIGN4_VAL(new_size);

    // Get the header for the block.
    __BlockHeader *blk = (__BlockHeader *)((u8 *)ptr - sizeof(__BlockHeader));
    if (new_size <= blk->size)
    {
        // The block is already big enough. If the new size is much smaller,
        // we can split the block to free up unused memory.
        const u32 min_split_thresh = sizeof(__BlockHeader) + 8; // minimal payload of 8 bytes
        if (blk->size >= new_size + min_split_thresh)
        {
            // Splitting the block. Create a new block header in the remaining space.
            __BlockHeader *const new_blk = (__BlockHeader *)((u8 *)blk + sizeof(__BlockHeader) + new_size);
            *new_blk = (__BlockHeader){
                .size = blk->size - new_size - sizeof(__BlockHeader),
                .free = true,
                .next = blk->next, // Link to the next block
            };

            blk->size = new_size;
            blk->next = new_blk; // Link to the newly created block
        }

        return ptr; // Returing the same pointer.
    }
    else
    {
        // The new size is larger than the current block. Need to move the data to a bigger block.
        void *new_ptr = MEM_Alloc(ma, new_size);
        if (!new_ptr)
            return NULL;

        memcpy(new_ptr, ptr, blk->size);
        MEM_Free(ma, ptr);
        return new_ptr;
    }
}