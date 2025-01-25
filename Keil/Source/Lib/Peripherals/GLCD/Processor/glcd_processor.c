#include "glcd_processor.h"
#include "glcd.h"

#include <math.h>

// VARIABLES

_DECL_EXTERNALLY u16 LCDMaxX, LCDMaxY;
_DECL_EXTERNALLY LCD_Color LCDCurrentBGColor;

_DECL_EXTERNALLY LCD_Font LCDFontList[];
_DECL_EXTERNALLY u8 LCDFontListSize;

// MACROS

#define IS_MODE(mode, flag) (((mode) & (flag)) != 0)

#define SWAP(type, src, dst)                                                                                           \
    {                                                                                                                  \
        type tmp = src;                                                                                                \
        src = dst;                                                                                                     \
        dst = tmp;                                                                                                     \
    }

// DRAWING FUNCTIONS

#define SET_POINT(mode, color, x, y)                                                                                   \
    {                                                                                                                  \
        if (IS_MODE(mode, MD_DRAW_BBOX))                                                                               \
            LCD_SetPointColor(color, (LCD_Coordinate){x, y});                                                          \
        else if (IS_MODE(mode, MD_DELETE))                                                                             \
            LCD_SetPointColor(LCDCurrentBGColor, (LCD_Coordinate){x, y});                                              \
        else                                                                                                           \
        {                                                                                                              \
            (void)(color);                                                                                             \
            (void)(x);                                                                                                 \
            (void)(y);                                                                                                 \
        }                                                                                                              \
    }

// PRIVATE FUNCTIONS

_PRIVATE bool process_line(const LCD_Line *const line, LCD_BBox *out_bbox, u8 mode)
{
    if (!line || !mode)
        return false;

    // If the starting point is to the right of the ending point (both for x and/or y), swap.
    LCD_Coordinate from = line->from, to = line->to;
    LCD_Coordinate original_from = from, original_to = to;

    int dx = abs(to.x - from.x); // Absolute difference
    int dy = abs(to.y - from.y);

    // Determine the step directions
    int x_step = (from.x < to.x) ? 1 : -1;
    int y_step = (from.y < to.y) ? 1 : -1;

    // Special cases: Vertical or Horizontal lines
    if (dx == 0) // Vertical line
    {
        while (from.y != to.y + y_step)
        {
            SET_POINT(mode, line->color, from.x, from.y);
            from.y += y_step;
        }
    }
    else if (dy == 0) // Horizontal line
    {
        while (from.x != to.x + x_step)
        {
            SET_POINT(mode, line->color, from.x, from.y);
            from.x += x_step;
        }
    }
    else if (dx > dy) // Bresenham's algorithm: slope < 1
    {
        int tmp = 2 * dy - dx;
        while (from.x != to.x + x_step)
        {
            SET_POINT(mode, line->color, from.x, from.y);
            from.x += x_step;
            if (tmp > 0)
            {
                from.y += y_step;
                tmp += 2 * dy - 2 * dx;
            }
            else
                tmp += 2 * dy;
        }
    }
    else // Bresenham's algorithm: slope >= 1
    {
        int tmp = 2 * dx - dy;
        while (from.y != to.y + y_step)
        {
            SET_POINT(mode, line->color, from.x, from.y);
            from.y += y_step;
            if (tmp > 0)
            {
                from.x += x_step;
                tmp += 2 * dx - 2 * dy;
            }
            else
                tmp += 2 * dx;
        }
    }

    if (out_bbox && IS_MODE(mode, MD_BBOX | MD_DRAW_BBOX))
    {
        // Swapping original coordinates if needed
        if (original_from.x > original_to.x)
            SWAP(u16, original_from.x, original_to.x)

        if (original_from.y > original_to.y)
            SWAP(u16, original_from.y, original_to.y)

        out_bbox->top_left = original_from;
        out_bbox->bottom_right = original_to;
    }

    return true;
}

