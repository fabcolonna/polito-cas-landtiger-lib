#ifndef __GLCD_H
#define __GLCD_H

#include "glcd_types.h"
#include "glcd_utils.h"

/// @brief Initializes TFT LCD Controller.
void LCD_Init(void);

/// @brief Clears the entire screen with the specified color.
/// @param color An RGB565 color
void LCD_ClearWith(LCD_RGBColor color);

/// @brief Returns the RGB565 color of the pixel at the specified coordinates.
/// @param point The coordinates of the pixel
/// @return The RGB565 color of the pixel
LCD_RGBColor LCD_GetPoint(LCD_Coordinate point);

/// @brief Sets the color of the pixel at the specified coordinates.
/// @param color The RGB565 color to set
/// @param where The coordinates of the pixel to color
void LCD_SetPoint(LCD_RGBColor color, LCD_Coordinate where);

/// @brief Draws a line using Brensenham's line algorithm.
/// @param from Starting coordinate
/// @param to Ending coordinate
/// @param color Line color
void LCD_DrawLine(LCD_Coordinate from, LCD_Coordinate to, u16 color);

/// @brief Draws a rectangle with the specified color.
/// @param from The top-left corner of the rectangle
/// @param to The bottom-right corner of the rectangle
/// @param edge_color The color of the edge of the rectangle
/// @param fill_color The color to fill the rectangle with
void LCD_DrawRectangle(LCD_Coordinate from, LCD_Coordinate to, u16 edge_color, u16 fill_color);

/// @brief Draws a circle with the specified color.
/// @param center The center of the circle
/// @param radius The radius of the circle
/// @param border_color The color of the border of the circle
/// @param fill_color The color to fill the circle with
void LCD_DrawCircle(LCD_Coordinate center, u16 radius, u16 border_color, u16 fill_color);

/// @brief Draws a sprite at the specified coordinates.
/// @param bitmap The sprite data
/// @param where The coordinates to draw the bitmap (top-left corner)
/// @note A bitmap is a 2D array of RGB565 colors, that defines an image,
///       or an animation, which is known as a sprite. A sprite can also
///       be described by further properties such as scale, position, rotation, etc.
void LCD_DrawSprite(const u16 *const bitmap, LCD_Coordinate where);

/// @brief Draws a char of the specified font at the specified coordinates using
///        the Midpoint Circle Drawing Algorithm.
/// @param chr The char to draw
/// @param chr_color The color of the char
/// @param font The font to use, from the LCD_Font enum
/// @param bg_color The background color
/// @param where The coordinates to draw the char
void LCD_PutChar(u8 chr, u16 chr_color, u8 font, u16 bg_color, LCD_Coordinate where);

/// @brief Prints an entire string starting from the specified coordinates.
/// @param str The string to print
/// @param str_color The color of the string
/// @param font The font to use, from the LCD_Font enum
/// @param bg_color The background color
/// @param where The coordinates to start printing the string
void LCD_PrintString(const char *const str, u16 str_color, u8 font, u16 bg_color, LCD_Coordinate where);

#endif