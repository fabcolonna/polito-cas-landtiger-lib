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

// COMPONENT MANAGER

/// @brief Adds a new component to the render queue, and (optionally) triggers an update.
/// @param component The component to add
/// @param update Whether to trigger an update after adding the component
/// @return -1 if the component is NULL or the render queue is full, the ID of the component otherwise
LCD_ComponentID LCD_CMAddComp(LCD_Component component, bool update);

/// @brief Manually triggers an update of the screen. Useful when you want to
///        add multiple components at once, and only update the screen at the end.
void LCD_CMRender(void);

/// @brief Removes a component from the render queue by its ID.
/// @param id The ID of the component to remove
/// @param redraw_lower_layers Whether to redraw the components at lower layers
void LCD_CMRemoveComp(LCD_ComponentID id, bool redraw_lower_layers);

/// @brief Hides a component from the screen without removing it from the render queue.
/// @param id The ID of the component to hide
/// @param visible Whether the component should be visible
/// @param redraw_lower_layers Whether to redraw the components at lower layers
void LCD_CMSetCompVisibility(LCD_ComponentID id, bool visible, bool redraw_lower_layers);

/// @brief Removes all components from the screen.
void LCD_CMRemoveAllComps(void);

// FONT MANAGER

LCD_FontID LCD_FMAddFont(LCD_Font font);

void LCD_FMRemoveFont(LCD_FontID id);

#endif