_PRIVATE bool process_rect(const LCD_Rect *const rect, LCD_Coordinate pos, LCD_BBox *out_bbox, u8 mode)
{
    if (!rect || !mode)
        return false;

    // Rect object now contains width and height, not the start/end coordinates.
    // This, to support the rendering of the rectangle at various positions on the
    // screen, based on the pos parameter. pos is the top left corner of the rectangle.
    if (rect->width == 0 || rect->height == 0)
        return false;

    LCD_Coordinate from = pos;
    LCD_Coordinate to = {from.x + rect->width, from.y + rect->height};
    if (from.x >= LCDMaxX || from.y >= LCDMaxY)
        return false; // Rectangle is completely outside the screen

    // If the ending point is outside the screen, adjust it.
    if (to.x >= LCDMaxX)
        to.x = LCDMaxX - 1;
    if (to.y >= LCDMaxY)
        to.y = LCDMaxY - 1;

    // If the starting point is to the right of the ending point (both for x and/or y), swap.
    if (from.x > to.x)
        SWAP(u16, to.x, from.x)
    if (from.y > to.y)
        SWAP(u16, to.y, from.y)

    if (IS_MODE(mode, MD_DRAW_BBOX) || IS_MODE(mode, MD_DELETE))
    {
        // Drawing the edges of the rectangle
        LCD_Line line = {.color = rect->edge_color};

        line.from = from;
        line.to = (LCD_Coordinate){to.x, from.y};
        process_line(&line, NULL, mode); // Top horizontal edge
        line.to = (LCD_Coordinate){from.x, to.y};
        process_line(&line, NULL, mode); // Left vertical edge

        line.from = to;
        line.to = (LCD_Coordinate){to.x, from.y};
        process_line(&line, NULL, mode); // Right vertical edge
        line.to = (LCD_Coordinate){from.x, to.y};
        process_line(&line, NULL, mode); // Bottom horizontal edge

        if (rect->fill_color != LCD_COL_NONE)
        {
            // Filling the rectangle
            for (u16 i = from.x + 1; i < to.x; i++)
                for (u16 j = from.y + 1; j < to.y; j++)
                    SET_POINT(mode, rect->fill_color, i, j);
        }
    }

    if (out_bbox && IS_MODE(mode, MD_BBOX | MD_DRAW_BBOX))
    {
        out_bbox->top_left = from;
        out_bbox->bottom_right = to;
    }

    return true;
}

_PRIVATE bool process_circle(const LCD_Circle *const circle, LCD_BBox *out_bbox, u8 mode)
{
    if (!circle || !mode || circle->radius == 0)
        return false;

    const LCD_Coordinate center = circle->center;
    const u16 radius = circle->radius;

    if (IS_MODE(mode, MD_DRAW_BBOX) || IS_MODE(mode, MD_DELETE))
    {
        // Printing 4 cardinal points of the circle: note that if the radius is subtracted from y,
        // from the center, the pixel will be actually colored at the other side, because smaller
        // numbers mean higher positions in the screen.
        SET_POINT(mode, circle->edge_color, center.x + radius, center.y); // Right side of the center
        SET_POINT(mode, circle->edge_color, center.x - radius, center.y); // Left side
        SET_POINT(mode, circle->edge_color, center.x, center.y + radius); // Top, but we +, it's the bottom
        SET_POINT(mode, circle->edge_color, center.x, center.y - radius); // Bottom, but we -, it's the top

        u16 x = radius, y = 0; // Working on the first octant in the fourth quadrant
        int p = 1 - radius;    // Initial value of the decision parameter

        // Iterating till y (that is initially zero) becomes = x, meaning that
        // we reached the end of the octant. That's because we're actually iterating
        // over the points of that octant only. The other ones are printed using symmetry.
        while (x > y++)
        {
            // The decision parameter is needed to figure out if the X position at which
            // we're drawing needs to be decreased by 1, i.e. moved closer to the circle,
            // so that a curved line is formed. Y is constanty incremented in the while
            // loop, meaning that we're moving upwords in the octant, but really downward
            // on the screen, since we get higher pixel values if we move to the bottom of it.
            if (p <= 0)           // The midpoint is inside or on the border of the circle, x stays fixed.
                p += (2 * y + 1); // 2y + 1 accounts for the distance increase due to Y++
            else                  // Outside the circle
            {
                x--;                    // We move closer to the center
                p += (2 * (y - x) + 1); // 2(y-x) + 1 accounts for the decreased X and increased Y
            }

            if (y >= x) // We reached the end of the octant (y=x)
                break;

            // Printing the points in the other octants using symmetry
            SET_POINT(mode, circle->edge_color, x + center.x, y + center.y);
            SET_POINT(mode, circle->edge_color, -x + center.x, y + center.y);
            SET_POINT(mode, circle->edge_color, x + center.x, -y + center.y);
            SET_POINT(mode, circle->edge_color, -x + center.x, -y + center.y);
            SET_POINT(mode, circle->edge_color, y + center.x, x + center.y);
            SET_POINT(mode, circle->edge_color, -y + center.x, x + center.y);
            SET_POINT(mode, circle->edge_color, y + center.x, -x + center.y);
            SET_POINT(mode, circle->edge_color, -y + center.x, -x + center.y);
        }

        if (circle->fill_color != LCD_COL_NONE)
        {
            i16 x_diff, y_diff, rad_sq;
            for (u16 i = center.x - radius + 1; i < center.x + radius; i++)
            {
                for (u16 j = center.y - radius + 1; j < center.y + radius; j++)
                {
                    x_diff = (i - center.x) * (i - center.x);
                    y_diff = (j - center.y) * (j - center.y);
                    rad_sq = radius * radius;
                    if (x_diff + y_diff <= rad_sq)
                        SET_POINT(mode, circle->fill_color, i, j);
                }
            }
        }
    }

    if (out_bbox && IS_MODE(mode, MD_BBOX | MD_DRAW_BBOX))
    {
        out_bbox->top_left = (LCD_Coordinate){center.x - radius, center.y - radius};
        out_bbox->bottom_right = (LCD_Coordinate){center.x + radius, center.y + radius};
    }

    return true;
}

