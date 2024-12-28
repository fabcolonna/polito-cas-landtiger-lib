#include "touch.h"
#include "glcd.h"
#include "power.h"

#include <LPC17xx.h>
#include <stdbool.h>
#include <stdlib.h>

#define TP_CS(a) ((a) ? (LPC_GPIO0->FIOSET = (1 << 6)) : (LPC_GPIO0->FIOCLR = (1 << 6)))
#define TP_INT_IN (LPC_GPIO2->FIOPIN & (1 << 13))

// Bit-frequency = PCLK / CPSR
#define SPI_SPEED_4MHz 18    /* 4MHz */
#define SPI_SPEED_2MHz 36    /* 2MHz */
#define SPI_SPEED_1MHz 72    /* 1MHz */
#define SPI_SPEED_500kHz 144 /* 500kHz */
#define SPI_SPEED_400kHz 180 /* 400kHz */

#define THRESHOLD 2

// Bits in the SSP Status Register for checking the status of the SSP interface.
// Refer to User Manual @ Table 373 for more info
#define SSP_SR_RECV_FIFO_NOT_EMPTY 2 // 0 if the receive FIFO is empty, 1 if not
#define SSP_SR_BUSY 4                // 0 if SSP is idle, 1 if it's sending/receiving data and/or SEND FIFO is not empty

// Waits till the SSP interface is not busy anymore (& is 0)
#define WAIT_IF_BUSY while (LPC_SSP1->SR & (1 << SSP_SR_BUSY))

// Touch Panel registers for X, Y coordinates
#define CHX 0x90
#define CHY 0xd0

// Math functions
#define ABS(x) ((x) < 0 ? -(x) : (x))

// TYPES

typedef struct
{
    long double a_n, b_n, c_n, d_n, e_n, f_n, divider;
} TP_CalibrationMatrix;

// STATE VARIABLES

/// @brief Current calibration matrix.
_PRIVATE TP_CalibrationMatrix current_calib_matrix;

// DELAY PRIVATE FUNCTIONS

/// @brief Delays for the specified number of microseconds.
_PRIVATE void delay_us(u32 count)
{
    for (u32 i = 0; i < count; i++)
    {
        u8 us = 12;
        while (us--)
            ;
    }
}

// SPI PRIVATE FUNCTIONS

/// @brief Sets SPI speed to the specified value.
_PRIVATE inline void spi_set_speed(u8 speed)
{
    speed &= 0xFE; // Make sure that speed is even.
    if (speed < 2)
        speed = 2;

    // Setting clock prescale register to the desired speed.
    LPC_SSP1->CPSR = speed;
}

/// @brief Writes a command to the ADS7843 trough SSP1 and returns the received byte.
/// @param cmd The command to write to the ADS7843.
/// @return The byte received from the ADS7843.
_PRIVATE inline u8 spi_send_command(u8 cmd)
{
    WAIT_IF_BUSY;
    LPC_SSP1->DR = cmd; // Write command to the SSP1 data register (R/W)
    WAIT_IF_BUSY;

    // Controller should now respond with a byte. It will put it in the RX FIFO.
    while (!(LPC_SSP1->SR & (1 << SSP_SR_RECV_FIFO_NOT_EMPTY)))
        ; // Wait until RX FIFO is not empty

    // If here, FIFO contains something, reading it
    const u8 byte_returned = LPC_SSP1->DR;
    return byte_returned;
}

