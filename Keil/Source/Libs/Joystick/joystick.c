#include "joystick.h"
#include "rit.h"
#include <LPC17xx.h>

#include <stdlib.h>

typedef struct
{
    JOYSTICK_Function function;
    bool enabled;
} FunctionWrapper;

/// @brief Array of function pointers containing the hanlder for each joystick function.
_PRIVATE FunctionWrapper functions[JOY_ACTION_COUNT] = {0};

// RIT JOB

_PRIVATE void handle_polling(void)
{
    if ((LPC_GPIO1->FIOPIN & (1 << 29)) == 0 && functions[JOY_ACTION_UP].function && functions[JOY_ACTION_UP].enabled)
        functions[JOY_ACTION_UP].function();

    if ((LPC_GPIO1->FIOPIN & (1 << 25)) == 0 && functions[JOY_ACTION_SEL].function && functions[JOY_ACTION_SEL].enabled)
        functions[JOY_ACTION_SEL].function();

    if ((LPC_GPIO1->FIOPIN & (1 << 26)) == 0 && functions[JOY_ACTION_DOWN].function &&
        functions[JOY_ACTION_DOWN].enabled)
        functions[JOY_ACTION_DOWN].function();

    if ((LPC_GPIO1->FIOPIN & (1 << 27)) == 0 && functions[JOY_ACTION_LEFT].function &&
        functions[JOY_ACTION_LEFT].enabled)
        functions[JOY_ACTION_LEFT].function();

    if ((LPC_GPIO1->FIOPIN & (1 << 28)) == 0 && functions[JOY_ACTION_RIGHT].function &&
        functions[JOY_ACTION_RIGHT].enabled)
        functions[JOY_ACTION_RIGHT].function();
}

// PUBLIC FUNCTIONS

JOYSTICK_Error JOYSTICK_Init(void)
{
    // Joystick PINS are 1.25 to 1.29. Need to set them as GPIO
    // These go from bit 18 to 27 in the PINSEL3 register.
    LPC_PINCON->PINSEL3 &= ~(0x3FF << 18);

    // Setting Pin 25-29 as inputs (0) in the FIODIR for GPIO1
    LPC_GPIO1->FIODIR &= ~(0x1F << 25);

    if (!RIT_IsEnabled())
        return JOY_ERR_RIT_UNINIT;

    RIT_AddJob(handle_polling, RIT_NO_DIVIDER);
    RIT_EnableJob(handle_polling);
    return JOY_ERR_OK;
}

void JOYSTICK_Deinit(void)
{
    RIT_RemoveJob(handle_polling);
}

// ACTIONS

void JOYSTICK_EnableAction(JOYSTICK_Action action)
{
    if ((u8)action == JOY_ACTION_ALL)
    {
        for (u8 i = 0; i < JOY_ACTION_COUNT; i++)
        {
            if (!functions[i].function || functions[i].enabled)
                continue;

            functions[i].enabled = true;
        }

        return;
    }

    if (!functions[action].function)
        return; // No function associated.

    functions[action].enabled = true;
}

void JOYSTICK_DisableAction(JOYSTICK_Action action)
{
    if ((u8)action == JOY_ACTION_ALL)
    {
        for (u8 i = 0; i < JOY_ACTION_COUNT; i++)
        {
            if (!functions[i].function || !functions[i].enabled)
                continue;

            functions[i].enabled = false;
        }

        return;
    }

    if (!functions[action].function)
        return; // No function associated.

    functions[action].enabled = false;
}

void JOYSTICK_SetFunction(JOYSTICK_Action action, JOYSTICK_Function function)
{
    functions[action] = (FunctionWrapper){
        .function = function,
        .enabled = false,
    };
}

void JOYSTICK_UnsetFunction(JOYSTICK_Action action)
{
    functions[action] = (FunctionWrapper){
        .function = NULL,
        .enabled = false,
    };
}