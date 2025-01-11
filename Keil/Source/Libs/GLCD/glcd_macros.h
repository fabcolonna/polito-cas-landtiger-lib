#ifndef __GLCD_MACROS_H
#define __GLCD_MACROS_H

#include "glcd_types.h"

#include <stdlib.h>
#include <string.h>

/**
 * @brief Constructs an identifiable object from a list of components, in a declarative fashion.
 * @param id [OUTPUT] A pointer to an LCD_ObjID variable, where the ID of the object will be stored.
 *          This ID can be used to remove the object from the queue, or to update it. You can specify
 *          NULL if you don't need the ID (in that case just use the LCD_RENDER_IMM!)
 * @param ... The components that make up the object.
 * @example
 *
 * LCD_ObjID id;
 * LCD_Error err = LCD_OBJECT(&id, {
 *      LCD_RECT(pos_rect, {
 *          .width = 10, .height = 10,
 *          .fill_color = LCD_COL_RED,
 *      }),
 *      LCD_TEXT(pos_text, {
 *          .text = "Hello, World!",
 *          .font = LCD_DEF_FONT_SYSTEM,
 *          .text_color = LCD_COL_BLUE,
 *      }),
 * });
 * @note Since the objects are identifiable, they need space in the memory arena specified during the
 *      initialization of the LCD module. If the memory arena is full, the function will return the error.
 * @return The LCD_Error that LCD_RQAddObject returns.
 */
#define LCD_OBJECT(id, ...)                                                                                            \
    LCD_RQAddObject((LCD_Obj){.comps = (LCD_Component[])__VA_ARGS__,                                                   \
                              .comps_size = sizeof((LCD_Component[])__VA_ARGS__) / sizeof(LCD_Component)},             \
                    id)

/**
 * @brief Renders a list of components immediately, without adding them to the queue.
 * @param ... The components to render.
 * @example
 * LCD_RENDER_IMM(
 *     LCD_RECT(pos_rect, {
 *        .width = 10, .height = 10,
 *        .fill_color = LCD_COL_RED,
 *    }),
 * });
 * @note This is useful when you want to render something immediately, without adding it to the queue.
 * @return The LCD_Error that LCD_RQRenderImmediate returns.
 */
#define LCD_RENDER_IMM(...)                                                                                            \
    LCD_RQRenderImmediate(&(LCD_Obj){.comps = (LCD_Component[])__VA_ARGS__,                                            \
                                     .comps_size = sizeof((LCD_Component[])__VA_ARGS__) / sizeof(LCD_Component)})

#define LCD_LINE(...)                                                                                                  \
    (LCD_Component)                                                                                                    \
    {                                                                                                                  \
        .type = LCD_COMP_LINE, .object.line = (LCD_Line)__VA_ARGS__                                                    \
    }

#define LCD_RECT(coords, ...)                                                                                          \
    (LCD_Component)                                                                                                    \
    {                                                                                                                  \
        .type = LCD_COMP_RECT, .pos = coords, .object.rect = (LCD_Rect)__VA_ARGS__                                     \
    }

#define LCD_RECT2(x, y, ...)                                                                                           \
    (LCD_Component)                                                                                                    \
    {                                                                                                                  \
        .type = LCD_COMP_RECT, .pos = (LCD_Coordinate){x, y}, .object.rect = (LCD_Rect)__VA_ARGS__                     \
    }

#define LCD_CIRCLE(...)                                                                                                \
    (LCD_Component)                                                                                                    \
    {                                                                                                                  \
        .type = LCD_COMP_CIRCLE, .object.circle = (LCD_Circle)__VA_ARGS__                                              \
    }

#define LCD_IMAGE(coords, img)                                                                                         \
    (LCD_Component)                                                                                                    \
    {                                                                                                                  \
        .type = LCD_COMP_IMAGE, .pos = coords, .object.image = img                                                     \
    }

#define LCD_IMAGE2(x, y, img)                                                                                          \
    (LCD_Component)                                                                                                    \
    {                                                                                                                  \
        .type = LCD_COMP_IMAGE, .pos = (LCD_Coordinate){x, y}, .object.image = img                                     \
    }

#define LCD_TEXT(coords, ...)                                                                                          \
    (LCD_Component)                                                                                                    \
    {                                                                                                                  \
        .type = LCD_COMP_TEXT, .pos = coords, .object.text = (LCD_Text)__VA_ARGS__                                     \
    }

#define LCD_TEXT2(x, y, ...)                                                                                           \
    (LCD_Component)                                                                                                    \
    {                                                                                                                  \
        .type = LCD_COMP_TEXT, .pos = (LCD_Coordinate){x, y}, .object.text = (LCD_Text)__VA_ARGS__                     \
    }

// BUTTON

#define LCD_BUTTON_LABEL(...) (LCD_ButtonLabel) __VA_ARGS__

/// @brief Creates a button component.
/// @param x The x-coordinate of the button.
/// @param y The y-coordinate of the button.
/// @param ... The initializers for the button component.
/// @returns A TP_ButtonArea component, mapped to the position & size of the button, so that
///          it can be used with the touch panel, to detect button presses.
/// @note Use the LCD_BUTTON_LABEL macro to initialize the label field, not the text field.
#define LCD_BUTTON2(x, y, out_button_area, ...)                                                                        \
    ((out_button_area = TP_AssignButtonArea((LCD_Button)__VA_ARGS__, (LCD_Coordinate){x, y})),                         \
     (LCD_Component){                                                                                                  \
         .type = LCD_COMP_BUTTON,                                                                                      \
         .pos = (LCD_Coordinate){x, y},                                                                                \
         .object.button = (LCD_Button)__VA_ARGS__,                                                                     \
     })

/// @brief Creates a button component.
/// @param coords The position of the button.
/// @param ... The initializers for the button component.
/// @returns A TP_ButtonArea component, mapped to the position & size of the button, so that
///          it can be used with the touch panel, to detect button presses.
/// @note Use the LCD_BUTTON_LABEL macro to initialize the label field, not the text field.
#define LCD_BUTTON(coords, out_button_area, ...)                                                                       \
    ((out_button_area = TP_AssignButtonArea((LCD_Button)__VA_ARGS__, coords)),                                         \
     (LCD_Component){                                                                                                  \
         .type = LCD_COMP_BUTTON,                                                                                      \
         .pos = coords,                                                                                                \
         .object.button = (LCD_Button)__VA_ARGS__,                                                                     \
     })

#endif