/// @brief Initializes the SPI interface for ADS7843.
_PRIVATE inline void spi_init(void)
{
    // Enabling power to SSP1 interface (Synchronous Serial Port), which provides SPI capability.
    POWER_TurnOnPeripheral(POW_PCSSP1);

    // P0.7 SCK, P0.8 MISO, P0.9 MOSI are SSP pins.
    LPC_PINCON->PINSEL0 &= ~((3UL << 14) | (3UL << 16) | (3UL << 18)); // P0.7, P0.8, P0.9 cleared
    LPC_PINCON->PINSEL0 |= (2UL << 14) | (2UL << 16) | (2UL << 18);    // P0.7 SCK1, P0.8 MISO1, P0.9 MOSI1

    // Setting clock speed for SSP1. 01 = CCLK
    LPC_SC->PCLKSEL0 &= ~(3 << 20); /* PCLKSP0 = CCLK/4 (18MHz) */
    LPC_SC->PCLKSEL0 |= (1 << 20);  /* PCLKSP0 = CCLK   (72MHz) */

    // Register Table: User Manual @ Table 370/371
    // CPOL = Clock Polarity / CPHA = Clock Phase
    LPC_SSP1->CR0 = 0x0007; // 8Bit, CPOL=0, CPHA=0 (Control Register 0, clock rate, data size...)
    LPC_SSP1->CR1 = 0x0002; // SSP1 enable, master (Control Register 1, master/slave...)

    // SPI communication speed needs to be known and fixed.
    spi_set_speed(SPI_SPEED_500kHz);

    // Wait, maybe touch panel is busy
    WAIT_IF_BUSY;

    // Consume eventual data in RX FIFO, so that we can start fresh.
    volatile uint32_t dummy;
    while (LPC_SSP1->SR & (1 << SSP_SR_RECV_FIFO_NOT_EMPTY))
        dummy = LPC_SSP1->DR;
}

// TOUCH CONTROLLER PRIVATE FUNCTIONS

/// @brief Communicate (via SPI) with ADS7843 and read a 12-bit ADC (Analog to Digital Converter) value.
/// @note This function implements the protocol specification that the device uses.
_PRIVATE u16 ads7843_read_adc(void)
{
    u16 buf, tmp;

    tmp = spi_send_command(0x00);
    buf = tmp << 8; // Read first 8 bits of the ADC result

    delay_us(1); // Wait for the next byte to be ready

    tmp = spi_send_command(0x00);
    buf |= tmp; // Read the last 8 bytes of the ADC result

    // ADC result is a 12-bit value transmitted as a 16 bit with 4 bits of padding.
    // Need to discard those extra LSBs and return the 12-bit value (& 0xFFF).
    return (buf >> 4) & 0xFFF;
}

/// @brief Reads the X coordinate from the touch panel.
/// @return The 12-bit ADC value of the X coordinate.
_PRIVATE u16 ads7843_read_x(void)
{
    TP_CS(0);
    delay_us(1);
    spi_send_command(CHX);
    delay_us(1);

    const u16 adc = ads7843_read_adc();
    TP_CS(1);
    return adc;
}

/// @brief Reads the Y coordinate from the touch panel.
/// @return The 12-bit ADC value of the Y coordinate.
_PRIVATE u16 ads7843_read_y(void)
{
    TP_CS(0);
    delay_us(1);
    spi_send_command(CHY);
    delay_us(1);

    const u16 adc = ads7843_read_adc();
    TP_CS(1);
    return adc;
}

// PRIVATE TOUCH FUNCTIONS

/// @brief Returns the X, Y coordinates of the touched point.
_PRIVATE void get_xy(u16 *out_x, u16 *out_y)
{
    if (!out_x || !out_y)
        return;

    const u16 adx = ads7843_read_x();
    delay_us(1);
    const u16 ady = ads7843_read_y();
    *out_x = adx;
    *out_y = ady;
}

