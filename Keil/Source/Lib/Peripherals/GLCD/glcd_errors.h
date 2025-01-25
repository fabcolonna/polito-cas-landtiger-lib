#ifndef __GLCD_ERRORS_H
#define __GLCD_ERRORS_H

/// @brief Error codes returned by the GLCD library.
typedef enum
{
    /// @brief No error occurred.
    LCD_ERR_OK = 0,

    /// @brief The library is not initialized.
    LCD_ERR_UNINITIALIZED,

    /// @brief The object is invalid (invalid ID, empty components array, etc.)
    LCD_ERR_INVALID_OBJ,

    /// @brief One or more params is NULL
    LCD_ERR_NULL_PARAMS,

    /// @brief The object has too many components.
    LCD_ERR_TOO_MANY_COMPS_IN_OBJ,

    /// @brief The specified (x,y) coordinates are out of the screen boundaries.
    LCD_ERR_COORDS_OUT_OF_BOUNDS,

    /// @brief The font list is full. No more fonts can be added.
    LCD_ERR_FONT_LIST_FULL,

    /// @brief The font ID is invalid.
    LCD_ERR_INVALID_FONT_ID,

    /// @brief The memory pool does not have enough space to allocate the object.
    LCD_ERR_NO_MEMORY,

    // PROCESS SHAPE ERRORS

    /// @brief An error occurred during shape drawing
    LCD_ERR_DURING_RENDER,

    /// @brief An error occurred during shape deletion
    LCD_ERR_DURING_UNRENDER,

    /// @brief An error occurred during bounding box calculation
    LCD_ERR_DURING_BBOX_CALC,
} LCD_Error;

#endif