_PRIVATE bool process_img_rle(const LCD_Image *const img, LCD_Coordinate pos, LCD_BBox *out_bbox, u8 mode)
{
    if (!img || !img->pixels || !mode)
        return false;

    u16 width = img->width, height = img->height;
    if (width == 0 || height == 0)
        return false;

    // Pos is the top-left corner of the image.
    if (pos.x >= LCDMaxX || pos.y >= LCDMaxY)
        return false; // Image is completely outside the screen

    // Not drawing images partially out of the screen.
    if (pos.x + width > LCDMaxX || pos.y + height > LCDMaxY)
        return false;

    if (IS_MODE(mode, MD_DRAW_BBOX) || IS_MODE(mode, MD_DELETE))
    {
        const u32 *pixels_copy = img->pixels;
        u32 pixel_data,
            rgb888; // Pixel is a 32-bit or 24-bit value: 0xAARRGGBB if alpha is present, 0x00RRGGBB otherwise

        u8 alpha_data;
        u16 current_x, pixels_left, count;
        for (u16 i = 0; i < height; i++)
        {
            current_x = pos.x;   // Start at the beginning of the row
            pixels_left = width; // Remaining pixels to draw in the row

            while (pixels_left > 0)
            {
                count = *pixels_copy++;
                pixel_data = *pixels_copy++;

                // Draw pixels, ensuring we stay within the row
                for (u16 j = 0; j < count && pixels_left > 0; j++)
                {
                    if (img->has_alpha)
                    {
                        // If the alpha is present, then pixel_data is of type 0xAARRGGBB.
                        alpha_data = (u8)(pixel_data >> 24);
                        rgb888 = (u32)(pixel_data & 0x00FFFFFF);

                        if (alpha_data) // Alpha is != 0
                            SET_POINT(mode, RGB8_TO_RGB565(rgb888), current_x, pos.y + i);

                        current_x++; // Move to the next pixel, regardless of the alpha value
                    }
                    else
                    {
                        // No alpha, pixel_data is of type 0x00RRGGBB
                        rgb888 = (u32)(pixel_data & 0x00FFFFFF);
                        SET_POINT(mode, RGB8_TO_RGB565(rgb888), current_x++, pos.y + i);
                    }

                    pixels_left--;
                }
            }
        }
    }

    if (out_bbox && IS_MODE(mode, MD_BBOX | MD_DRAW_BBOX))
    {
        out_bbox->top_left = pos;
        out_bbox->bottom_right = (LCD_Coordinate){pos.x + width, pos.y + height};
    }

    return true;
}

