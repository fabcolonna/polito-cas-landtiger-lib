#include "glcd.h"

#include <LPC17xx.h>
#include <alloca.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// PRIVATE DEFINES

/// @brief LCD Controller PINs are connected to the GPIO0 port (same as the LEDs).
///        Hence, they're controlled using the FIOSET and FIOCLR registers.
/// @details EN = Enable (19)
/// @details LE = Latch Enable (20)
/// @details DIR = Bus Direction (21) (0 = Write, 1 = Read)
/// @details CS = Chip Select (22)
/// @details RS = Register Select (23) (0 = Command, 1 = Data)
/// @details WR = Write Operation (24)
/// @details RD = Read Operation (25)
#define PIN_EN (1 << 19)
#define PIN_LE (1 << 20)
#define PIN_DIR (1 << 21)
#define PIN_CS (1 << 22)
#define PIN_RS (1 << 23)
#define PIN_WR (1 << 24)
#define PIN_RD (1 << 25)

#define LCD_EN(x) ((x) ? (LPC_GPIO0->FIOSET = PIN_EN) : (LPC_GPIO0->FIOCLR = PIN_EN))
#define LCD_LE(x) ((x) ? (LPC_GPIO0->FIOSET = PIN_LE) : (LPC_GPIO0->FIOCLR = PIN_LE))
#define LCD_DIR(x) ((x) ? (LPC_GPIO0->FIOSET = PIN_DIR) : (LPC_GPIO0->FIOCLR = PIN_DIR))
#define LCD_CS(x) ((x) ? (LPC_GPIO0->FIOSET = PIN_CS) : (LPC_GPIO0->FIOCLR = PIN_CS))
#define LCD_RS(x) ((x) ? (LPC_GPIO0->FIOSET = PIN_RS) : (LPC_GPIO0->FIOCLR = PIN_RS))
#define LCD_WR(x) ((x) ? (LPC_GPIO0->FIOSET = PIN_WR) : (LPC_GPIO0->FIOCLR = PIN_WR))
#define LCD_RD(x) ((x) ? (LPC_GPIO0->FIOSET = PIN_RD) : (LPC_GPIO0->FIOCLR = PIN_RD))

#define DELAY_COUNT(count) for (u16 i = count; i > 0; i--)

#define DELAY_MS(ms)                                                                                                   \
    for (u16 i = 0; i < ms; i++)                                                                                       \
        for (u16 j = 0; j < 1141; j++)

#define SWAP(type, src, dst)                                                                                           \
    {                                                                                                                  \
        type tmp = src;                                                                                                \
        src = dst;                                                                                                     \
        dst = tmp;                                                                                                     \
    }

/// @brief Models.
typedef enum
{
    ILI9320,   // 0x9320
    ILI9325,   // 0x9325
    ILI9328,   // 0x9328
    ILI9331,   // 0x9331
    SSD1298,   // 0x8999
    SSD1289,   // 0x8989
    ST7781,    // 0x7783
    LGDP4531,  // 0x4531
    SPFD5408B, // 0x5408
    R61505U,   // 0x1505 0x0505
    HX8346A,   // 0x0046
    HX8347D,   // 0x0047
    HX8347A,   // 0x0047
    LGDP4535,  // 0x4535
    SSD2119    // 3.5 LCD 0x9919
} LCD_Model;

// VARIABLES

/// @brief Code for the current LCD peripheral.
_PRIVATE u8 model_code;
_PRIVATE bool initialized = false;

/// @brief Current LCD orientation and maximum X and Y coordinates.
_PRIVATE u16 lcd_orientation;
_PRIVATE u16 MAX_X, MAX_Y;

/// @brief Current background color of the screen.
_PRIVATE LCD_Color current_bg_color = LCD_COL_BLACK;

// RENDER QUEUE

typedef struct
{
    LCD_ObjID id; // -1 if the object has been removed
    LCD_Obj *obj; // Can be NULL if the object has been removed
    bool visible, rendered;
} LCD_RQItem;

/// @brief Maximum number of components that can be rendered on the screen.
#define MAX_RQ_ITEMS 1024

/// @brief The render queue, containing all the components to be rendered.
///        It also contains info about the visibility and rendering status
///        of each component. The queue is also paired with a free list of
///        the same size, to keep track of the free slots in the queue.
_PRIVATE LCD_RQItem render_queue[MAX_RQ_ITEMS] = {0};
_PRIVATE u32 render_queue_free_list[MAX_RQ_ITEMS] = {0};

/// @brief Keeps track of the number of free slots in the render queue.
///        A slot is free if the index = -1 and the obj pointer is NULL.
_PRIVATE u32 render_queue_free_list_count;
_PRIVATE u32 render_queue_size;

