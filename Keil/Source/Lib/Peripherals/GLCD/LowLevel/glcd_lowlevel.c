#include "glcd_lowlevel.h"
#include "glcd_types.h"

#include <LPC17xx.h>

// PRIVATE DEFINES & TYPES

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

#ifdef GLCD_REQUIRES_DELAY
#define DELAY_COUNT(count) for (u16 i = count; i > 0; i--)

#define DELAY_MS(ms)                                                                                                   \
    for (u16 i = 0; i < ms; i++)                                                                                       \
        for (u16 j = 0; j < 1141; j++)
#else
#define DELAY_COUNT(count)
#define DELAY_MS(ms)
#endif

// PRIVATE TYPES

/// @brief Models.
enum LCDModel
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
};

// VARIABLES

/// @brief The model code of the LCD controller.
_PRIVATE u8 s_model_code;

/// @brief The current orientation of the screen.
_PRIVATE u16 s_orientation;

/// @brief The maximum X and Y coordinates of the screen.
_DECL_EXTERNALLY u16 LCDMaxX, LCDMaxY;

// PRIVATE FUNCTIONS

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
    if (s_orientation == LCD_ORIENT_HOR || s_orientation == LCD_ORIENT_HOR_INV)
    {
        // Swap X and Y if orientation is 90 or 270 degrees
        u16 tmp = x;
        x = y;
        y = (LCDMaxX - 1) - tmp;
    }

    switch (s_model_code)
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

// PUBLIC FUNCTIONS

void __LCD_LL_Init(u16 orientation)
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

    s_orientation = orientation;
    if (orientation == LCD_ORIENT_VER || orientation == LCD_ORIENT_VER_INV)
    {
        LCDMaxX = 240;
        LCDMaxY = 320;
    }
    else
    {
        LCDMaxX = 320;
        LCDMaxY = 240;
    }

    DELAY_MS(100);
    const u16 code = read_from(0x0000);
    if (code == 0x9325 || code == 0x9328)
    {
        s_model_code = ILI9325;
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
}

u16 __LCD_LL_GetPointColor(u16 x, u16 y)
{
    u16 dummy;

    set_gram_cursor(x, y);
    init_rw_operation_at(0x0022); // Write GRAM

    switch (s_model_code)
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
        return dummy;
    }
}

void __LCD_LL_SetPointColor(u16 rgb565, u16 x, u16 y)
{
    set_gram_cursor(x, y);
    write_to(0x0022, rgb565 & 0xFFFF);
}

void __LCD_LL_FillScreen(u16 color)
{
    // Sets the GRAM cursor to the top-left corner of the display
    if (s_model_code == HX8347D || s_model_code == HX8347A)
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
    for (u32 index = 0; index < LCDMaxX * LCDMaxY; index++)
        do_write(color & 0xFFFF);
}