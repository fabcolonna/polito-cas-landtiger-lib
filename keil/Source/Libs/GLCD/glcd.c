#include "glcd.h"
#include <LPC17xx.h>
#include <math.h>
#include <stdbool.h>

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

// PRIVATE WAIT FUNCTIONS

_PRIVATE void delay_count(u32 count)
{
    while (count--)
        ;
}

_PRIVATE void delay_ms(u16 ms)
{
    for (u16 i = 0; i < ms; i++)
        for (u16 j = 0; j < 1141; j++)
            ;
}

// PRIVATE LOW LEVEL STUFF

/// @brief Code for the current LCD peripheral.
_PRIVATE u8 LCD_Code;

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
    delay_count(30);                           /* delay some times */
    value = LPC_GPIO2->FIOPIN0;                /* Read D8..D15 */
    LCD_EN(1);                                 /* Enable 1B->1A */
    delay_count(30);                           /* delay some times */
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
    delay_count(22);
    LCD_WR(0);
    delay_count(1);
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
    delay_count(1);
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

// PRIVATE HIGHER LEVEL FUNCTIONS

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
#if (DISP_ORIENTATION == 90) || (DISP_ORIENTATION == 270)
    // Swap X and Y if orientation is 90 or 270 degrees
    u16 tmp = x;
    x = y;
    y = (MAX_X - 1) - tmp;
#endif

    switch (LCD_Code)
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

// COLOR CONVERSIONS

/// @brief Converts from BGR565 (Blue-Green-Red) to RGB565.
/// @param bgr_color BGR565 color
/// @return Associated RGB565 color
/// @note Just swap the B (top 5 bits) & R (bottom 5 bits) channels.
_PRIVATE u16 bgr_to_rgb(u16 bgr)
{
    // BGR = BBBBBGGGGGGRRRRR
    const u16 b = (bgr >> 11) & 0x1f;
    const u16 g = (bgr >> 5) & 0x3f;
    const u16 r = bgr & 0x1f;

    // RGB = RRRRRGGGGGGBBBBB
    return (r << 11) | (g << 5) | b;
}

// PUBLIC FUNCTIONS

void LCD_Init(void)
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

    delay_ms(100);

    const u16 code = read_from(0x0000);
    if (code == 0x9325 || code == 0x9328)
    {
        LCD_Code = ILI9325;
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
        delay_ms(50);
        write_to(0x0010, 0x1590);
        write_to(0x0011, 0x0227);
        delay_ms(50);
        write_to(0x0012, 0x009c);
        delay_ms(50);
        write_to(0x0013, 0x1900);
        write_to(0x0029, 0x0023);

        write_to(0x002b, 0x000e); // Frate rate & Color control
        delay_ms(50);
        write_to(0x0020, 0x0000); // GRAM horizontal Address
        write_to(0x0021, 0x0000); // GRAM Vertical Address

        delay_ms(50);
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
        delay_ms(50);
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

    delay_ms(50);
}

void LCD_ClearWith(LCD_RGBColor color)
{
    // Sets the GRAM cursor to the top-left corner of the display
    if (LCD_Code == HX8347D || LCD_Code == HX8347A)
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
        do_write(color);
}

LCD_RGBColor LCD_GetPoint(LCD_Coordinate point)
{
    LCD_RGBColor dummy;

    set_gram_cursor(point.x, point.y);
    init_rw_operation_at(0x0022); // Write GRAM

    switch (LCD_Code)
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
        return bgr_to_rgb(dummy);
    }
}

void LCD_SetPoint(LCD_RGBColor color, LCD_Coordinate where)
{
    if (where.x >= MAX_X || where.y >= MAX_Y)
        return;

    set_gram_cursor(where.x, where.y);
    write_to(0x0022, color);
}

// DRAW LINE STUFF

_PRIVATE inline void set_point_internal(LCD_RGBColor color, u16 x, u16 y)
{
    LCD_Coordinate c = {.x = x, .y = y};
    LCD_SetPoint(color, c);
}

void LCD_DrawLine(LCD_Coordinate from, LCD_Coordinate to, u16 color)
{
    // If the starting point is to the right of the ending point (both for x and/or y), swap.
    u16 tmp;
    if (from.x > to.x)
    {
        tmp = to.x;
        to.x = from.x;
        from.x = tmp;
    }
    if (from.y > to.y)
    {
        tmp = to.y;
        to.y = from.y;
        from.y = tmp;
    }

    u16 dx = to.x - from.x;
    u16 dy = to.y - from.y;
    if (dx == 0) // Vertical line
    {
        while (from.y <= to.y)
            set_point_internal(color, from.x, from.y++);
    }
    else if (dy == 0) // Horizontal line
    {
        while (from.x <= to.x)
            set_point_internal(color, from.x++, from.y);
    }
    else if (dx > dy)
    {
        tmp = 2 * dy - dx;
        while (from.x != to.x)
        {
            set_point_internal(color, from.x++, from.y);
            if (tmp > 0)
            {
                from.y++;
                tmp += 2 * dy - 2 * dx;
            }
            else
                tmp += 2 * dy;
        }
        set_point_internal(color, from.x, from.y);
    }
    else
    {
        tmp = 2 * dx - dy; /* ¿¿½üYÖá */
        while (from.y != to.y)
        {
            set_point_internal(color, from.x, from.y++);
            if (tmp > 0)
            {
                from.x++;
                tmp += 2 * dy - 2 * dx;
            }
            else
                tmp += 2 * dy;
        }
        set_point_internal(color, from.x, from.y);
    }
}