/// @brief Maximum number of fonts that can be loaded into the LCD.
#define MAX_FONTS 16

_PRIVATE LCD_Font font_list[MAX_FONTS] = {0};
_PRIVATE u8 font_list_size = 0;

// PRIVATE LOW LEVEL FUNCTIONS

/// @brief Writes an half-word (16 bits) into the LCD's DB port.
/// @param byte The HW to write.
/// @note Writing to DB requires a specific sequence of operations, timed
///       based on the specific protocol.
/// @note DB[7..0] and DB[15..8] are connected to the same pins 2.[0..7].
//        Low bits and high bits need to be sent separately, with specific timings.
_PRIVATE inline void send(u16 halfw)
{
    LPC_GPIO2->FIODIR |= 0xFF; /* P2.0...P2.7 Output */
    LCD_DIR(1);                /* Interface A->B */
    LCD_EN(0);                 /* Enable 2A->2B */
    LPC_GPIO2->FIOPIN = halfw; /* Write D0..D7 */
    LCD_LE(1);
    LCD_LE(0);                      /* latch D0..D7	*/
    LPC_GPIO2->FIOPIN = halfw >> 8; /* Write D8..D15 */
}

/// @brief Reads an half-word (16 bits) from the LCD's DB port.
/// @return The HW read.
_PRIVATE inline u16 recv(void)
{
    u16 value;

    LPC_GPIO2->FIODIR &= ~(0xFF);              /* P2.0...P2.7 Input */
    LCD_DIR(0);                                /* Interface B->A */
    LCD_EN(0);                                 /* Enable 2B->2A */
    DELAY_COUNT(30);                           /* delay some times */
    value = LPC_GPIO2->FIOPIN0;                /* Read D8..D15 */
    LCD_EN(1);                                 /* Enable 1B->1A */
    DELAY_COUNT(30);                           /* delay some times */
    value = (value << 8) | LPC_GPIO2->FIOPIN0; /* Read D0..D7 */
    LCD_DIR(1);

    return value;
}

/// @brief Tells the LCD controller to initiate a R/W operation
///        with the register at the index specified.
/// @param index The index of the register to read/write.
_PRIVATE void init_rw_operation_at(u16 index)
{
    LCD_CS(0);
    LCD_RS(0);
    LCD_RD(1);
    send(index);
    DELAY_COUNT(22);
    LCD_WR(0);
    DELAY_COUNT(1);
    LCD_WR(1);
    LCD_CS(1);
}

/// @brief Writes an half-word (16 bits) into the LCD's DB port,
///        destined to the register at the index selected with the
///        previous call to init_ro_operation_at.
/// @param data The HW to write.
/// @note Before calling this function, init_rw_operation_at must be called
///       to select the register to write to.
_PRIVATE void do_write(u16 data)
{
    LCD_CS(0);
    LCD_RS(1);
    send(data);
    LCD_WR(0);
    DELAY_COUNT(1);
    LCD_WR(1);
    LCD_CS(1);
}

/// @brief Reads an half-word (16 bits) from the LCD's DB port,
///        from the register at the index selected with the
///        previous call to init_ro_operation_at.
/// @return The HW read.
/// @note Before calling this function, init_rw_operation_at must be called
///       to select the register to read from.
_PRIVATE u16 do_read(void)
{
    LCD_CS(0);
    LCD_RS(1);
    LCD_WR(1);
    LCD_RD(0);
    const u16 value = recv();
    LCD_RD(1);
    LCD_CS(1);
    return value;
}

/// @brief Writes a 16-bit value to the LCD controller, at the specified register.
/// @param reg The register to write to.
/// @param data The value to write.
_PRIVATE void write_to(u16 reg, u16 data)
{
    init_rw_operation_at(reg);
    do_write(data);
}

/// @brief Reads an HW from the LCD controller, at the specified register.
/// @param reg The register to read from.
/// @return The HW.
_PRIVATE u16 read_from(u16 reg)
{
    init_rw_operation_at(reg);
    return do_read();
}

