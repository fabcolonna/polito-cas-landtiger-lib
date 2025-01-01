#ifndef __GLCD_TYPES_H
#define __GLCD_TYPES_H

#include "utils.h"
#include <stdbool.h>

typedef enum
{
    LCD_ORIENT_VER = 0,
    LCD_ORIENT_HOR = 90,
    LCD_ORIENT_VER_INV = 180,
    LCD_ORIENT_HOR_INV = 270
} LCD_Orientation;

/// @brief Even though the LCD itself supports 260K colors, the bus interface
///        is only 16-bit, so we can only use 65K colors
typedef enum
{
    LCD_COL_WHITE = 0xFFFF,
    LCD_COL_BLACK = 0x0000,
    LCD_COL_GREY = 0xF7DE,
    LCD_COL_BLUE = 0x001F,
    LCD_COL_BLUE2 = 0x051F,
    LCD_COL_RED = 0xF800,
    LCD_COL_MAGENTA = 0xF81F,
    LCD_COL_GREEN = 0x07E0,
    LCD_COL_CYAN = 0x7FFF,
    LCD_COL_YELLOW = 0xFFE0,
    LCD_COL_NONE = 0x10000
} LCD_ColorPalette;

/// @brief Represents a color in the RGB565 format.
/// @note 32 bits are used to store the color, but only 17 are used:
///        5 bits for red, 6 bits for green, and 5 bits for blue, and
///        1 for the transparency bit.
typedef u32 LCD_Color;

typedef struct
{
    u16 x, y;
} LCD_Coordinate;

typedef enum
{
    LCD_FONT_MSGOTHIC,
    LCD_FONT_SYSTEM
} LCD_Font;

// COMPONENTS

typedef struct
{
    LCD_Coordinate from, to;
    LCD_Color color;
} LCD_Line;

typedef struct
{
    LCD_Coordinate from, to;
    LCD_Color edge_color, fill_color;
} LCD_Rect;

typedef struct
{
    LCD_Coordinate center;
    u16 radius;
    LCD_Color edge_color, fill_color;
} LCD_Circle;

typedef struct
{
    u32 *pixels;
    u16 width, height;
    bool has_alpha;
    LCD_Coordinate pos;
} LCD_Image;

typedef struct
{
    char *text;
    LCD_Color text_color, bg_color;
    u8 font;
    LCD_Coordinate pos;
} LCD_Text;

/// @brief Represents a drawable component that can be rendered on the screen.
typedef enum
{
    LCD_COMP_LINE,
    LCD_COMP_RECT,
    LCD_COMP_CIRCLE,
    LCD_COMP_IMAGE_RLE,
    LCD_COMP_IMAGE,
    LCD_COMP_TEXT
} LCD_ComponentType;

/// @brief Used to store a drawable component of any type.
typedef struct
{
    LCD_ComponentType type;
    union {
        LCD_Line line;
        LCD_Rect rect;
        LCD_Circle circle;
        LCD_Image image;
        LCD_Text text;
    } object;
} LCD_Component;

#endif