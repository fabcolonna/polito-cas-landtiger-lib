#ifndef __GLCD_H
#define __GLCD_H

#include "allocator.h"
#include "glcd_macros.h"
#include "glcd_types.h"

/// @brief Converts RGB (24 bit) into RGB565 (16 bit):
///        - 5 bits for red (bits 11-15)
///        - 6 bits for green (bits 5-10)
///        - 5 bits for blue (bits 0-4)
#define RGB8_TO_RGB565(rgb)                                                                                            \
    (u16)((((rgb >> 16) & 0xFF) >> 3) << 11 | (((rgb >> 8) & 0xFF) >> 2) << 5 | ((rgb & 0xFF) >> 3))

// PUBLIC FUNCTIONS

/// @brief Initializes TFT LCD Controller.
/// @param orientation The orientation of the screen, from the LCD_Orientation enum
/// @param alloc The memory allocator to use for the LCD objects
/// @param clear_to [OPTIONAL] The color to clear the screen with
/// @return LCD_Error The error code.
LCD_Error LCD_Init(LCD_Orientation orientation, MEM_Allocator *const alloc, const LCD_Color *const clear_to);

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
/// @param redraw_objects If true, the object that were previously visible
///        are re-rendered on top of the new background color.
/// @return LCD_Error The error code.
/// @note This function clears the screen, then re-renders all the objects
///       in the render list with the new background color.
LCD_Error LCD_SetBackgroundColor(LCD_Color color, bool redraw_objects);

/// @brief Adds a new object to the render list, and returns its ID (if out_id is not
///        NULL) through the out_id pointer.
/// @param obj The object to add
/// @param out_id The ID of the object, or NULL on error.
/// @param options Adding preferences, from the LCD_RMAddOption enum.
/// @return LCD_Error The error code.
/// @note Accepting the obj as a const * to simulate the r-value reference in C++, where
///       it's possible to pass a temporary (LCD_Obj&&) to the function, and then move
///       the object to the memory arena. This is not possible in C. If used with the two
///       macros LCD_OBJECT an LCD_RENDER_IMM, the object is passed as r-value reference!
LCD_Error LCD_RMAdd(LCD_Obj *const obj, LCD_ObjID *out_id, u8 options);

/// @brief Removes an object from the render list by its ID. It also deallocates the
///        memory used by that object in the memory arena.
/// @param id The ID of the object to remove
/// @param redraw_underneath Whether to redraw the objects that are below the removed object
/// @return LCD_Error The error code.
LCD_Error LCD_RMRemove(LCD_ObjID id, bool redraw_underneath);

/// @brief Removes all visible and non-visible objects from the screen.
/// @return LCD_Error The error code.
LCD_Error LCD_RMClear(void);

/// @brief Manually triggers an update of the screen. Useful when you want to
///        add multiple objects at once, and only update the screen at the end.
/// @return LCD_Error The error code.
LCD_Error LCD_RMRender(void);

/// @brief Renders the object immediately, without adding neither to the render list
///        nor to the memory arena. This is useful for objects that are not frequently
///        updated, and that you don't want to store in the memory arena.
/// @note To remove an object rendered you must call LCD_SetBackgroundColor(), effectively
///       clearing the screen and redrawing only the objects stored.
/// @param obj The object to render immediately
/// @return LCD_Error The error code.
/// @note Accepting the obj as a const * to simulate the r-value reference in C++, where
///       it's possible to pass a temporary (LCD_Obj&&) to the function, and then move
///       the object to the memory arena. This is not possible in C. If used with the two
///       macros LCD_OBJECT an LCD_RENDER_IMM, the object is passed as r-value reference!
LCD_Error LCD_RMRenderTemporary(LCD_Obj *const obj);

/// @brief Shows/hides an object on/from the screen without modifying the render list.
/// @param id The ID of the object to hide
/// @param visible Whether the object should be visible or not
/// @param redraw_underneath Whether to redraw the objects that are below the current one.
/// @return LCD_Error The error code.
LCD_Error LCD_RMSetVisibility(LCD_ObjID id, bool visible, bool redraw_underneath);

/// @brief Returns whether an object is visible on the screen or not.
/// @param id The ID of the object to check
/// @return Whether the object is visible or not
bool LCD_RMIsVisible(LCD_ObjID id);

/// @brief Moves an object in the render list to a new position. It also updates the
///        object's position in the RL itself, and in the memory arena.
/// @param id The ID of the object to move
/// @param new_pos The new position of the object, which will replace the old one
/// @param redraw_underneath Whether to redraw the objects that are below the object prior to the move
/// @return LCD_Error The error code.
/// @note As of now, this method moves the oject by translating each components' coordinate by an
///       offset calculated from the top-left corner of the bounding box of the object itself, returned
///       by the LCD_GetObjBBox() method. This may be imprecise!
LCD_Error LCD_RMMove(LCD_ObjID id, LCD_Coordinate new_pos, bool redraw_underneath);

// FONT MANAGER

/// @brief Adds a new font to the font manager, and returns its ID through the out_id pointer.
/// @param font The font to add
/// @param out_id The ID of the font, if it was added successfully to the font manager.
/// @return LCD_Error The error code.
LCD_Error LCD_FMAddFont(LCD_Font font, LCD_FontID *out_id);

/// @brief Removes a font from the font manager by its ID.
/// @param id The ID of the font to remove from the font manager.
/// @return LCD_Error The error code.
LCD_Error LCD_FMRemoveFont(LCD_FontID id);

// BBOX

/// @brief Returns the bounding box of an object in the render list.
/// @param id The ID of the object to get the bounding box of.
/// @param out_bbox The bounding box of the object, if it was found.
/// @return LCD_Error The error code.
LCD_Error LCD_GetBBox(LCD_ObjID id, LCD_BBox *out_bbox);

/// @brief Returns the bounding box of a temporary object.
/// @param obj The object to get the bounding box of.
/// @param out_bbox The bounding box of the object, if it was found.
/// @return LCD_Error The error code.
LCD_Error LCD_CalcBBoxForObject(const LCD_Obj *const obj, LCD_BBox *out_bbox);

// DEBUG FUNCTIONS

/// @brief Debug function to render the bounding box of a component.
/// @param id The ID of the object to render the bounding box of.
/// @return LCD_Error The error code.
LCD_Error LCD_DEBUG_RenderBBox(LCD_ObjID id);

#endif