/// @brief Sets the GRAM cursor to the specified coordinates, for writing
///        data into the appropriate pixel in the display.
/// @param x The X coordinate (column).
/// @param y The Y coordinate (row).
_PRIVATE void set_gram_cursor(u16 x, u16 y)
{
    if (lcd_orientation == LCD_ORIENT_HOR || lcd_orientation == LCD_ORIENT_HOR_INV)
    {
        // Swap X and Y if orientation is 90 or 270 degrees
        u16 tmp = x;
        x = y;
        y = (MAX_X - 1) - tmp;
    }

    switch (model_code)
    {
    default:                 /* 0x9320 0x9325 0x9328 0x9331 0x5408 0x1505 0x0505 0x7783 0x4531 0x4535 */
        write_to(0x0020, x); // 0x20 = GRAM Address Set (Horizontal Address)
        write_to(0x0021, y); // 0x21 = GRAM Address Set (Vertical Address)
        break;

    case SSD1298: /* 0x8999 */
    case SSD1289: /* 0x8989 */
        write_to(0x004e, x);
        write_to(0x004f, y);
        break;

    case HX8346A: /* 0x0046 */
    case HX8347A: /* 0x0047 */
    case HX8347D: /* 0x0047 */
        write_to(0x02, x >> 8);
        write_to(0x03, x);

        write_to(0x06, y >> 8);
        write_to(0x07, y);
        break;

    case SSD2119: /* 3.5 LCD 0x9919 */
        break;
    }
}

// PRIVATE DRAWING FUNCTIONS

#define SET_POINT_SIMPLER(color, x, y) LCD_SetPointColor(color, (LCD_Coordinate){x, y})

_PRIVATE void draw_line(const LCD_Line *const line)
{
    if (!line)
        return;

    // If the starting point is to the right of the ending point (both for x and/or y), swap.
    LCD_Coordinate from = line->from, to = line->to;

    if (from.x > to.x)
        SWAP(u16, to.x, from.x)
    if (from.y > to.y)
        SWAP(u16, to.y, from.y)

    u16 tmp, dx = to.x - from.x, dy = to.y - from.y;
    if (dx == 0) // Vertical line
    {
        while (from.y <= to.y)
            SET_POINT_SIMPLER(line->color, from.x, from.y++);
    }
    else if (dy == 0) // Horizontal line
    {
        while (from.x <= to.x)
            SET_POINT_SIMPLER(line->color, from.x++, from.y);
    }
    else if (dx > dy)
    {
        tmp = 2 * dy - dx;
        while (from.x != to.x)
        {
            SET_POINT_SIMPLER(line->color, from.x++, from.y);
            if (tmp > 0)
            {
                from.y++;
                tmp += 2 * dy - 2 * dx;
            }
            else
                tmp += 2 * dy;
        }
        SET_POINT_SIMPLER(line->color, from.x, from.y);
    }
    else
    {
        tmp = 2 * dx - dy;
        while (from.y != to.y)
        {
            SET_POINT_SIMPLER(line->color, from.x, from.y++);
            if (tmp > 0)
            {
                from.x++;
                tmp += 2 * dy - 2 * dx;
            }
            else
                tmp += 2 * dy;
        }
        SET_POINT_SIMPLER(line->color, from.x, from.y);
    }
}

_PRIVATE void draw_rect(const LCD_Rect *const rect, LCD_Coordinate pos)
{
    if (!rect)
        return;

    // Rect object now contains width and height, not the start/end coordinates.
    // This, to support the rendering of the rectangle at various positions on the
    // screen, based on the pos parameter. pos is the top left corner of the rectangle.
    if (rect->width == 0 || rect->height == 0)
        return;

    LCD_Coordinate from = pos;
    LCD_Coordinate to = {from.x + rect->width, from.y + rect->height};
    if (from.x >= MAX_X || from.y >= MAX_Y)
        return; // Rectangle is completely outside the screen

    // If the ending point is outside the screen, adjust it.
    if (to.x >= MAX_X)
        to.x = MAX_X - 1;
    if (to.y >= MAX_Y)
        to.y = MAX_Y - 1;

    // If the starting point is to the right of the ending point (both for x and/or y), swap.
    if (from.x > to.x)
        SWAP(u16, to.x, from.x)
    if (from.y > to.y)
        SWAP(u16, to.y, from.y)

    // Drawing the edges of the rectangle
    LCD_Line line = {.color = rect->edge_color};

    line.from = from;
    line.to = (LCD_Coordinate){to.x, from.y};
    draw_line(&line); // Top horizontal edge
    line.to = (LCD_Coordinate){from.x, to.y};
    draw_line(&line); // Left vertical edge

    line.from = to;
    line.to = (LCD_Coordinate){to.x, from.y};
    draw_line(&line); // Right vertical edge
    line.to = (LCD_Coordinate){from.x, to.y};
    draw_line(&line); // Bottom horizontal edge

    if (rect->fill_color != LCD_COL_NONE)
    {
        // Filling the rectangle
        for (u16 i = from.x + 1; i < to.x; i++)
            for (u16 j = from.y + 1; j < to.y; j++)
                SET_POINT_SIMPLER(rect->fill_color, i, j);
    }
}

