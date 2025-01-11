#ifndef __GLCD_H
#define __GLCD_H

#include "glcd_macros.h"
#include "glcd_mem.h"
#include "glcd_types.h"

/// @brief Converts RGB (24 bit) into RGB565 (16 bit):
///        - 5 bits for red (bits 11-15)
///        - 6 bits for green (bits 5-10)
///        - 5 bits for blue (bits 0-4)
_FORCE_INLINE u16 rgb888_to_rgb565(u32 rgb888)
{
    const u8 r = (rgb888 >> 16) & 0xFF;
    const u8 g = (rgb888 >> 8) & 0xFF;
    const u8 b = rgb888 & 0xFF;
    return (u16)((r >> 3) << 11 | (g >> 2) << 5 | (b >> 3));
}

/// @brief Initializes TFT LCD Controller.
/// @param orientation The orientation of the screen, from the LCD_Orientation enum
/// @param arena A correctly initialized memory arena, which will be used to store
///        the objects that will be rendered on the screen.
/// @return LCD_Error The error code.
LCD_Error LCD_Init(LCD_Orientation orientation, LCD_MemoryArena *arena);

/// @brief Changes the memory arena used by the LCD.
/// @param arena The new memory arena to use
/// @return LCD_Error The error code.
LCD_Error LCD_UseArena(LCD_MemoryArena *arena);

/// @brief Checks if the LCD has been initialized.
/// @return Whether the LCD has been initialized
bool LCD_IsInitialized(void);

/// @brief Returns the width of the screen at the current orientation.
u16 LCD_GetWidth(void);

/// @brief Returns the height of the screen at the current orientation.
u16 LCD_GetHeight(void);

/// @brief Returns the size of the screen at the current orientation.
LCD_Coordinate LCD_GetSize(void);

/// @brief Returns the center of the screen at the current orientation.
LCD_Coordinate LCD_GetCenter(void);

// TODO: Maybe in the future!
/// @brief Returns the coordinates at which the object should be drawn to be centered.
/// @param obj The object to center
/// @return The coordinates at which the object should be drawn
// LCD_Coordinate LCD_GetCenterForObject(const LCD_Obj *const obj);

/// @brief Returns the bounding box of the component, i.e. the smallest
///        rectangle that contains the component itself, with both the
///        2 coordinates (top-left & bottom-right), and the width and
///        height already calculated.
/// @param comp The component to get the bounding box of
/// @return The bounding box of the component, expressed with 2 coordinates
///         and the width and height.
LCD_BBox LCD_GetComponentBBox(LCD_Component comp);

/// @brief Returns the bounding box of an object that has been previously added
///        to the render queue, and thus it has an ID.
/// @param id The ID of the object to get the bounding box of
/// @return The bounding box of the object, expressed with 2 coordinates
///         and the width and height.
LCD_BBox LCD_GetObjBBoxWithID(LCD_ObjID id);

/// @brief Returns the bounding box of the object passed as parameter, regardless
///        of whether it has been added to the render queue or not.
/// @param obj The object to get the bounding box of
/// @return The bounding box of the object, expressed with 2 coordinates
///         and the width and height.
LCD_BBox LCD_GetObjBBox(const LCD_Obj *const obj);

/// @brief Returns the RGB565 color of the pixel at the specified coordinates.
/// @param point The coordinates of the pixel
/// @return The RGB565 color of the pixel
LCD_Color LCD_GetPointColor(LCD_Coordinate point);

/// @brief Sets the color of the pixel at the specified coordinates.
/// @param color The RGB565 color to set
/// @param point The coordinates of the pixel to color
/// @return LCD_Error The error code.
LCD_Error LCD_SetPointColor(LCD_Color color, LCD_Coordinate point);

/// @brief Sets the background color of the screen.
/// @param color The RGB565 color to set
/// @note This function clears the screen, then re-renders all the objects
///       in the render queue with the new background color.
void LCD_SetBackgroundColor(LCD_Color color);

/// @brief Adds a new object to the memory arena and to the render queue, and returns
///        its ID (if out_id is not NULL) through the out_id pointer.
/// @param obj The object to add
/// @param out_id The ID of the object, or NULL on error.
/// @return LCD_Error The error code.
LCD_Error LCD_RQAddObject(LCD_Obj obj, LCD_ObjID *out_id);

/// @brief Manually triggers an update of the screen. Useful when you want to
///        add multiple objects at once, and only update the screen at the end.
void LCD_RQRender(void);

/// @brief Renders the object immediately, without adding neither to the render queue
///        nor to the memory arena. This is useful for objects that are not frequently
///        updated, and that you don't want to store in the memory arena.
/// @note To remove an object rendered you must call LCD_SetBackgroundColor(), effectively
///       clearing the screen and redrawing only the objects stored.
/// @param obj The object to render immediately
LCD_Error LCD_RQRenderImmediate(const LCD_Obj *const obj);

/// @brief Removes an object from the render queue by its ID. It also deallocates the
///        memory used by that object in the memory arena.
/// @param id The ID of the object to remove
/// @param redraw_screen Whether to redraw the other objects
LCD_Error LCD_RQRemoveObject(LCD_ObjID id, bool redraw_screen);

/// @brief Shows/hides an object on/from the screen without modifying the render queue.
/// @param id The ID of the object to hide
/// @param visible Whether the object should be visible or not
/// @param redraw_lower_layers Whether to redraw the other objects (if removed)
LCD_Error LCD_RQSetObjectVisibility(LCD_ObjID id, bool visible, bool redraw_screen);

/// @brief Removes all visible and non-visible objects from the screen.
void LCD_RQClear(void);

// FONT MANAGER

/// @brief Adds a new font to the font manager, and returns its ID through the out_id pointer.
/// @param font The font to add
/// @return -1 if the font is NULL or the font manager is full, the ID of the font otherwise
LCD_Error LCD_FMAddFont(LCD_Font font, LCD_FontID *out_id);

/// @brief Removes a font from the font manager by its ID.
LCD_Error LCD_FMRemoveFont(LCD_FontID id);

// DEBUG FUNCTIONS

/// @brief Debug function to render the bounding box of a component.
/// @param bbox The bounding box to render
void LCD_DEBUG_RenderBBox(LCD_BBox bbox);

#endif