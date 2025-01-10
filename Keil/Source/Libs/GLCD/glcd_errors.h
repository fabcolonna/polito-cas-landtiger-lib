#ifndef __GLCD_ERRORS_H
#define __GLCD_ERRORS_H

/// @brief Error codes returned by the GLCD library.
typedef enum
{
    /// @brief No error occurred.
    LCD_ERR_OK = 0,

    /// @brief The memory arena is NULL or invalid.
    LCD_ERR_INVALID_ARENA,

    /// @brief The object is NULL or empty.
    LCD_ERR_EMPTY_OBJ,

    /// @brief The render queue is full. No more objects can be added.
    LCD_ERR_RQ_FULL,

    /// @brief The specified (x,y) coordinates are out of the screen boundaries.
    LCD_COORDS_OUT_OF_BOUNDS,

    /// @brief The object ID is invalid.
    LCD_ERR_INVALID_OBJ_ID,

    /// @brief The font list is full. No more fonts can be added.
    LCD_ERR_FONT_LIST_FULL,

    /// @brief The font ID is invalid.
    LCD_ERR_INVALID_FONT_ID,

    // MEMORY ARENA ERRORS

    /// @brief Attempt to free an invalid object.
    LCD_ERR_MA_ATTEMPT_TO_FREE_INVALID_OBJ,

    /// @brief The memory arena is invalid.
    LCD_ERR_MA_INVALID_ARENA,

    /// @brief The memory arena does not have enough space to allocate the object.
    LCD_ERR_MA_NOT_ENOUGH_SPACE,
} LCD_Error;

#endif