_PRIVATE void draw_circle(const LCD_Circle *const circle)
{
    const LCD_Coordinate center = circle->center;
    const u16 radius = circle->radius;

    if (radius == 0)
        return;

    // Printing 4 cardinal points of the circle: note that if the radius is subtracted from y,
    // from the center, the pixel will be actually colored at the other side, because smaller
    // numbers mean higher positions in the screen.
    SET_POINT_SIMPLER(circle->edge_color, center.x + radius, center.y); // Right side of the center
    SET_POINT_SIMPLER(circle->edge_color, center.x - radius, center.y); // Left side
    SET_POINT_SIMPLER(circle->edge_color, center.x, center.y + radius); // Top, but we +, it's the bottom
    SET_POINT_SIMPLER(circle->edge_color, center.x, center.y - radius); // Bottom, but we -, it's the top

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
        SET_POINT_SIMPLER(circle->edge_color, x + center.x, y + center.y);
        SET_POINT_SIMPLER(circle->edge_color, -x + center.x, y + center.y);
        SET_POINT_SIMPLER(circle->edge_color, x + center.x, -y + center.y);
        SET_POINT_SIMPLER(circle->edge_color, -x + center.x, -y + center.y);
        SET_POINT_SIMPLER(circle->edge_color, y + center.x, x + center.y);
        SET_POINT_SIMPLER(circle->edge_color, -y + center.x, x + center.y);
        SET_POINT_SIMPLER(circle->edge_color, y + center.x, -x + center.y);
        SET_POINT_SIMPLER(circle->edge_color, -y + center.x, -x + center.y);
    }

    if (circle->fill_color != LCD_COL_NONE)
    {
        for (u16 i = center.x - radius + 1; i < center.x + radius; i++)
            for (u16 j = center.y - radius + 1; j < center.y + radius; j++)
                if (pow((i - center.x), 2) + pow((j - center.y), 2) <= pow(radius, 2)) // Inside the circle
                    SET_POINT_SIMPLER(circle->fill_color, i, j);
    }
}

_PRIVATE void draw_img_rle(const LCD_Image *const img, LCD_Coordinate pos)
{
    if (!img || !img->pixels)
        return;

    u16 width = img->width, height = img->height;
    if (width == 0 || height == 0)
        return;

    // Pos is the top-left corner of the image.
    if (pos.x >= MAX_X || pos.y >= MAX_Y)
        return; // Image is completely outside the screen

    // If the image is partially outside the screen, adjust its width and height.
    if (pos.x + width > MAX_X)
        width = MAX_X - pos.x;
    if (pos.y + height > MAX_Y)
        height = MAX_Y - pos.y;

    const u32 *pixels_copy = img->pixels;
    u32 pixel_data, rgb888; // Pixel is a 32-bit or 24-bit value: 0xAARRGGBB if alpha is present, 0x00RRGGBB otherwise

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
                        SET_POINT_SIMPLER(rgb888_to_rgb565(rgb888), current_x, pos.y + i);
                    
                    current_x++; // Move to the next pixel, regardless of the alpha value
                }
                else
                {
                    // No alpha, pixel_data is of type 0x00RRGGBB
                    rgb888 = (u32)(pixel_data & 0x00FFFFFF);
                    SET_POINT_SIMPLER(rgb888_to_rgb565(rgb888), current_x++, pos.y + i);
                }

                pixels_left--;
            }
        }
    }
}

// TEXT FUNCTIONS

