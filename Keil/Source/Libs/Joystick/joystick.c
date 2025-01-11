#include "joystick.h"
#include "rit.h"
#include <LPC17xx.h>

#include <stdlib.h>

/// @brief Array of function pointers containing the hanlder for each joystick function.
_PRIVATE JOYSTICK_Function functions[JOY_ACTION_COUNT] = {NULL};

// RIT JOB

_PRIVATE void handle_polling(void)
{
    if ((LPC_GPIO1->FIOPIN & (1 << 29)) == 0 && functions[JOY_ACTION_UP])
        functions[JOY_ACTION_UP]();

    if ((LPC_GPIO1->FIOPIN & (1 << 25)) == 0 && functions[JOY_ACTION_SEL])
        functions[JOY_ACTION_SEL]();

    if ((LPC_GPIO1->FIOPIN & (1 << 26)) == 0 && functions[JOY_ACTION_DOWN])
        functions[JOY_ACTION_DOWN]();

    if ((LPC_GPIO1->FIOPIN & (1 << 27)) == 0 && functions[JOY_ACTION_LEFT])
        functions[JOY_ACTION_LEFT]();

    if ((LPC_GPIO1->FIOPIN & (1 << 28)) == 0 && functions[JOY_ACTION_RIGHT])
        functions[JOY_ACTION_RIGHT]();
}

// PUBLIC FUNCTIONS

void JOYSTICK_Init(u8 config)
{
    // Joystick PINS are 1.25 to 1.29. Need to set them as GPIO
    // These go from bit 18 to 27 in the PINSEL3 register.
    LPC_PINCON->PINSEL3 &= ~(0x3FF << 18);

    // Setting Pin 25-29 as inputs (0) in the FIODIR for GPIO1
    LPC_GPIO1->FIODIR &= ~(0x1F << 25);

    if (config & JOY_POLL_WITH_RIT && RIT_IsEnabled())
        RIT_AddJob(handle_polling, RIT_NO_DIVIDER);
}

void JOYSTICK_Deinit(void)
{
    RIT_RemoveJob(handle_polling);
}

void JOYSTICK_SetFunction(JOYSTICK_Action action, JOYSTICK_Function function)
{
    functions[action] = function;
}

void JOYSTICK_UnsetFunction(JOYSTICK_Action action)
{
    functions[action] = NULL;
}