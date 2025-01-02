#ifndef __GLCD_MACROS_H
#define __GLCD_MACROS_H

#include "glcd_types.h"

// ALIGNMENT & POSITION MACROS

#define LCD_COMP_ABSOLUTE_POS(pos)

// MAKE COMPONENTS

#define LCD_MAKE_LINE(...)                                                                                             \
    (LCD_Component)                                                                                                    \
    {                                                                                                                  \
        .type = LCD_COMP_LINE, .pos = (LCD_Coordinate){0, 0} /*unused*/, .object.line = (LCD_Line)__VA_ARGS__          \
    }

#define LCD_MAKE_RECT(x, y, ...)                                                                                       \
    (LCD_Component)                                                                                                    \
    {                                                                                                                  \
        .type = LCD_COMP_RECT, .pos = (LCD_Coordinate){x, y}, .object.rect = (LCD_Rect)__VA_ARGS__                     \
    }

#define LCD_MAKE_CIRCLE(...)                                                                                           \
    (LCD_Component)                                                                                                    \
    {                                                                                                                  \
        .type = LCD_COMP_CIRCLE, .pos = (LCD_Coordinate){0, 0} /*unused*/, .object.circle = (LCD_Circle)__VA_ARGS__    \
    }

#define LCD_MAKE_IMAGE(x, y, ...)                                                                                      \
    (LCD_Component)                                                                                                    \
    {                                                                                                                  \
        .type = LCD_COMP_IMAGE, .pos = (LCD_Coordinate){x, y}, .object.image = (LCD_Image)__VA_ARGS__                  \
    }

#define LCD_MAKE_TEXT(x, y, ...)                                                                                       \
    (LCD_Component)                                                                                                    \
    {                                                                                                                  \
        .type = LCD_COMP_TEXT, .pos = (LCD_Coordinate){x, y}, .object.text = (LCD_Text)__VA_ARGS__                     \
    }

#endif