#ifndef __LED_TYPES_H
#define __LED_TYPES_H

typedef enum
{
    LED0 = 0x01,
    LED1 = 0x02,
    LED2 = 0x04,
    LED3 = 0x08,
    LED4 = 0x10,
    LED5 = 0x20,
    LED6 = 0x40,
    LED7 = 0x80,
	
		LED_ALL = 0xFF
} LEDS;

#endif