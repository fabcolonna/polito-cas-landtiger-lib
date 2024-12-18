#include "joystick.h"
#include "LPC17xx.h"

#include <stdlib.h>

/// @brief Array of function pointers containing the hanlder for each joystick function.
_USED_EXTERNALLY JOYSTICK_Function JOYSTICK_Functions[JOY_ACTION_COUNT] = {NULL};

void JOYSTICK_Init(void)
{
    // Joystick PINS are 1.25 to 1.29. Need to set them as GPIO
    // These go from bit 18 to 27 in the PINSEL3 register.
    LPC_PINCON->PINSEL3 &= ~(0x3FF << 18);

    // Setting Pin 25-29 as inputs (0) in the FIODIR for GPIO1
    LPC_GPIO1->FIODIR &= ~(0x1F << 25);
}

void JOYSTICK_SetFunction(JOYSTICK_Action action, JOYSTICK_Function function)
{
    JOYSTICK_Functions[action] = function;
}

void JOYSTICK_ClearFunction(JOYSTICK_Action action)
{
    JOYSTICK_Functions[action] = NULL;
}