// TEXT FUNCTIONS

typedef enum
{
    PRINT_CHR_OK = 0,
    PRINT_CHR_NOTHINGNESS,
    PRINT_CHR_ERR_NULL_PARAMS,
    PRINT_CHR_ERR_SHOULD_NEWLINE,
    PRINT_CHR_ERR_OUT_OF_VERTICAL_BOUNDS,
} PrintCharError;

_PRIVATE PrintCharError print_char(u8 chr, const LCD_Font *const font, LCD_Coordinate where, u8 mode, LCD_Color txt_col,
                                   LCD_Color bg_col, u16 *out_char_w, u16 *out_char_h)
{
    // We can't print non-ASCII printable chars, and other invalid values
    if (!font || chr < ASCII_FONT_MIN_VALUE || chr > ASCII_FONT_MAX_VALUE)
        return PRINT_CHR_ERR_NULL_PARAMS;

    // Each line of the array contains the char data, and chars are stored in ASCII order. Each line is
    // made up of font->char_height different u32 values, each one representing a row of the given char.
    // Hence, for every char, I need to retrieve font->char_height u32 values from the array line given
    // by chr - ASCII_FONT_MIN_VALUE, since the first printable ASCII char is 32.
    // For example: A (=65) - 32 = 33, so starting from the 33rd line of the array, I need to read
    // font->char_height u32 values.
    const u16 index = chr - ASCII_FONT_MIN_VALUE;

    // Considering the baseline offset of the current char to align it correctly (in case of letters with
    // descenders like 'g' or 'y' compared to letters like 'a' or 'b'). If the font data has been created
    // with the ttf2c script, chances are that the baseline offsets are stored in the baseline_offsets array.
    const u16 baseline_offset = font->baseline_offsets ? font->baseline_offsets[index] : 0;

    // We're moving the char down by baseline_offset pixels, so that when it's printed
    // by moving up, it's gonna have its top-left corner at the expected position.
    where.y += font->max_baseline_offset;

    // Getting the char height/width from the char_heights/widths array, which stores them in ASCII order.
    // If any of these arrays is NULL (e.g. for the preloaded fonts), then max_char_height/width is used.

    // We need to determine the width of the selected char, so we can move forward to the next char
    // leaving just the right amount of space between them. If the font data has been created with
    // the ttf2c script, chances are that the width of the char is stored in the char_widths array.
    // This also applies to the char_heights array.
    *out_char_h = font->char_heights ? font->char_heights[index] : font->max_char_height;
    *out_char_w = font->char_widths ? font->char_widths[index] : font->max_char_width;

    // If the char has no width, it's really nothingness, so we return immediately.
    if (*out_char_w == 0)
        return PRINT_CHR_NOTHINGNESS;

    // Verifying this before the x check, because the char might be completely outside the screen
    if (where.y - baseline_offset < 0 || where.y - baseline_offset >= LCDMaxY)
        return PRINT_CHR_ERR_OUT_OF_VERTICAL_BOUNDS;

    // If the char is completely outside the screen, return immediately, but with the char details set.
    if (where.x < 0 || where.x + *out_char_w >= LCDMaxX)
        return PRINT_CHR_ERR_SHOULD_NEWLINE;

    // If we don't want to actually print/delete the char, we just return the width of the char.
    // Additionally, if we want to print/delete but the char has no height data, just return,
    // so that print_text can move to the next char and leave some space between them.
    if (((mode & (MD_DRAW_BBOX | MD_DELETE)) == 0) || *out_char_h == 0)
        return PRINT_CHR_OK;

    // We need to move to the correct row of the array, which contains the char data.
    // Problem is, the length of the rows before the one we need is variable, hence we can't
    // simply move the pointer by a fixed quantity index * char_h, because char_h is the length
    // of the specific row we need!
    const u32 *font_data_copy = font->data;
    for (u16 i = 0; i < index; i++)
        font_data_copy += font->char_heights ? font->char_heights[i] : font->max_char_height;

    u32 value;
    bool pixel_has_value;
    for (u16 i = 0; i < *out_char_h; i++) // For each char row, we have font->char_width bits
    {
        for (u16 j = 0; j < *out_char_w; j++)
        {
            // Since a char value is not necessarily 32 bits long, but its length depends on
            // the char width, we mask the MSB that are not part of the char data.
            // E.g. 8 bits, we & with 0xFFFFFFFF >> (32 - 8) = 0xFFFFFFFF >> 24 = 0x000000FF
            value = font_data_copy[i] & (0xFFFFFFFF >> (32 - *out_char_w));

            // E.g 8 bit font data: mask 0xFF and data 0xC3 = 0b11000011
            // Starting from the leftmost bit -> 0b11000011 >> 7=(char_w=8 - j=0 - 1) = 0b00000011 & 0x1 = 1
            pixel_has_value = (value >> (*out_char_w - j - 1)) & 0x1;
            SET_POINT(mode, pixel_has_value ? txt_col : bg_col, where.x + j, where.y + i - baseline_offset);
        }
    }

    return PRINT_CHR_OK;
}

