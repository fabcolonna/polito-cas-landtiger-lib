#include "glcd_mem.h"
#include <stdlib.h>

#define IS_ARENA_OK (arena_ready && current_arena.objs && current_arena.capacity > 0)

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
    const u32 size_needed = sizeof(LCD_ObjSlot) + sizeof(LCD_Obj) + (comps_size * sizeof(LCD_Component));

    // Checking if something has been free-d and is big enough for the new obj
    while (ptr < end)
    {
        LCD_ObjSlot *slot = (LCD_ObjSlot *)ptr;
        if (!slot->used && !slot->obj && slot->size >= size_needed)
            return slot;

        ptr += slot->size;
    }

    // If here, no free slot was found, so we need to create a new one,
    // provided that there's enough space in the arena.
    if (current_arena.offset + size_needed > current_arena.capacity)
        return NULL;

    LCD_ObjSlot *new_slot = (LCD_ObjSlot *)ptr;
    new_slot->obj = (LCD_Obj *)(ptr + sizeof(LCD_ObjSlot));
    new_slot->size = size_needed;
    new_slot->used = true;

    // Initializing the object itself
    new_slot->obj->comps_size = comps_size;
    new_slot->obj->comps = (LCD_Component *)((u8 *)new_slot->obj + sizeof(LCD_Obj));
    current_arena.offset += size_needed;
    return new_slot;
}

_PRIVATE LCD_MAError free_object(LCD_Obj *obj)
{
    if (!obj)
        return LCD_MA_ERR_INVALID_OBJECT;

    // Moving backwards from the object to the slot that wraps it
    LCD_ObjSlot *slot = (LCD_ObjSlot *)((u8 *)obj - sizeof(LCD_ObjSlot));

    // Checking if the object is in the arena
    const u8 *start = (u8 *)current_arena.objs;
    const u8 *end = start + current_arena.offset;
    if ((u8 *)slot < start || (u8 *)slot >= end)
        return LCD_MA_ERR_INVALID_OBJECT;

    slot->used = false;
    slot->obj->comps = NULL;
    slot->obj->comps_size = 0;
    slot->obj = NULL; // Just to be sure, but it's not really necessary

    // We should decrement the offset, if this was the last object in the arena
    // and there are no more objects after it.
    if ((u8 *)slot + slot->size == end)
        current_arena.offset -= slot->size;

    return LCD_MA_ERR_NONE;
}

// PUBLIC FUNCTIONS

LCD_MemoryArena *LCD_MAUseMemory(void *const memory, u32 capacity)
{
    if (!memory || capacity <= 0)
        return NULL;

    current_arena.objs = memory;
    current_arena.capacity = capacity;
    current_arena.offset = 0;

    arena_ready = true;
    return &current_arena;
}

LCD_MAError LCD_MAReleaseMemory(LCD_MemoryArena *arena)
{
    if (!arena || arena != &current_arena)
        return LCD_MA_ERR_INVALID_ARENA;

    current_arena.objs = NULL;
    current_arena.capacity = 0;
    current_arena.offset = 0;

    arena_ready = false;
    arena = NULL;
    return LCD_MA_ERR_NONE;
}

LCD_MAError LCD_MAAllocObject(u16 comps_size, LCD_Obj **out_obj)
{
    if (!IS_ARENA_OK)
    {
        *out_obj = NULL;
        return LCD_MA_ERR_INVALID_ARENA;
    }

    LCD_ObjSlot *slot = find_free_obj_slot(comps_size);
    if (!slot)
    {
        *out_obj = NULL;
        return LCD_MA_ERR_NOT_ENOUGH_SPACE;
    }

    *out_obj = slot->obj;
    return LCD_MA_ERR_NONE;
}

LCD_MAError LCD_MAFreeObject(LCD_Obj *obj)
{
    if (!IS_ARENA_OK)
        return LCD_MA_ERR_INVALID_ARENA;

    return free_object(obj);
}
