#ifndef GLCD_MEM_ARENA_H
#define GLCD_MEM_ARENA_H

#include "glcd_types.h"
#include "utils.h"

// DEFINE FUNCTIONS

/// @brief Initializes  a static memory arena with given name and capacity.
/// @param name The name of the memory arena.
/// @param capacity The capacity of the memory arena in bytes.
/// @note The memory area needs to be aligned to 4 bytes, since unaligned memory access is not allowed on ARM.
#define LCD_MA_ALLOC_STATIC_MEM(name, capacity) static u8 __attribute__((aligned(4))) name[capacity];

// TYPES

// Forward declaration of LCD_MemoryArena type
typedef struct _lcd_mem_arena LCD_MemoryArena;

// PUBLIC FUNCTIONS

/// @brief Initializes the memory arena with the given memory and capacity.
/// @param memory A pointer to a valid memory location.
/// @param capacity The capacity of the memory in bytes.
/// @param out_arena A pointer to the memory arena to be initialized.
/// @return The memory arena reference, or NULL if memory is NULL or capacity is <= 0.
LCD_MemoryArena *LCD_MAUseMemory(void *const memory, u32 capacity);

/// @brief Releases the memory arena.
/// @param arena A pointer to the memory arena to be released.
/// @return LCD_Error The error code.
LCD_Error LCD_MAReleaseMemory(LCD_MemoryArena *arena);

// AUTOMATICALLY CALLED FUNCTIONS (MAYBE WILL MAKE THEM PRIVATE)

/// @brief Allocates an object in the memory arena.
/// @param comps_size The size of the object in bytes.
/// @param out_obj A pointer to a pointer to the object to be allocated.
/// @note Why a pointer to a pointer? If I pass LCD_Obj *, the pointer itself will be
///       passed as a value. Hence, If all i need to do is change the value pointed by
///       the pointer, this works; but if I need to change the pointer itself, I need
///       to pass itself as a pointer!
/// @return LCD_Error The error code.
/// @note This function is called automatically by the GLCD library, no need to call it manually.
LCD_Error LCD_MAAllocObject(u16 comps_size, LCD_Obj **out_obj);

/// @brief Frees an object in the memory arena.
/// @param obj A pointer to the object to be freed.
/// @return LCD_Error The error code.
/// @note This function is called automatically by the GLCD library, no need to call it manually.
LCD_Error LCD_MAFreeObject(LCD_Obj *obj);

#endif