/// @brief Prints the string onto the screen, or calculates its dimensions without the overhead of printing.
/// @param text The text to print
/// @param pos The top-left corner (considering the eventual baseline offset) of the text
/// @param dont_actually_print If true, the text is not printed, but the width is calculated. Used
///                            to determine the width of the text without actually printing it.
/// @param out_dim [OUTPUT] The width and height of the text, regardless of the actual printing.
_PRIVATE bool process_text(const LCD_Text *const text, LCD_Coordinate pos, LCD_BBox *out_bbox, u8 mode)
{
    if (!text || !text->text || !mode || text->font >= LCDFontListSize)
        return false;

    const LCD_Font font = LCDFontList[text->font];
    if (!font.data)
        return false;

    if (pos.x >= LCDMaxX || pos.y >= LCDMaxY)
        return false; // Text is completely outside the screen

    u8 chr, *str = (u8 *)(text->text);
    u16 start_x = pos.x, start_y = pos.y; // Saving the starting (x, y) coords

    // Track the max width (for multi-line) and the total height of the text (min. is one line, i.e. font height)
    u16 cur_width = 0, max_width = 0, total_height = font.max_char_height;

    PrintCharError err;
    i16 x_diff;
    bool no_more_space = false;
    u16 char_w, char_h, x_inc, y_inc;
    while ((chr = *str) && !no_more_space)
    {
        err = print_char(chr, &font, pos, mode, text->text_color, text->bg_color, &char_w, &char_h);
        if (err == PRINT_CHR_ERR_NULL_PARAMS)
            return false; // Invalid, return.

        // String is incremented only if the char has been printed correctly, otherwise we
        // may need to re-print it (e.g. if err = PRINT_CHR_ERR_SHOULD_NEWLINE)
        if (err == PRINT_CHR_OK)
            str++;

        x_inc = char_w + text->char_spacing;
        y_inc = font.max_char_height + text->line_spacing;

        if ((pos.x + x_inc) < LCDMaxX)
        {
            // While there's space on the x axis, move on this axis by x_inc
            pos.x += x_inc;
        }
        else if ((pos.y + y_inc) < LCDMaxY)
        {
            // If there's no space on the x axis, but there's space on the y axis, move to the next line
            pos.x = start_x; // =0 OR start_x, to keep the same x position for all lines or start at the very left.
            pos.y += y_inc;  // Move to the next line
            total_height += y_inc; // Increment the total height of the text
        }
        else
        {
            // assert(err == PRINT_CHR_ERR_OUT_OF_VERTICAL_BOUNDS);
            no_more_space = true; // Stop printing if there's no more space
        }

        x_diff = pos.x - start_x;
        if ((pos.x + x_inc) < LCDMaxX)
            cur_width = (x_diff < LCDMaxX) ? x_diff : LCDMaxX;
        else
            cur_width = (x_diff + char_w) < LCDMaxX ? (x_diff + char_w) : LCDMaxX;

        // assert(cur_width >= 0); // Should never be < 0, if @ line 800 we specify to start at start_x, not 0
        max_width = (cur_width > max_width) ? cur_width : max_width;
    }

    // If the string is short and doesn't wrap, set max_width correctly
    if (max_width == 0)
        max_width = pos.x - start_x;

    if (out_bbox && IS_MODE(mode, MD_BBOX | MD_DRAW_BBOX))
    {
        out_bbox->top_left = (LCD_Coordinate){start_x, start_y};
        out_bbox->bottom_right = (LCD_Coordinate){start_x + max_width, start_y + total_height};
    }

    return true;
}

