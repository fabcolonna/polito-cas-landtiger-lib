#include "led.h"
#include <LPC17xx.h>

void LED_Init(void)
{
    LPC_PINCON->PINSEL4 &= 0xFFFF0000; // P2.0 to P2.7 set to function 00 (GPIO)
    LPC_GPIO2->FIODIR |= 0xFF;         // P2.0 to P2.7 set as output
    LED_Off(0xFF);                     // Turn off all LEDs
}

void LED_Deinit(void)
{
    LPC_GPIO2->FIODIR &= ~0xFF; // i.e. 0xFFFFFF00, top 24 unchanged, bottom 8 = 0
}

void LED_On(u8 which)
{
    LPC_GPIO2->FIOSET = which;
}

void LED_Off(u8 which)
{
    LPC_GPIO2->FIOCLR = which;
}

void LED_Output(u8 value)
{
    LED_Off(LED_ALL);
    LED_On(value);
}

void LED_Clear(void)
{
    LPC_GPIO2->FIOCLR = LED_ALL;
}