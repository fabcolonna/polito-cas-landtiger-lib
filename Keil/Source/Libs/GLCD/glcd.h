#ifndef __GLCD_H
#define __GLCD_H

#include "glcd_macros.h"
#include "glcd_types.h"

/// @brief Converts RGB (24 bit) into RGB565 (16 bit):
///        - 5 bits for red (bits 11-15)
///        - 6 bits for green (bits 5-10)
///        - 5 bits for blue (bits 0-4)
#define RGB_TO_RGB565(r, g, b) (u16)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3))

#define BGR_TO_RGB565(bgr) (u16)(((bgr >> 11) & 0x1F) | ((bgr >> 5) & 0x3F) | (bgr & 0x1F))

/// @brief Initializes TFT LCD Controller.
/// @param orientation The orientation of the screen, from the LCD_Orientation enum
void LCD_Init(LCD_Orientation orientation);

/// @brief Checks if the LCD has been initialized.
/// @return Whether the LCD has been initialized
bool LCD_IsInitialized(void);

/// @brief Returns the RGB565 color of the pixel at the specified coordinates.
/// @param point The coordinates of the pixel
/// @return The RGB565 color of the pixel
LCD_Color LCD_GetPointColor(LCD_Coordinate point);

/// @brief Sets the color of the pixel at the specified coordinates.
/// @param color The RGB565 color to set
/// @param point The coordinates of the pixel to color
void LCD_SetPointColor(LCD_Color color, LCD_Coordinate point);

/// @brief Sets the background color of the screen.
/// @param color The RGB565 color to set
/// @note If this function is called after components have been rendered
///       on the screen, they will be deleted and the screen will be cleared.
void LCD_SetBackgroundColor(LCD_Color color);

/// @brief Adds a new object to the render queue, and (optionally) triggers an update.
/// @param obj The object to add
/// @return -1 if the object is NULL or the render queue is full, the ID of the object otherwise
LCD_ObjID LCD_RQAddObject(const LCD_Obj *const obj);

/// @brief Manually triggers an update of the screen. Useful when you want to
///        add multiple objects at once, and only update the screen at the end.
void LCD_RQRender(void);

/// @brief Removes an object from the render queue by its ID.
/// @param id The ID of the object to remove
/// @param redraw_screen Whether to redraw the other objects
void LCD_RQRemoveObject(LCD_ObjID id, bool redraw_screen);

/// @brief Shows/hides an object on/from the screen without modifying the render queue.
/// @param id The ID of the object to hide
/// @param visible Whether the object should be visible or not
/// @param redraw_lower_layers Whether to redraw the other objects (if removed)
void LCD_RQSetObjectVisibility(LCD_ObjID id, bool visible, bool redraw_screen);

/// @brief Removes all visible and non-visible objects from the screen.
void LCD_RQClear(void);

// FONT MANAGER

LCD_FontID LCD_FMAddFont(LCD_Font font);

void LCD_FMRemoveFont(LCD_FontID id);

#endif