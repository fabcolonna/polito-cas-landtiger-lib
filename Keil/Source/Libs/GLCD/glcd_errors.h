#ifndef __GLCD_ERRORS_H
#define __GLCD_ERRORS_H

/// @brief Error codes returned by the GLCD library.
typedef enum
{
    /// @brief No error occurred.
    LCD_ERR_OK = 0,

    /// @brief The memory arena is NULL or invalid.
    LCD_ERR_INVALID_ARENA,

    /// @brief The object is invalid (invalid ID, empty components array, etc.)
    LCD_ERR_INVALID_OBJ,

    /// @brief One or more params is NULL
    LCD_ERR_NULL_PARAMS,

    /// @brief The render queue is full. No more objects can be added.
    LCD_ERR_RQ_FULL,

    /// @brief The object has too many components.
    LCD_ERR_TOO_MANY_COMPS_IN_OBJ,

    /// @brief The specified (x,y) coordinates are out of the screen boundaries.
    LCD_ERR_COORDS_OUT_OF_BOUNDS,

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

    // PROCESS SHAPE ERRORS

    /// @brief An error occurred during BBox evaluation for a shape
    LCD_ERR_COULD_NOT_PROCESS_SHAPE_BBOX,

    /// @brief An error occurred during shape drawing
    LCD_ERR_COULD_NOT_PROCESS_SHAPE_DRAW,

    /// @brief An error occurred during shape deletion
    LCD_ERR_COULD_NOT_PROCESS_SHAPE_DELETE,

    /// @brief An error occured while trying to hide an object.
    LCD_ERR_COULD_NOT_HIDE_OBJ,
} LCD_Error;



#endif