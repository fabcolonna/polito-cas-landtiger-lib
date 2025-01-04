#ifndef __GLCD_MACROS_H
#define __GLCD_MACROS_H

#include "glcd_types.h"

#include <stdlib.h>
#include <string.h>

#define LCD_BEGIN_DRAWING {
#define LCD_END_DRAWING                                                                                                \
    LCD_RQRender();                                                                                                    \
    }

/**
 * @brief  Creates a static array of LCD_Component, and a corresponding LCD_Obj
 *         that references it.
 * @param  name   A unique identifier for this object (e.g. myLabel, mainMenu, etc.).
 * @param  size   The fixed number of components in the array.
 * @param  ...    The initializers for each LCD_Component in the array.
 */
#define LCD_OBJECT(name, num_comps, ...)                                                                               \
    ({                                                                                                                 \
        LCD_Component name##_comps[num_comps] = __VA_ARGS__;                                                           \
        static LCD_Component name##_comps_static[num_comps];                                                           \
        memcpy(name##_comps_static, name##_comps, num_comps * sizeof(LCD_Component));                                  \
        static LCD_Obj name##_obj_static = {.comps = name##_comps_static, .comps_size = (num_comps)};                  \
        LCD_RQAddObject(&name##_obj_static); /* Return the ID from this statement expression */                        \
    })

#define LCD_LINE(...)                                                                                                  \
    (LCD_Component)                                                                                                    \
    {                                                                                                                  \
        .type = LCD_COMP_LINE, .object.line = (LCD_Line)__VA_ARGS__                                                    \
    }

#define LCD_RECT(x, y, ...)                                                                                            \
    (LCD_Component)                                                                                                    \
    {                                                                                                                  \
        .type = LCD_COMP_RECT, .pos = (LCD_Coordinate){x, y}, .object.rect = (LCD_Rect)__VA_ARGS__                     \
    }

#define LCD_CIRCLE(...)                                                                                                \
    (LCD_Component)                                                                                                    \
    {                                                                                                                  \
        .type = LCD_COMP_CIRCLE, .object.circle = (LCD_Circle)__VA_ARGS__                                              \
    }

#define LCD_IMAGE(x, y, img)                                                                                           \
    (LCD_Component)                                                                                                    \
    {                                                                                                                  \
        .type = LCD_COMP_IMAGE, .pos = (LCD_Coordinate){x, y}, .object.image = img                                     \
    }

#define LCD_TEXT(x, y, ...)                                                                                            \
    (LCD_Component)                                                                                                    \
    {                                                                                                                  \
        .type = LCD_COMP_TEXT, .pos = (LCD_Coordinate){x, y}, .object.text = (LCD_Text)__VA_ARGS__                     \
    }

#endif