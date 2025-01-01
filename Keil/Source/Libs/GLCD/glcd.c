#include "glcd.h"

#include <LPC17xx.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

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

// PRIVATE TYPES

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

typedef struct
{
    u32 id;
    LCD_Component comp;
    bool visible, rendered;
} LCD_RenderQueueItem;

// VARIABLES

/// @brief Code for the current LCD peripheral.
_PRIVATE u8 model_code;
_PRIVATE bool initialized = false;

/// @brief Current LCD orientation and maximum X and Y coordinates.
_PRIVATE u16 lcd_orientation;
_PRIVATE u16 MAX_X, MAX_Y;

/// @brief Maximum number of components that can be rendered on the screen.
#define MAX_RENDER_QUEUE_COMPS 1024

/// @brief The render queue, containing all the components to be rendered.
_PRIVATE LCD_RenderQueueItem render_queue[MAX_RENDER_QUEUE_COMPS] = {0};
_PRIVATE u32 render_queue_size = 0;
_PRIVATE LCD_Color current_bg_color = LCD_COL_BLACK;

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

_DECL_EXTERNALLY void get_ascii_for(u8 ascii, u8 font, u8 *const out);

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

_PRIVATE void draw_rect(const LCD_Rect *const rect)
{
    if (!rect)
        return;

    LCD_Coordinate from = rect->from, to = rect->to;

    // If the starting point is to the right of the ending point (both for x and/or y), swap.
    u16 tmp;
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

_PRIVATE void draw_img_rle(const LCD_Image *const img)
{
    if (!img)
        return;

    const LCD_Coordinate where = img->pos;
    u16 width = img->width, height = img->height;

    if (!img->pixels || width == 0 || height == 0 || where.x > MAX_X || where.y > MAX_Y)
        return;

    if (where.x + width > MAX_X)
        width = MAX_X - where.x;
    if (where.y + height > MAX_Y)
        height = MAX_Y - where.y;

    const u32 *pixels_copy = img->pixels;
    u16 current_x, pixels_left, count, value, rgb;
    for (u16 i = 0; i < height; i++)
    {
        current_x = where.x; // Start at the beginning of the row
        pixels_left = width; // Remaining pixels to draw in the row

        while (pixels_left > 0)
        {
            count = *pixels_copy++;
            value = *pixels_copy++;

            // Draw pixels, ensuring we stay within the row
            for (u16 j = 0; j < count && pixels_left > 0; j++)
            {
                if (img->has_alpha)
                {
                    if ((value >> 16) & 0xFF) // Alpha is != 0
                    {
                        rgb = (u16)(value & 0x00FFFF);
                        SET_POINT_SIMPLER(rgb, current_x++, where.y + i);
                    }
                }
                else
                    SET_POINT_SIMPLER(value, current_x++, where.y + i);

                pixels_left--;
            }
        }
    }
}

_PRIVATE void draw_img(const LCD_Image *const img)
{
    if (!img)
        return;

    LCD_Coordinate where = img->pos;
    u16 width = img->width, height = img->height;

    if (!img->pixels || width == 0 || height == 0 || where.x > MAX_X || where.y > MAX_Y)
        return;

    if (where.x + width > MAX_X)
        width = MAX_X - where.x;
    if (where.y + height > MAX_Y)
        height = MAX_Y - where.y;

    u32 pixel;
    u16 rgb;
    for (u16 i = 0; i < height; i++)
    {
        for (u16 j = 0; j < width; j++)
        {
            pixel = img->pixels[j + i * width];
            if (img->has_alpha)
            {
                if ((pixel >> 16) & 0xFF) // Alpha != 0
                {
                    rgb = (u16)(pixel & 0x00FFFF);
                    SET_POINT_SIMPLER(rgb, where.x + j, where.y + i);
                }
            }
            else
                SET_POINT_SIMPLER(pixel, where.x + j, where.y + i);
        }
    }
}

_PRIVATE void print_text(const LCD_Text *const text)
{
    if (!text || !text->text)
        return;

    LCD_Coordinate where = text->pos;
    u8 chr, *str = (u8 *)(text->text), ascii_buf[16];

    u16 max_width = 0;     // Track the maximum width (for multi-line)
    u16 start_x = where.x; // Saving the starting x position
    u16 total_height = 16; // Minimum height is one line (font height)

    bool no_more_space = false;
    while ((chr = *str++) && !no_more_space)
    {
        get_ascii_for(chr, text->font, ascii_buf);
        for (u16 i = 0; i < 16; i++)
        {
            for (u16 j = 0; j < 8; j++)
            {
                if ((ascii_buf[i] >> (7 - j)) & 0x1)
                    SET_POINT_SIMPLER(text->text_color, where.x + j, where.y + i);
                else
                    SET_POINT_SIMPLER(text->bg_color, where.x + j, where.y + i);
            }
        }

        // Moving to the next char position
        if (where.x + 8 < MAX_X) // Continue on the same line
            where.x += 8;
        else if (where.y + 16 < MAX_Y) // Go to the next line if there's space on the x axis
        {
            where.x = 0;
            where.y += 16;
            total_height += 16; // Increment total height for each new line
        }
        else
            no_more_space = true; // Stop printing if there's no more space

        // Update the maximum width for the current line
        u16 current_width = where.x - start_x;
        if (current_width > max_width)
            max_width = current_width;
    }

    // If the string is short and doesn't wrap, set max_width correctly
    if (max_width == 0)
        max_width = where.x - start_x;
}

// PRIVATE RENDER QUEUE FUNCTIONS

_PRIVATE void render_comp(u32 id)
{
    if (id >= render_queue_size)
        return;

    LCD_RenderQueueItem *const item = &render_queue[id];
    if (!item->visible || item->rendered)
        return; // Item is already rendered or not visible

    switch (item->comp.type)
    {
    case LCD_COMP_LINE:
        draw_line(&item->comp.object.line);
        break;
    case LCD_COMP_RECT:
        draw_rect(&item->comp.object.rect);
        break;
    case LCD_COMP_CIRCLE:
        draw_circle(&item->comp.object.circle);
        break;
    case LCD_COMP_IMAGE_RLE:
        draw_img_rle(&item->comp.object.image);
        break;
    case LCD_COMP_IMAGE:
        draw_img(&item->comp.object.image);
        break;
    case LCD_COMP_TEXT:
        print_text(&item->comp.object.text);
        break;
    }

    item->visible = true;
    item->rendered = true;
}

/// @brief Removes the component from the screen, and redraws the other
///        components in order to refill the gap left by the removed one.
/// @param id Id of the component to remove.
/// @param redraw_lower_layers If true, the components at lower layers will be redrawn.
_PRIVATE void remove_comp(u32 id, bool redraw_lower_layers)
{
    if (id >= render_queue_size)
        return;

    LCD_RenderQueueItem *const item = &render_queue[id];
    if (!item->rendered)
        return; // Item is not rendered

    const LCD_ComponentType type = item->comp.type;
    ;
    if (type == LCD_COMP_LINE)
    {
        LCD_Line line = item->comp.object.line;
        line.color = current_bg_color;
        draw_line(&line);
    }
    else if (type == LCD_COMP_RECT)
    {
        LCD_Rect rect = item->comp.object.rect;
        rect.edge_color = current_bg_color;
        if (rect.fill_color != LCD_COL_NONE)
            rect.fill_color = current_bg_color;
        draw_rect(&rect);
    }
    else if (type == LCD_COMP_CIRCLE)
    {
        LCD_Circle circle = item->comp.object.circle;
        circle.edge_color = current_bg_color;
        if (circle.fill_color != LCD_COL_NONE)
            circle.fill_color = current_bg_color;
        draw_circle(&circle);
    }
    else if (type == LCD_COMP_IMAGE_RLE || type == LCD_COMP_IMAGE)
    {
        const LCD_Image image = item->comp.object.image;
        const LCD_Coordinate from = image.pos, to = {from.x + image.width, from.y + image.height};

        LCD_Rect rect = {from, to, current_bg_color, current_bg_color};
        draw_rect(&rect);
    }
    else if (type == LCD_COMP_TEXT)
    {
        LCD_Text text = item->comp.object.text;
        text.text_color = current_bg_color;
        text.bg_color = current_bg_color;
        print_text(&text);
    }

    item->visible = false;
    item->rendered = false;

    // Redraw the components at lower layers
    if (redraw_lower_layers)
    {
        for (u32 i = 0; i < render_queue_size; i++)
            if (i != id && render_queue[i].visible && render_queue[i].rendered)
                render_comp(i);
    }
}

// PUBLIC FUNCTIONS

void LCD_Init(u16 orientation)
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
    initialized = true;
}

