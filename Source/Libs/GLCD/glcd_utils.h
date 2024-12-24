#ifndef __GLCD_UTILS_H
#define __GLCD_UTILS_H

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

#endif