// Prints the char and returns the width of the char printed, or -1 if something went wrong.
_PRIVATE i16 print_char(u8 chr, const LCD_Font *const font, const LCD_Coordinate *const where, LCD_Color txt_col,
                        LCD_Color bg_col)
{
    // We can't print non-ASCII printable chars, and other invalid values
    if (!font || !where || chr < ASCII_FONT_MIN_VALUE || chr > ASCII_FONT_MAX_VALUE)
        return -1;

    if (where->x >= MAX_X || where->y >= MAX_Y)
        return -1; // Char is completely outside the screen

    // Each line of the array contains the char data, and chars are stored in ASCII order. Each line is
    // made up of font->char_height different u32 values, each one representing a row of the given char.
    // Hence, for every char, I need to retrieve font->char_height u32 values from the array line given
    // by chr - ASCII_FONT_MIN_VALUE, since the first printable ASCII char is 32.
    // For example: A (=65) - 32 = 33, so starting from the 33rd line of the array, I need to read
    // font->char_height u32 values.
    const u32 *font_data_copy = font->data;
    font_data_copy += (chr - ASCII_FONT_MIN_VALUE) * font->char_height; // Move to the correct line

    u32 *const chr_data = (u32 *)alloca(font->char_height * sizeof(u32));
    memcpy(chr_data, font_data_copy, font->char_height * sizeof(u32)); // Copying the row

    u32 value;
    for (u16 i = 0; i < font->char_height; i++) // For each char row, we have font->char_width bits
    {
        for (u16 j = 0; j < font->max_char_width; j++)
        {
            // Since a char value is not necessarily 32 bits long, but its length depends on
            // the char width, we mask the MSB that are not part of the char data.
            // E.g. 8 bits, we & with 0xFFFFFFFF >> (32 - 8) = 0xFFFFFFFF >> 24 = 0x000000FF
            value = chr_data[i] & (0xFFFFFFFF >> (32 - font->max_char_width));

            // If the bit at position width - 1 - j is 1, print it with the text color.
            if ((value >> (font->max_char_width - 1 - j)) & 0x1)
                SET_POINT_SIMPLER(txt_col, where->x + j, where->y + i);
            else // Otherwise, print the background color, because that pixel is not part of the chr.
                SET_POINT_SIMPLER(bg_col, where->x + j, where->y + i);
        }
    }

    // We need to determine the width of the selected char, so we can move forward to the next char
    // leaving just the right amount of space between them. If the font data has been created with
    // the ttf2c script, chances are that the width of the char is stored in the char_widths array.
    // The array is also ordered by ASCII value, so we can directly access the width of the char
    // using the char itself as index. If the array is NULL, we don't have info about the individual
    // char widths, so we can just use the max_char_width property, which is always defined.
    return (font->char_widths) ? font->char_widths[chr - ASCII_FONT_MIN_VALUE] : font->max_char_width;
}

_PRIVATE void print_text(const LCD_Text *const text, LCD_Coordinate pos)
{
    if (!text || !text->text)
        return;

    const LCD_FontID font_id = text->font;
    if (font_id >= font_list_size)
        return;

    const LCD_Font font = font_list[font_id];
    if (!font.data)
        return;

    if (pos.x >= MAX_X || pos.y >= MAX_Y)
        return; // Text is completely outside the screen

    u8 chr, *str = (u8 *)(text->text);

    u16 max_width = 0;                   // Track the maximum width (for multi-line)
    u16 start_x = pos.x;                 // Saving the starting x position
    // u16 total_height = font.char_height; // Minimum height is one line (font height)

    i16 current_char_width;
    bool no_more_space = false;
    while ((chr = *str++) && !no_more_space)
    {
        if ((current_char_width = print_char(chr, &font, &pos, text->text_color, text->bg_color)) < 0)
            return; // Something went wrong, should not happen!

        // Moving to the next char position
        if (pos.x + current_char_width < MAX_X) // Continue on the same line
            pos.x += current_char_width;
        else if (pos.y + font.char_height < MAX_Y) // Go to the next line if there's space on the x axis
        {
            pos.x = 0;
            pos.y += font.char_height;
            // total_height += font.char_height; // Increment total height for each new line
        }
        else
            no_more_space = true; // Stop printing if there's no more space

        // Update the maximum width for the current line
        u16 current_width = pos.x - start_x;
        if (current_width > max_width)
            max_width = current_width;
    }

    // If the string is short and doesn't wrap, set max_width correctly
    if (max_width == 0)
        max_width = pos.x - start_x;
}

// PRIVATE RENDER QUEUE FUNCTIONS

_PRIVATE void render_item(LCD_RQItem *const item)
{
    if (!item || !item->obj || item->id < 0 || !item->obj->comps || item->obj->comps_size == 0)
        return;

    // If the object is already rendered or not visible, skip it
    if (item->rendered || !item->visible)
        return;

    LCD_Component *comp;
    for (u16 i = 0; i < item->obj->comps_size; i++)
    {
        comp = &item->obj->comps[i];
        switch (comp->type)
        {
        case LCD_COMP_LINE:
            draw_line(&comp->object.line);
            break;
        case LCD_COMP_RECT:
            draw_rect(&comp->object.rect, comp->pos);
            break;
        case LCD_COMP_CIRCLE:
            draw_circle(&comp->object.circle);
            break;
        case LCD_COMP_IMAGE:
            draw_img_rle(&comp->object.image, comp->pos);
            break;
        case LCD_COMP_TEXT:
            print_text(&comp->object.text, comp->pos);
            break;
        }
    }

    item->rendered = true;
}