// BUTTON FUNCTIONS

_PRIVATE bool process_button(const LCD_Button *const button, LCD_Coordinate pos, LCD_BBox *out_bbox, u8 mode)
{
    if (!button || !mode)
        return false;

    const LCD_Text label_as_text = {
        .text = button->label.text,
        .font = button->label.font,
        .text_color = button->label.text_color,
        .bg_color = LCD_COL_NONE,
        .char_spacing = button->label.char_spacing,
        .line_spacing = button->label.line_spacing,
    };

    // There are two cases: the button has no border, and the button has a border.
    // If the button has no border, we only need to consider the label:
    if (button->edge_color == LCD_COL_NONE && button->fill_color == LCD_COL_NONE)
    {
        // With no border, we only need to consider the label. Again, two more cases:
        // if the mode is MD_DRAW_BBOX, we need to print the label, otherwise we only need to calculate
        // its bounding box, so that the caller can use it to position the button correctly.
        return process_text(&label_as_text, pos, out_bbox, mode);
    }

    // If the button has a border, we need to consider the label and the border itself.
    // First of all, let's calculate the bounding box of the label, so that we can center it
    LCD_BBox label_bbox;
    if (!process_text(&label_as_text, pos, &label_bbox, MD_BBOX))
        return false;

    const LCD_Dimension label_bbox_dim = {
        .width = abs(label_bbox.bottom_right.x - label_bbox.top_left.x),
        .height = abs(label_bbox.bottom_right.y - label_bbox.top_left.y),
    };

    const LCD_Rect border = {
        .width = label_bbox_dim.width + button->padding.left + button->padding.right,
        .height = label_bbox_dim.height + button->padding.top + button->padding.bottom,
        .edge_color = button->edge_color,
        .fill_color = button->fill_color,
    };

    // If the button has a border, its bbox is the border itself, hence we're setting it
    // right from the process_rect function.
    process_rect(&border, pos, out_bbox, mode);

    // Now we need to center the label in the button
    const LCD_Coordinate label_coords = {
        .x = pos.x + (border.width - label_bbox_dim.width) / 2,
        .y = pos.y + (border.height - label_bbox_dim.height) / 2,
    };

    process_text(&label_as_text, label_coords, NULL, mode);
    return true;
}

_PRIVATE bool delete_button(const LCD_Button *const button, LCD_Coordinate pos)
{
    if (!button)
        return false;

    // We need to delete the label, and retain it's bbox, since we need the
    // dimensions for deleting the border too.

    LCD_BBox label_bbox;
    if (!process_text(
            &(LCD_Text){
                .text = button->label.text,
                .font = button->label.font,
                .text_color = LCDCurrentBGColor,
                .bg_color = LCD_COL_NONE, // BG is never drawn w/ labels.
                .char_spacing = button->label.char_spacing,
                .line_spacing = button->label.line_spacing,
            },
            pos, &label_bbox, MD_DELETE | MD_BBOX))
        return false;

    // If edge_color and/or fill_color are defined, we only need to delete the label
    if (button->edge_color == LCD_COL_NONE && button->fill_color == LCD_COL_NONE)
        return true;

    LCD_Dimension label_dim = {
        .width = abs(label_bbox.bottom_right.x - label_bbox.top_left.x),
        .height = abs(label_bbox.bottom_right.y - label_bbox.top_left.y),
    };

    // Otherwise, we need to delete the border too.
    const LCD_Rect border = {
        .width = label_dim.width + button->padding.left + button->padding.right,
        .height = label_dim.height + button->padding.top + button->padding.bottom,
        .edge_color = LCDCurrentBGColor,
        .fill_color = LCDCurrentBGColor,
    };

    process_rect(&border, pos, NULL, MD_DELETE);
    return true;
}