_PRIVATE bool poll_touch(TP_Coordinate *out_tp_coords)
{
    if (!out_tp_coords)
        return false;

    u16 tp_x, tp_y;
    u16 buffer[2][9] = {{0}, {0}};

    u8 count = 0;
    while (!TP_INT_IN && count < 9)
    {
        get_xy(&tp_x, &tp_y);
        buffer[0][count] = tp_x;
        buffer[1][count] = tp_y;
        count++;
    }

    if (count != 9)
        return false;

    int m0, m1, m2, temp[3];
    temp[0] = (buffer[0][0] + buffer[0][1] + buffer[0][2]) / 3;
    temp[1] = (buffer[0][3] + buffer[0][4] + buffer[0][5]) / 3;
    temp[2] = (buffer[0][6] + buffer[0][7] + buffer[0][8]) / 3;
    m0 = ABS(temp[0] - temp[1]);
    m1 = ABS(temp[1] - temp[2]);
    m2 = ABS(temp[2] - temp[0]);
    if (m0 > THRESHOLD && m1 > THRESHOLD && m2 > THRESHOLD)
        return false;

    if (m0 < m1)
    {
        if (m2 < m0)
            out_tp_coords->x = (temp[0] + temp[2]) / 2;
        else
            out_tp_coords->x = (temp[0] + temp[1]) / 2;
    }
    else if (m2 < m1)
        out_tp_coords->x = (temp[0] + temp[2]) / 2;
    else
        out_tp_coords->x = (temp[1] + temp[2]) / 2;

    temp[0] = (buffer[1][0] + buffer[1][1] + buffer[1][2]) / 3;
    temp[1] = (buffer[1][3] + buffer[1][4] + buffer[1][5]) / 3;
    temp[2] = (buffer[1][6] + buffer[1][7] + buffer[1][8]) / 3;

    m0 = ABS(temp[0] - temp[1]);
    m1 = ABS(temp[1] - temp[2]);
    m2 = ABS(temp[2] - temp[0]);
    if (m0 > THRESHOLD && m1 > THRESHOLD && m2 > THRESHOLD)
        return false;

    if (m0 < m1)
    {
        if (m2 < m0)
            out_tp_coords->y = (temp[0] + temp[2]) / 2;
        else
            out_tp_coords->y = (temp[0] + temp[1]) / 2;
    }
    else if (m2 < m1)
        out_tp_coords->y = (temp[0] + temp[2]) / 2;
    else
        out_tp_coords->y = (temp[1] + temp[2]) / 2;

    return true;
}

// PRIVATE CALIBRATION FUNCTIONS

_PRIVATE inline void draw_line_primitive(u16 from_x, u16 from_y, u16 to_x, u16 to_y, u16 color)
{
    LCD_DrawLine((LCD_Coordinate){from_x, from_y}, (LCD_Coordinate){to_x, to_y}, color);
}

_PRIVATE inline void draw_calibration_cross(u16 x, u16 y)
{
    LCD_DrawRectangle((LCD_Coordinate){x - 15, y - 15}, (LCD_Coordinate){x + 15, y + 15}, COL_WHITE, NO_FILL_COLOR);
    draw_line_primitive(x, y - 7, x, y + 7, COL_WHITE);
    draw_line_primitive(x - 7, y, x + 7, y, COL_WHITE);
}

_PRIVATE inline void delete_calibration_cross(u16 x, u16 y)
{
    LCD_DrawRectangle((LCD_Coordinate){x - 15, y - 15}, (LCD_Coordinate){x + 15, y + 15}, COL_BLACK, NO_FILL_COLOR);
    draw_line_primitive(x, y - 7, x, y + 7, COL_BLACK);
    draw_line_primitive(x - 7, y, x + 7, y, COL_BLACK);
}

_PRIVATE bool calc_calibration_matrix(TP_CalibrationMatrix *out_matrix, const LCD_Coordinate *const lcd_3points,
                                      const TP_Coordinate *const tp_3points)
{
    if (!lcd_3points || !tp_3points || !out_matrix)
        return false;

    long double divider = ((tp_3points[0].x - tp_3points[2].x) * (tp_3points[1].y - tp_3points[2].y)) -
                          ((tp_3points[1].x - tp_3points[2].x) * (tp_3points[0].y - tp_3points[2].y));
    if (divider == 0)
        return false;

    out_matrix->divider = divider;
    out_matrix->a_n = ((lcd_3points[0].x - lcd_3points[2].x) * (tp_3points[1].y - tp_3points[2].y)) -
                      ((lcd_3points[1].x - lcd_3points[2].x) * (tp_3points[0].y - tp_3points[2].y));
    out_matrix->b_n = ((tp_3points[0].x - tp_3points[2].x) * (lcd_3points[1].x - lcd_3points[2].x)) -
                      ((lcd_3points[0].x - lcd_3points[2].x) * (tp_3points[1].x - tp_3points[2].x));
    out_matrix->c_n = (tp_3points[2].x * lcd_3points[1].x - tp_3points[1].x * lcd_3points[2].x) * tp_3points[0].y +
                      (tp_3points[0].x * lcd_3points[2].x - tp_3points[2].x * lcd_3points[0].x) * tp_3points[1].y +
                      (tp_3points[1].x * lcd_3points[0].x - tp_3points[0].x * lcd_3points[1].x) * tp_3points[2].y;
    out_matrix->d_n = ((lcd_3points[0].y - lcd_3points[2].y) * (tp_3points[1].y - tp_3points[2].y)) -
                      ((lcd_3points[1].y - lcd_3points[2].y) * (tp_3points[0].y - tp_3points[2].y));
    out_matrix->e_n = ((tp_3points[0].x - tp_3points[2].x) * (lcd_3points[1].y - lcd_3points[2].y)) -
                      ((lcd_3points[0].y - lcd_3points[2].y) * (tp_3points[1].x - tp_3points[2].x));
    out_matrix->f_n = (tp_3points[2].x * lcd_3points[1].y - tp_3points[1].x * lcd_3points[2].y) * tp_3points[0].y +
                      (tp_3points[0].x * lcd_3points[2].y - tp_3points[2].x * lcd_3points[0].y) * tp_3points[1].y +
                      (tp_3points[1].x * lcd_3points[0].y - tp_3points[0].x * lcd_3points[1].y) * tp_3points[2].y;

    return true;
}

