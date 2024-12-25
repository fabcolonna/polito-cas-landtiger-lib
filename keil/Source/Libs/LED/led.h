#ifndef __LED_H
#define __LED_H

#include "utils.h"
#include "led_types.h"
#include <stdint.h>

#define LED_NUM 8

void LED_Init(void);
void LED_Deinit(void);

/// @brief Turns on the specified LEDs
/// @param leds Bitmask of LEDs to turn on [LED(0..7)]
void LED_On(u8 leds);

/// @brief Turns off the specified LEDs
/// @param leds Bitmask of LEDs to turn off [LED(0..7)]
void LED_Off(u8 leds);

/// @brief Displays the value using LEDs
/// @param value The 8 bit value to display
void LED_Output(u8 value);

/// @brief Clears the LEDs
void LED_Clear();

#endif