// SHAPES STUFF

void LCD_DrawRectangle(LCD_Coordinate from, LCD_Coordinate to, u16 edge_color, u16 fill_color)
{
    // If the starting point is to the right of the ending point (both for x and/or y), swap.
    u16 tmp;
    if (from.x > to.x)
    {
        tmp = to.x;
        to.x = from.x;
        from.x = tmp;
    }
    if (from.y > to.y)
    {
        tmp = to.y;
        to.y = from.y;
        from.y = tmp;
    }

    // Drawing the edges of the rectangle
    LCD_DrawLine(from, (LCD_Coordinate){to.x, from.y}, edge_color); // Top horizontal edge
    LCD_DrawLine(from, (LCD_Coordinate){from.x, to.y}, edge_color); // Left vertical edge
    LCD_DrawLine(to, (LCD_Coordinate){to.x, from.y}, edge_color);   // Right vertical edge
    LCD_DrawLine(to, (LCD_Coordinate){from.x, to.y}, edge_color);   // Bottom horizontal edge

    if (fill_color == NO_FILL_COLOR)
        return;

    // Filling the rectangle
    for (u16 i = from.x + 1; i < to.x; i++)
        for (u16 j = from.y + 1; j < to.y; j++)
            set_point_internal(fill_color, i, j);
}

_PRIVATE inline bool is_inside_circle(u16 x, u16 y, u16 cx, u16 cy, u16 r)
{
    return pow((x - cx), 2) + pow((y - cx), 2) <= pow(r, 2);
}

void LCD_DrawCircle(LCD_Coordinate center, u16 radius, u16 border_color, u16 fill_color)
{
    if (radius == 0)
        return;

    // Printing 4 cardinal points of the circle: note that if the radius is subtracted from y,
    // from the center, the pixel will be actually colored at the other side, because smaller
    // numbers mean higher positions in the screen.
    set_point_internal(border_color, center.x + radius, center.y); // Right side of the center
    set_point_internal(border_color, center.x - radius, center.y); // Left side
    set_point_internal(border_color, center.x, center.y + radius); // Top side, but since we +, it's actually the bottom
    set_point_internal(border_color, center.x, center.y - radius); // Bottom side, but since we -, it's actually the top

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
        set_point_internal(border_color, x + center.x, y + center.y);
        set_point_internal(border_color, -x + center.x, y + center.y);
        set_point_internal(border_color, x + center.x, -y + center.y);
        set_point_internal(border_color, -x + center.x, -y + center.y);
        set_point_internal(border_color, y + center.x, x + center.y);
        set_point_internal(border_color, -y + center.x, x + center.y);
        set_point_internal(border_color, y + center.x, -x + center.y);
        set_point_internal(border_color, -y + center.x, -x + center.y);
    }

    if (fill_color == NO_FILL_COLOR)
        return;

    // Filling the circle
    for (u16 i = center.x - radius + 1; i < center.x + radius; i++)
        for (u16 j = center.y - radius + 1; j < center.y + radius; j++)
            if (is_inside_circle(i, j, center.x, center.y, radius))
                set_point_internal(fill_color, i, j);
}

void LCD_DrawSprite(const u16 *const bitmap, LCD_Coordinate where)
{
    if (!bitmap)
        return;

    u16 x = where.x, y = where.y;
    for (u16 i = 0; i < MAX_X; i++)
        for (u16 j = 0; j < MAX_Y; j++)
            set_point_internal(bitmap[i + j * MAX_X], x + i, y + j);
}

// CHAR STUFF

_DECL_EXTERNALLY void get_ascii_for(u8 ascii, u8 font, u8 *const out);

void LCD_PutChar(u8 chr, u16 chr_color, u8 font, u16 bg_color, LCD_Coordinate where)
{
    u8 buffer[16], tmp_char;
    get_ascii_for(chr, font, buffer);
    for (u16 i = 0; i < 16; i++)
    {
        tmp_char = buffer[i];
        for (u16 j = 0; j < 8; j++)
        {
            if (((tmp_char >> (7 - j)) & 0x01))
                set_point_internal(chr_color, where.x + j, where.y + i);
            else
                set_point_internal(bg_color, where.x + j, where.y + i);
        }
    }
}

void LCD_PrintString(const char *const str, u16 str_color, u8 font, u16 bg_color, LCD_Coordinate where)
{
    if (!str)
        return;

    u8 tmp_char, *copy = (u8 *)str;
    while ((tmp_char = *copy++))
    {
        LCD_PutChar(tmp_char, str_color, font, bg_color, where);
        if (where.x < MAX_X - 8) // Continue on the same line
            where.x += 8;
        else if (where.y < MAX_Y - 16) // Go to the next line
        {
            where.x = 0;
            where.y += 16;
        }
        else // Wrap around to the top-left corner
        {
            where.x = 0;
            where.y = 0;
        }
    }
}