#ifndef GLCD_MEM_ARENA_H
#define GLCD_MEM_ARENA_H

#include "glcd_types.h"
#include "utils.h"

// MACRO FUNCTIONS

#define LCD_MA_ALLOC_STATIC_MEMORY(name, size) static u8 name[size];

// TYPES

// Forward declaration of LCD_MemoryArena type
typedef struct _lcd_mem_arena LCD_MemoryArena;

typedef enum
{
    LCD_MA_ERR_NONE = 0,
    LCD_MA_ERR_INVALID_INIT,
    LCD_MA_ERR_INVALID_ARENA,
    LCD_MA_ERR_INVALID_OBJECT,
    LCD_MA_ERR_NOT_ENOUGH_SPACE,
} LCD_MAError;

// PUBLIC FUNCTIONS

/// @brief Initializes the memory arena with the given memory and capacity.
/// @param memory A pointer to a valid memory location.
/// @param capacity The capacity of the memory in bytes.
/// @param out_arena A pointer to the memory arena to be initialized.
/// @return The memory arena reference, or NULL if memory is NULL or capacity is <= 0.
LCD_MemoryArena *LCD_MAUseMemory(void *const memory, u32 capacity);

/// @brief Releases the memory arena.
/// @param arena A pointer to the memory arena to be released.
/// @return LCD_MAError The error code.
LCD_MAError LCD_MAReleaseMemory(LCD_MemoryArena *arena);

/// @brief Allocates an object in the memory arena.
/// @param comps_size The size of the object in bytes.
/// @param out_obj A pointer to a pointer to the object to be allocated.
/// @note Why a pointer to a pointer? If I pass LCD_Obj *, the pointer itself will be
///       passed as a value. Hence, If all i need to do is change the value pointed by
///       the pointer, this works; but if I need to change the pointer itself, I need
///       to pass itself as a pointer!
LCD_MAError LCD_MAAllocObject(u16 comps_size, LCD_Obj **out_obj);

/// @brief Frees an object in the memory arena.
/// @param obj A pointer to the object to be freed.
/// @return LCD_MAError The error code.
LCD_MAError LCD_MAFreeObject(LCD_Obj *obj);

#endif