bool LCD_IsInitialized(void)
{
    return initialized;
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

void LCD_SetPointColor(LCD_Color color, LCD_Coordinate where)
{
    if (where.x >= MAX_X || where.y >= MAX_Y)
        return;

    set_gram_cursor(where.x, where.y);
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
}

i32 LCD_RendererAdd(LCD_Component *const component, bool update)
{
    if (!component || render_queue_size >= MAX_RENDER_QUEUE_COMPS)
        return -1;

    const u32 id = render_queue_size;
    render_queue[id] = (LCD_RenderQueueItem){id, *component, true, false};
    render_queue_size++;

    if (update)
        render_comp(id);

    return id;
}

void LCD_RendererUpdate(void)
{
    for (u32 i = 0; i < render_queue_size; i++)
    {
        if (render_queue[i].visible && !render_queue[i].rendered)
            render_comp(i);
    }
}

void LCD_RendererRemoveById(u32 id, bool redraw_lower_layers)
{
    if (id >= render_queue_size)
        return;

    // id is the index of the component in the render queue
    render_queue[id].visible = false;
    remove_comp(id, redraw_lower_layers);

    for (u32 i = id; i < render_queue_size - 1; i++)
        render_queue[i] = render_queue[i + 1];
    render_queue_size--;
}

void LCD_SetComponentVisibility(u32 id, bool visible, bool redraw_lower_layers)
{
    if (id >= render_queue_size)
        return;

    render_queue[id].visible = visible;
    if (visible)
        render_comp(id);
    else
        remove_comp(id, redraw_lower_layers);
}

void LCD_RendererRemoveAll(void)
{
    for (u32 i = 0; i < render_queue_size; i++)
        render_queue[i] = (LCD_RenderQueueItem){0, NULL, false, false};

    render_queue_size = 0;
    LCD_SetBackgroundColor(current_bg_color);
}

/*
LCD_Dimension LCD_GetStringDimension(const char *const str, u8 font)
{
    if (!str)
        return (LCD_Dimension){0, 0}; // Return (0, 0) if the string is null

    u16 cur_line_width = 0, max_width = 0;
    u16 total_height = 16; // Minimum height 1 line

    const u8 *copy = (u8 *)str;
    while (*copy++)
    {
        cur_line_width += 8;         // If you're using a fixed-width font (each char is 8px wide)
        if (cur_line_width >= MAX_X) // When line width exceeds the screen width, it wraps
        {
            total_height += 16; // Add height for the new line
            cur_line_width = 0; // Reset line width for the new line
        }

        // Update max width based on current line
        if (cur_line_width > max_width)
            max_width = cur_line_width;
    }

    return (LCD_Dimension){max_width, total_height};
}
*/