// Calibration should not be an operation that the user should do,
// hence it's been made private and called from the init function.
bool calibrate(void)
{
    const LCD_Coordinate lcd_crosses[3] = {{45, 45}, {45, 270}, {190, 190}};
    TP_Coordinate tp_crosses[3] = {{0}, {0}, {0}};

    LCD_ClearWith(COL_BLACK);

    char *calibration_text = "Touch crosshair to calibrate";
    LCD_PrintString(calibration_text, COL_BLACK, LCD_FONT_SYSTEM, COL_WHITE, (LCD_Coordinate){10, 10});

    for (u8 i = 0; i < 3; i++)
    {
#ifdef SIMULATOR
        delay_us(1000 * 50);
#else
        delay_us(1000 * 500);
#endif

        draw_calibration_cross(lcd_crosses[i].x, lcd_crosses[i].y);
        const TP_Coordinate *calib_coords = TP_WaitForTouch();
        tp_crosses[i].x = calib_coords->x;
        tp_crosses[i].y = calib_coords->y;
        delete_calibration_cross(lcd_crosses[i].x, lcd_crosses[i].y);
    }

    if (!calc_calibration_matrix(&current_calib_matrix, lcd_crosses, tp_crosses))
        return false;

    LCD_DeleteString(calibration_text, LCD_FONT_SYSTEM, COL_BLACK, (LCD_Coordinate){10, 10});
    LCD_PrintString("Touch is ready!", COL_BLACK, LCD_FONT_SYSTEM, COL_WHITE, LCD_POS_MIDDLE_CENTER(0, 0, 110, 10));
    TP_WaitForTouch();
    LCD_ClearWith(COL_BLACK);
    return true;
}

// PUBLIC FUNCTIONS

void TP_Init(void)
{
    LPC_GPIO0->FIODIR |= (1 << 6);  // P0.6 CS is output
    LPC_GPIO2->FIODIR |= (0 << 13); // P2.13 TP_INT is input
    TP_CS(1);

    spi_init(); // Initialize SPI interface for touch panel
    while (!calibrate())
        ;
}

const TP_Coordinate *TP_WaitForTouch(void)
{
    static TP_Coordinate tp_coords;
    while (!poll_touch(&tp_coords))
        ;
    return &tp_coords;
}

const LCD_Coordinate *TP_GetLCDCoordinateFor(TP_Coordinate *const tp_point)
{
    static LCD_Coordinate lcd_point;

    TP_CalibrationMatrix matrix = current_calib_matrix;
    if (matrix.divider == 0 || !tp_point)
        return NULL;

    lcd_point.x = ((matrix.a_n * tp_point->x) + (matrix.b_n * tp_point->y) + matrix.c_n) / matrix.divider;
    lcd_point.y = ((matrix.d_n * tp_point->x) + (matrix.e_n * tp_point->y) + matrix.f_n) / matrix.divider;

    return &lcd_point;
}