_PRIVATE void unrender_item(LCD_RQItem *const item)
{
    if (!item || !item->obj || item->id < 0 || !item->obj->comps || item->obj->comps_size == 0)
        return;

    // If the object is not rendered, skip it
    if (!item->rendered)
        return;

    LCD_Component *comp;
    for (u16 i = 0; i < item->obj->comps_size; i++)
    {
        comp = &item->obj->comps[i];
        if (comp->type == LCD_COMP_LINE)
        {
            LCD_Line line = comp->object.line;
            line.color = current_bg_color;
            draw_line(&line);
        }
        else if (comp->type == LCD_COMP_RECT)
        {
            LCD_Rect rect = comp->object.rect;
            rect.edge_color = current_bg_color;
            if (rect.fill_color != LCD_COL_NONE)
                rect.fill_color = current_bg_color;

            draw_rect(&rect, comp->pos);
        }
        else if (comp->type == LCD_COMP_CIRCLE)
        {
            LCD_Circle circle = comp->object.circle;
            circle.edge_color = current_bg_color;
            if (circle.fill_color != LCD_COL_NONE)
                circle.fill_color = current_bg_color;

            draw_circle(&circle);
        }
        else if (comp->type == LCD_COMP_IMAGE)
        {
            const LCD_Rect rect = {
                .width = comp->object.image.width,
                .height = comp->object.image.height,
                .fill_color = current_bg_color,
                .edge_color = current_bg_color,
            };

            draw_rect(&rect, comp->pos);
        }
        else if (comp->type == LCD_COMP_TEXT)
        {
            LCD_Text text = comp->object.text;
            text.text_color = current_bg_color;
            text.bg_color = current_bg_color;

            print_text(&text, comp->pos);
        }
    }

    item->rendered = false;
}

// PUBLIC FUNCTIONS

#include "font_msgothic.h"
#include "font_system.h"

void LCD_Init(LCD_Orientation orientation)
{
    // Setting PINS as 00 (GPIO) in PINSEL0 (bit 6 to 25) for P0.19 to P0.25
    // 0000 0011 1111 1111 1111 1111 1100 0000
    LPC_PINCON->PINSEL1 &= ~(0x03FFFFC0);

    // Setting 00 in PINSEL4 (bit 0 to 15) for P2.0 to P2.7
    // 0000 0000 0000 0000 1111 1111 1111 1111
    LPC_PINCON->PINSEL4 &= ~(0x0000FFFF);

    /* EN = P0.19 , LE = P0.20 , DIR = P0.21 , CS = P0.22 , RS = P0.23 , RS = P0.23 */
    /* RS = P0.23 , WR = P0.24 , RD = P0.25 , DB[0.7] = P2.0...P2.7 , DB[8.15]= P2.0...P2.7 */
    LPC_GPIO0->FIODIR |= 0x03f80000;
    LPC_GPIO0->FIOSET = 0x03f80000;

    lcd_orientation = orientation;
    if (orientation == LCD_ORIENT_VER || orientation == LCD_ORIENT_VER_INV)
    {
        MAX_X = 240;
        MAX_Y = 320;
    }
    else
    {
        MAX_X = 320;
        MAX_Y = 240;
    }

    DELAY_MS(100);
    const u16 code = read_from(0x0000);
    if (code == 0x9325 || code == 0x9328)
    {
        model_code = ILI9325;
        write_to(0x00e7, 0x0010); // Test Register
        write_to(0x0000, 0x0001); // Starts internal OSC
        write_to(0x0001, 0x0100); // Output driver control

        write_to(0x0002, 0x0700);                                     // LCD Driver Waveform Control
        write_to(0x0003, (1 << 12) | (1 << 5) | (1 << 4) | (0 << 3)); // Entry mode
        write_to(0x0004, 0x0000);                                     // Resizing Function
        write_to(0x0008, 0x0207);                                     // Back and Front porch periods
        write_to(0x0009, 0x0000);                                     // Non-Display Area refresh cycle
        write_to(0x000a, 0x0000);                                     // Frame cycle
        write_to(0x000c, 0x0001);
        write_to(0x000d, 0x0000);
        write_to(0x000f, 0x0000);

        // Power Control
        write_to(0x0010, 0x0000);
        write_to(0x0011, 0x0007);
        write_to(0x0012, 0x0000);
        write_to(0x0013, 0x0000);
        DELAY_MS(50);
        write_to(0x0010, 0x1590);
        write_to(0x0011, 0x0227);
        DELAY_MS(50);
        write_to(0x0012, 0x009c);
        DELAY_MS(50);
        write_to(0x0013, 0x1900);
        write_to(0x0029, 0x0023);

        write_to(0x002b, 0x000e); // Frate rate & Color control
        DELAY_MS(50);
        write_to(0x0020, 0x0000); // GRAM horizontal Address
        write_to(0x0021, 0x0000); // GRAM Vertical Address

        DELAY_MS(50);
        write_to(0x0030, 0x0007);
        write_to(0x0031, 0x0707);
        write_to(0x0032, 0x0006);
        write_to(0x0035, 0x0704);
        write_to(0x0036, 0x1f04);
        write_to(0x0037, 0x0004);
        write_to(0x0038, 0x0000);
        write_to(0x0039, 0x0706);
        write_to(0x003c, 0x0701);
        write_to(0x003d, 0x000f);
        DELAY_MS(50);
        write_to(0x0050, 0x0000);
        write_to(0x0051, 0x00ef);
        write_to(0x0052, 0x0000);
        write_to(0x0053, 0x013f);
        write_to(0x0060, 0xa700);
        write_to(0x0061, 0x0001);
        write_to(0x006a, 0x0000);
        write_to(0x0080, 0x0000);
        write_to(0x0081, 0x0000);
        write_to(0x0082, 0x0000);
        write_to(0x0083, 0x0000);
        write_to(0x0084, 0x0000);
        write_to(0x0085, 0x0000);
        write_to(0x0090, 0x0010);
        write_to(0x0092, 0x0000);
        write_to(0x0093, 0x0003);
        write_to(0x0095, 0x0110);
        write_to(0x0097, 0x0000);
        write_to(0x0098, 0x0000);

        // Display On Sequence
        write_to(0x0007, 0x0133);
        write_to(0x0020, 0x0000);
        write_to(0x0021, 0x0000);
    }

    DELAY_MS(50);

    // Loading the two default fonts: MS Gothic & System
    font_list[0] = Font_MSGothic;
    font_list[1] = Font_System;
    font_list_size = 2;

    // Initializing the RQ and the free list
    render_queue_free_list_count = MAX_RQ_ITEMS;
    render_queue_size = 0;

    // Each element in the free list is an index in the RQ. The free list is
    // initialized with the indexes of the RQ elements, in reverse order. This
    // is because, when adding new objects, the free slot will be retrieved by
    // accessing the last element in the free list, and then decrementing the
    // free list count. When removing an object, the slot will be added back to
    // the free list at the cell currently pointed by the free list count, which
    // will be then incremented.
    for (u32 i = 0; i < MAX_RQ_ITEMS; i++)
        render_queue_free_list[i] = MAX_RQ_ITEMS - 1 - i;

    initialized = true;
}

