#include "glcd_mem.h"
#include <stdlib.h>

// TODO: Implement a more efficient way to get free slots, maybe using a free list
//       instead of a linear search performed every time an allocation is requested.

#define IS_ARENA_OK (arena_ready && current_arena.objs && current_arena.capacity > 0)

// Memory needs to be aligned to 4 bytes, since unaligned memory access is not allowed on ARM
// and cause a hard fault (usage fault with the UNALIGNED bit set).
#define ALIGN_PTR_4(ptr) ((u8 *)(((uintptr_t)(ptr) + (3)) & ~(3)))
#define ALIGN_VAL_4(val) (((val + 3) / 4) * 4)

// PRIVATE VARIABLES & TYPES

typedef struct
{
    LCD_Obj *obj;
    u32 size;
    bool used;
} LCD_ObjSlot;

struct _lcd_mem_arena
{
    void *objs;
    u16 capacity, offset;
};

_PRIVATE bool arena_ready = false;
_PRIVATE LCD_MemoryArena current_arena = {0};

// PRIVATE FUNCTIONS

/// @brief Searchs the arena from the beginning to the current offset (i.e. the last object allocated)
///        to look for an object slot that was previously freed, and whose size is >= than the asked one.
///        If a slot is found, it's returned WITHOUT incrementing the current offset, otherwise, provided
///        that there's enough space in the arena, a new slot is created at the current offset, which is
///        then incremented by the size of that new slot.
/// @param comps_size The size of the components array that the object will have.
/// @return A pointer to the first free object, or NULL if there's not enough space.
_PRIVATE LCD_ObjSlot *find_free_obj_slot(u16 comps_size)
{
    u8 *ptr = (u8 *)current_arena.objs;
    const u8 *end = ptr + current_arena.offset;

    // Calculating the size needed & aligning it to 4 bytes
    const u32 size_needed = ALIGN_VAL_4(sizeof(LCD_ObjSlot) + sizeof(LCD_Obj) + (comps_size * sizeof(LCD_Component)));

    // Coalescing free slots
    bool found = false;
    LCD_ObjSlot *slot, *next;
    while (ptr < end && !found)
    {
        slot = (LCD_ObjSlot *)ptr;
        if (!slot->used)
        {
            // Check if the next slot is also free
            next = (LCD_ObjSlot *)(ptr + slot->size);
            if ((u8 *)next < end && !next->used)
            {
                slot->size += next->size; // Coalesce slots
                continue;
            }

            // Checking if the slot is big enough
            if (slot->size >= size_needed)
            {
                found = true;
                break; // Now ptr points to the slot that will be used
            }
        }

        ptr += slot->size;
        ptr = ALIGN_PTR_4(ptr);
    }

    // If here, no free slot was found, so we need to create a new one,
    // provided that there's enough space in the arena.
    if (!found && (current_arena.offset + size_needed > current_arena.capacity))
        return NULL;

    slot = (LCD_ObjSlot *)ALIGN_PTR_4(ptr);
    slot->obj = (LCD_Obj *)(ptr + sizeof(LCD_ObjSlot));
    slot->size = size_needed;
    slot->used = true;

    // Initializing the object itself
    slot->obj->comps_size = comps_size;
    slot->obj->comps = (LCD_Component *)ALIGN_PTR_4((u8 *)slot->obj + sizeof(LCD_Obj));
    current_arena.offset += size_needed;
    return slot;
}

_PRIVATE LCD_Error free_object(LCD_Obj *obj)
{
    if (!obj)
        return LCD_ERR_MA_ATTEMPT_TO_FREE_INVALID_OBJ;

    // Moving backwards from the object to the slot that wraps it
    LCD_ObjSlot *slot = (LCD_ObjSlot *)((u8 *)obj - sizeof(LCD_ObjSlot));

    // Checking if the object is in the arena
    const u8 *start = (u8 *)current_arena.objs;
    const u8 *end = start + current_arena.offset;
    if ((u8 *)slot < start || (u8 *)slot >= end)
        return LCD_ERR_MA_ATTEMPT_TO_FREE_INVALID_OBJ;

    slot->used = false;
    slot->obj->comps = NULL;
    slot->obj->comps_size = 0;
    slot->obj = NULL; // Just to be sure, but it's not really necessary

    // We should decrement the offset, if this was the last object in the arena
    // and there are no more objects after it.
    if ((u8 *)slot + slot->size == end)
        current_arena.offset -= slot->size;

    return LCD_ERR_OK;
}

// PUBLIC FUNCTIONS

LCD_MemoryArena *LCD_MAUseMemory(void *const memory, u32 capacity)
{
    if (!memory || capacity <= 0)
        return NULL;

    // Checking if memory is aligned to 4 bytes
    if ((uintptr_t)memory % 4 != 0)
        return NULL;

    current_arena.objs = memory;
    current_arena.capacity = capacity;
    current_arena.offset = 0;

    arena_ready = true;
    return &current_arena;
}

LCD_Error LCD_MAReleaseMemory(LCD_MemoryArena *arena)
{
    if (!arena || arena != &current_arena)
        return LCD_ERR_MA_INVALID_ARENA;

    current_arena.objs = NULL;
    current_arena.capacity = 0;
    current_arena.offset = 0;

    arena_ready = false;
    arena = NULL;
    return LCD_ERR_OK;
}

LCD_Error LCD_MAAllocObject(u16 comps_size, LCD_Obj **out_obj)
{
    if (!IS_ARENA_OK)
    {
        *out_obj = NULL;
        return LCD_ERR_MA_INVALID_ARENA;
    }

    LCD_ObjSlot *slot = find_free_obj_slot(comps_size);
    if (!slot)
    {
        *out_obj = NULL;
        return LCD_ERR_MA_NOT_ENOUGH_SPACE;
    }

    *out_obj = slot->obj;
    return LCD_ERR_OK;
}

LCD_Error LCD_MAFreeObject(LCD_Obj *obj)
{
    if (!IS_ARENA_OK)
        return LCD_ERR_MA_INVALID_ARENA;

    return free_object(obj);
}
