#include "glcd_mem.h"
#include <stdlib.h>

// TODO: Implement a more efficient way to get free slots, maybe using a free list
//       instead of a linear search performed every time an allocation is requested.

#define IS_ARENA_OK (arena_ready && arena.objs && arena.capacity > 0)
#define ALLOC_SIZE(comps_num) (sizeof(LCD_ObjSlot) + sizeof(LCD_Obj) + comps_num * sizeof(LCD_Component))

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
_PRIVATE LCD_MemoryArena arena = {0};

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
    // Calculating the size needed & aligning it to 4 bytes
    const u32 size_needed = ALIGN_VAL_4(ALLOC_SIZE(comps_size));

    u8 *ptr = (u8 *)arena.objs;
    const u8 *end = ptr + arena.offset;

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
    if (!found && (arena.offset + size_needed > arena.capacity))
        return NULL;

    slot = (LCD_ObjSlot *)ALIGN_PTR_4(ptr);
    slot->obj = (LCD_Obj *)(ptr + sizeof(LCD_ObjSlot));
    slot->size = size_needed;
    slot->used = true;

    // Initializing the object itself
    slot->obj->comps_size = comps_size;
    slot->obj->comps = (LCD_Component *)ALIGN_PTR_4((u8 *)slot->obj + sizeof(LCD_Obj));
    arena.offset += size_needed;
    return slot;
}

_PRIVATE LCD_Error free_object(LCD_Obj *obj)
{
    if (!obj)
        return LCD_ERR_MA_ATTEMPT_TO_FREE_INVALID_OBJ;

    // Moving backwards from the object to the slot that wraps it
    LCD_ObjSlot *const slot = (LCD_ObjSlot *)((u8 *)obj - sizeof(LCD_ObjSlot));

    // Checking if the object is in the arena
    const u8 *start = (u8 *)arena.objs;
    const u8 *end = start + arena.offset;
    if ((u8 *)slot < start || (u8 *)slot >= end)
        return LCD_ERR_MA_ATTEMPT_TO_FREE_INVALID_OBJ;

    slot->used = false;
    slot->obj->comps = NULL;
    slot->obj->comps_size = 0;

    // We should decrement the offset, if this was the last object in the arena
    // and there are no more objects after it.
    if ((u8 *)slot + slot->size == end)
        arena.offset -= slot->size;

    return LCD_ERR_OK;
}

// PUBLIC FUNCTIONS

const LCD_MemoryArena *LCD_MAUseMemory(void *const memory, u32 capacity)
{
    // Memory needs to be aligned to 4 bytes.
    if (!memory || capacity <= 0 || (uintptr_t)memory % 4 != 0)
        return NULL;

    arena = (LCD_MemoryArena){.objs = memory, .capacity = capacity, .offset = 0};
    arena_ready = true;
    return &arena;
}

bool LCD_MAIsArenaReady(const LCD_MemoryArena *const p_arena)
{
    // Checks if the current arena is the one specified as param
    if (p_arena != &arena)
        return false;

    return arena_ready;
}

LCD_Error LCD_MAReleaseMemory(const LCD_MemoryArena *const p_arena)
{
    if (!p_arena || p_arena != &arena)
        return LCD_ERR_MA_INVALID_ARENA;

    arena = (LCD_MemoryArena){0};
    arena_ready = false;
    return LCD_ERR_OK;
}

LCD_Error LCD_MAAllocObject(u16 comps_size, LCD_Obj **out_obj)
{
    if (!IS_ARENA_OK)
    {
        *out_obj = NULL;
        return LCD_ERR_MA_INVALID_ARENA;
    }

    const LCD_ObjSlot *const slot = find_free_obj_slot(comps_size);
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