bool LCD_IsInitialized(void)
{
    return initialized;
}

u16 LCD_GetWidth(void)
{
    return MAX_X;
}

u16 LCD_GetHeight(void)
{
    return MAX_Y;
}

LCD_Coordinate LCD_GetSize(void)
{
    return (LCD_Coordinate){MAX_X, MAX_Y};
}

LCD_Coordinate LCD_GetCenter(void)
{
    return (LCD_Coordinate){MAX_X / 2, MAX_Y / 2};
}

LCD_Color LCD_GetPointColor(LCD_Coordinate point)
{
    LCD_Color dummy;

    set_gram_cursor(point.x, point.y);
    init_rw_operation_at(0x0022); // Write GRAM

    switch (model_code)
    {
    case ST7781:
    case LGDP4531:
    case LGDP4535:
    case SSD1289:
    case SSD1298:
        dummy = do_read(); /* Empty read */
        dummy = do_read();
        return dummy;
    case HX8347A:
    case HX8347D: {
        dummy = do_read(); /* Empty read */

        const u8 red = do_read() >> 3;
        const u8 green = do_read() >> 2;
        const u8 blue = do_read() >> 3;
        dummy = (u16)((red << 11) | (green << 5) | blue);
        return dummy;
    }
    default:               /* 0x9320 0x9325 0x9328 0x9331 0x5408 0x1505 0x0505 0x9919 */
        dummy = do_read(); /* Empty read */
        dummy = do_read();
        return BGR_TO_RGB565(dummy);
    }
}

void LCD_SetPointColor(LCD_Color color, LCD_Coordinate point)
{
    if (point.x >= MAX_X || point.y >= MAX_Y)
        return;

    set_gram_cursor(point.x, point.y);
    if (color != LCD_COL_NONE)
        write_to(0x0022, color & 0xFFFF);
}