// PRIVATE BBOX FUNCTIONS

_PRIVATE inline LCD_BBox get_union_bbox(LCD_Component *comps, u16 comps_sz)
{
    if (!comps || comps_sz == 0)
        return (LCD_BBox){0};

    LCD_BBox bbox = comps[0].cached_bbox, *current_comp_bbox;
    for (u16 i = 0; i < comps_sz; i++)
    {
        current_comp_bbox = &comps[i].cached_bbox;
        if (current_comp_bbox->top_left.x < bbox.top_left.x)
            bbox.top_left.x = current_comp_bbox->top_left.x;

        if (current_comp_bbox->top_left.y < bbox.top_left.y)
            bbox.top_left.y = current_comp_bbox->top_left.y;

        if (current_comp_bbox->bottom_right.x > bbox.bottom_right.x)
            bbox.bottom_right.x = current_comp_bbox->bottom_right.x;

        if (current_comp_bbox->bottom_right.y > bbox.bottom_right.y)
            bbox.bottom_right.y = current_comp_bbox->bottom_right.y;
    }

    return bbox;
}

// PUBLIC FUNCTIONS

bool __LCD_PROC_DoProcessObject(const LCD_Obj *const obj, LCD_BBox *out_bbox, u8 mode)
{
    if (!obj || !obj->comps || obj->comps_size == 0)
        return false;

    bool res = true;
    LCD_Component *comp;
    for (u16 i = 0; i < obj->comps_size && res; i++)
    {
        comp = &obj->comps[i];
        switch (comp->type)
        {
        case LCD_COMP_LINE:
            // Cannot delete faster, skipping MD_FAST check
            res = process_line(comp->object.line, &comp->cached_bbox, mode);
            break;
        case LCD_COMP_RECT:
            // Cannot delete faster, skipping MD_FAST check
            res = process_rect(comp->object.rect, comp->pos, &comp->cached_bbox, mode);
            break;
        case LCD_COMP_CIRCLE:
            // Maybe can delete faster?
            res = process_circle(comp->object.circle, &comp->cached_bbox, mode);
            break;
        case LCD_COMP_IMAGE: {
            if (IS_MODE(mode, MD_DELETE))
            {
                res = process_rect(
                    &(LCD_Rect){
                        .width = comp->object.image->width,
                        .height = comp->object.image->height,
                        .fill_color = LCDCurrentBGColor,
                        .edge_color = LCDCurrentBGColor,
                    },
                    comp->pos, NULL, MD_DELETE);
            }
            else
                res = process_img_rle(comp->object.image, comp->pos, &comp->cached_bbox, MD_DRAW_BBOX);
            break;
        }
        case LCD_COMP_TEXT:
            if (IS_MODE(mode, MD_DELETE | MD_DELETE_FAST))
            {
                // If we're deleting in fast mode, we don't delete char by char, but we simply paint
                // the whole text's bounding box (i.e. a rectangle) with the background color. Faster?
                LCD_BBox bbox;
                if (!(res = process_text(comp->object.text, comp->pos, &bbox, MD_BBOX)))
                    break;

                res = process_rect(
                    &(LCD_Rect){
                        .width = abs(bbox.top_left.x - bbox.bottom_right.x),
                        .height = abs(bbox.top_left.y - bbox.bottom_right.y),
                    },
                    comp->pos, NULL, MD_DELETE);
                break;
            }
            else
                res = process_text(comp->object.text, comp->pos, &comp->cached_bbox, mode);
            break;
        case LCD_COMP_BUTTON: {
            if (IS_MODE(mode, MD_DELETE))
                res = delete_button(comp->object.button, comp->pos);
            else
                res = process_button(comp->object.button, comp->pos, &comp->cached_bbox, mode);
            break;
        }
        }
    }

    if (!res)
        return false;

    // If we need to calculate the bounding box of the object, we need to calculate the union of all
    // the bounding boxes of the components. This is needed to determine the actual dimensions of the
    // object, so that the caller can use them to position it correctly.
    if (out_bbox && IS_MODE(mode, MD_BBOX | MD_DRAW_BBOX))
        *out_bbox = get_union_bbox(obj->comps, obj->comps_size);

    return true;
}