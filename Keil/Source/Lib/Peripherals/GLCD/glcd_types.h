#ifndef __GLCD_TYPES_H
#define __GLCD_TYPES_H

#include "glcd_errors.h"
#include "types.h"
#include <stdbool.h>

typedef enum
{
    LCD_ORIENT_VER = 0,
    LCD_ORIENT_HOR = 90,
    LCD_ORIENT_VER_INV = 180,
    LCD_ORIENT_HOR_INV = 270
} LCD_Orientation;

// TODO: Implement alignment!
typedef enum
{
    LCD_ALIGN_TOP_LEFT,
    LCD_ALIGN_TOP_CENTER,
    LCD_ALIGN_TOP_RIGHT,
    LCD_ALIGN_CENTER_LEFT,
    LCD_ALIGN_CENTER,
    LCD_ALIGN_CENTER_RIGHT,
    LCD_ALIGN_BOTTOM_LEFT,
    LCD_ALIGN_BOTTOM_CENTER,
    LCD_ALIGN_BOTTOM_RIGHT
} LCD_Alignment;

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

typedef struct
{
    u16 width, height;
} LCD_Dimension;

typedef struct
{
    u16 top, right, bottom, left;
} LCD_Padding;

typedef struct
{
    LCD_Coordinate top_left, bottom_right;
} LCD_BBox;

// FONT

typedef enum
{
    LCD_DEF_FONT_MSGOTHIC = 0,
    LCD_DEF_FONT_SYSTEM = 1
} LCD_DefaultFont;

typedef struct
{
    const u32 *data;
    u16 data_size, max_char_width, max_char_height;
    // Stores the width of each individual char, so we can move forward to the next char leaving just the right amount
    // of space between them. It's optional, e.g. system font & msgothic font don't have this info. In this case, the
    // spacing between chars is the same as the max_char_width.
    const u16 *char_widths, *char_heights;

    // Stores the offsets from the baseline for each char, so we can align letters like 'g' or 'h' correctly with
    // letters like 'a' or 'b'. It's optional, e.g. system font & msgothic font don't have this info. In this case, we
    // simply have this array = NULL. The renderer uses the baseline to determine how much we should move up from the
    // where->y coordinate so that the char is aligned to the baseline correctly. Moving up means subtracting the offset
    // from where-y.
    const u16 *baseline_offsets;
    u16 max_baseline_offset;
} LCD_Font;

typedef i8 LCD_FontID;

#define ASCII_FONT_MIN_VALUE 32
#define ASCII_FONT_MAX_VALUE 126

// COMPONENTS

typedef struct
{
    LCD_Coordinate from, to;
    LCD_Color color;
} LCD_Line;

typedef struct
{
    u16 width, height;
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
    const u32 *pixels; // RGB or ARGB data
    u16 width, height;
    bool has_alpha;
} LCD_Image;

typedef struct
{
    char *text;
    LCD_Color text_color, bg_color;
    LCD_FontID font;
    i16 char_spacing, line_spacing;
} LCD_Text;

// BUTTON STUFF

typedef struct
{
    char *text;
    LCD_Color text_color;
    LCD_FontID font;
    i16 char_spacing, line_spacing;
} LCD_ButtonLabel;

typedef struct
{
    LCD_ButtonLabel label;
    LCD_Color edge_color, fill_color;
    LCD_Padding padding;
} LCD_Button;

#define LCD_TEXT_DEF_CHAR_SPACING 0
#define LCD_TEXT_DEF_LINE_SPACING 0

#define LCD_NO_PADDING (LCD_Padding){0, 0, 0, 0}

/// @brief Represents a drawable component that can be rendered on the screen.
typedef enum
{
    LCD_COMP_LINE,
    LCD_COMP_RECT,
    LCD_COMP_CIRCLE,
    LCD_COMP_IMAGE,
    LCD_COMP_TEXT,
    LCD_COMP_BUTTON,
} LCD_ComponentType;

/// @brief Used to store a drawable component of any type.
typedef struct
{
    LCD_ComponentType type;
    LCD_Coordinate pos;
    union {
        LCD_Line line;
        LCD_Rect rect;
        LCD_Circle circle;
        LCD_Image image;
        LCD_Text text;
        LCD_Button button;
    } object;
} LCD_Component;

/// @brief Represents a generic object, made up of 1 or more basic
///        components, that are rendered on the screen.
/// @note This type is the one that the user should use to create
///       the objects that he wants to represent.
typedef struct
{
    LCD_Component *comps; // Array of components, or just one.
    u8 comps_size;
} LCD_Obj;

typedef i16 LCD_ObjID;

// LCD ADDING OPTIONS

typedef enum
{
    /// @brief Request GLCD to not mark the newly added object as visible. In that way, it
    ///        it won't be rendered by any LCD_RQRender() call, unless it's been marked as
    ///        visible through the LCD_RQSetObjectVisibility() function.
    LCD_ADD_OBJ_OPT_DONT_MARK_VISIBLE = 0x1
} LCD_RQAddOption;

#endif