void LCD_SetBackgroundColor(LCD_Color color)
{
    if (color == LCD_COL_NONE)
        return;

    current_bg_color = color;

    // Sets the GRAM cursor to the top-left corner of the display
    if (model_code == HX8347D || model_code == HX8347A)
    {
        write_to(0x02, 0x00);
        write_to(0x03, 0x00);
        write_to(0x04, 0x00);
        write_to(0x05, 0xEF);
        write_to(0x06, 0x00);
        write_to(0x07, 0x00);
        write_to(0x08, 0x01);
        write_to(0x09, 0x3F);
    }
    else
        set_gram_cursor(0, 0);

    init_rw_operation_at(0x0022); // Write GRAM
    for (u32 index = 0; index < MAX_X * MAX_Y; index++)
        do_write(color & 0xFFFF);

    // No object is displayed anymore, so we need to change the rendered
    // property of each object in the render queue to false, so that after
    // calling LCD_RQRender(), they will be re-rendered with the new background.
    LCD_RQItem *item;
    for (u32 i = 0, count = 0; i < MAX_RQ_ITEMS && count != render_queue_size; i++)
    {
        item = &render_queue[i];
        if (item->id == -1 && !item->obj)
            continue;

        count++; // Found actual object, incrementing count.
        item->rendered = false;
    }

    // Re-rendering everything on top of the new background color.
    LCD_RQRender();
}

// RENDERING

LCD_ObjID LCD_RQAddObject(const LCD_Obj *const obj)
{
    if (!obj || !obj->comps || obj->comps_size <= 0)
        return -1;

    // No more space in the render queue
    if (render_queue_free_list_count <= 0)
        return -1;

    const u32 rq_slot = render_queue_free_list[--render_queue_free_list_count];
    LCD_RQItem *item = &render_queue[rq_slot];

    item->id = rq_slot;
    item->obj = (LCD_Obj *)obj;
    item->visible = true; // Object needs to be rendered, eventually
    item->rendered = false;

    render_queue_size++;
    return rq_slot;
}

void LCD_RQRender(void)
{
    // Visiting the render queue and rendering the visible objects.
    // The render queue size tells us how many objects are in the queue, but
    // there may be empty slots in the middle of the queue, so we need to
    // iterate over the whole queue, and stop when we reach <render_queue_size>
    // actual objects (i.e. not NULL objs). We could simply iterate over
    // MAX_RQ_ITEMS, but that would be surely more inefficient.

    LCD_RQItem *item;
    for (u32 i = 0, count = 0; i < MAX_RQ_ITEMS && count != render_queue_size; i++)
    {
        item = &render_queue[i];
        if (item->id == -1 && !item->obj)
            continue;

        count++; // Found actual object, incrementing count.
        if (item->visible && !item->rendered)
            render_item(item);
    }
}

void LCD_RQRemoveObject(LCD_ObjID id, bool redraw_screen)
{
    if (id < 0 || id >= MAX_RQ_ITEMS)
        return;

    // id is the index of the component in the render queue
    LCD_RQItem *const item = &render_queue[id];
    if (!item || item->id == -1 || !item->obj)
        return;

    item->visible = false;
    unrender_item(item);

    // Freeing the RQ_Item
    item->id = -1;
    item->obj = NULL;

    // Adding the slod index to the free list
    render_queue_free_list[render_queue_free_list_count++] = id;
    render_queue_size--;

    if (redraw_screen)
        LCD_RQRender();
}

void LCD_RQSetObjectVisibility(LCD_ObjID id, bool visible, bool redraw_screen)
{
    if (id < 0 || id >= MAX_RQ_ITEMS)
        return;

    LCD_RQItem *const item = &render_queue[id];
    if (!item || item->id == -1 || !item->obj)
        return;

    item->visible = visible;
    if (visible && !item->rendered)
        render_item(item);
    else if (!visible && item->rendered)
    {
        unrender_item(item);
        if (redraw_screen)
            LCD_RQRender();
    }
}

void LCD_RQClear(void)
{
    // Iterating over MAX_ITEMS, and stopping til we NULL'd render_queue_size
    // objects. This is because, although render_queue_size tells us how many
    // objects are in the queue, there may be empty slots in the queue.
    LCD_RQItem *item;
    for (u32 i = 0, count = 0; i < MAX_RQ_ITEMS && count != render_queue_size; i++)
    {
        item = &render_queue[i];
        if (item->id == -1 || !item->obj)
            continue;

        item->id = -1;
        item->obj = NULL;
        item->rendered = false;
        item->visible = false;

        count++;                                                    // Found actual object, incrementing count.
        render_queue_free_list[render_queue_free_list_count++] = i; // Adding index into the FL.
    }

    render_queue_size = 0;
    LCD_SetBackgroundColor(current_bg_color); // Deleting everything
}

LCD_FontID LCD_FMAddFont(LCD_Font font)
{
    if (font_list_size >= MAX_FONTS)
        return -1;

    LCD_FontID id = font_list_size;
    font_list[id] = font;
    font_list_size++;
    return id;
}

void LCD_FMRemoveFont(LCD_FontID id)
{
    if (id >= font_list_size || id <= 1) // Cannot remove the default fonts
        return;

    for (u32 i = id; i < font_list_size - 1; i++)
        font_list[i] = font_list[i + 1];
    font_list_size--;
}