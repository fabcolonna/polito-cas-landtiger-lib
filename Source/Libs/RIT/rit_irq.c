#include "LPC17xx.h"
#include "adc_pm.h"
#include "button_types.h"
#include "joystick_types.h"
#include "rit.h"

#include <stdbool.h>

// PRIVATE STUFF FOR BUTTONS

/// @brief Array of function pointers to the interrupt handlers for each BUTTON peripheral.
///        This is declared in button_irq.c and referenced here since the same handlers will
///        be used by the debouncing code as well. Hence, the array is marked _DECL_EXTERNALLY.
_DECL_EXTERNALLY BUTTON_Interrupt_Handler BUTTON_Handlers[];

/// @brief Flag to indicate if the button is currently pressed. Used for debouncing.
_USED_EXTERNALLY u8 eint0_down = 0, eint1_down = 0, eint2_down = 0;

/// @brief Flag to indicate if the debouncer is currently active. Set in debouncer.c.
_DECL_EXTERNALLY bool debouncer_on;

// PRIVATE STUFF FOR JOYSTICK

_DECL_EXTERNALLY JOYSTICK_Function JOYSTICK_Functions[];

// Checking each GPIO PIN corresponding to the joystick functionalities
// static u8 joy_up = 0, joy_down = 0, joy_left = 0, joy_right = 0, joy_sel = 0;

// PRIVATE FUNCTIONS

_PRIVATE inline void handle_button_debouncing(void)
{
    if (!debouncer_on)
        return;

    if (eint0_down)
    {
        eint0_down++;

        // If P2.10 is low, button is pressed
        if ((LPC_GPIO2->FIOPIN & (1 << 10)) == 0)
        {
            if (eint0_down == 2 && BUTTON_Handlers[BTN_INT_SRC_EINT0])
                BUTTON_Handlers[BTN_INT_SRC_EINT0]();
        }
        // Button released
        else
        {
            eint0_down = 0;
            NVIC_EnableIRQ(EINT0_IRQn);       // Enabling the interrupt again
            SET_BIT(LPC_PINCON->PINSEL4, 20); // Set P2.10 back to 01 (EINT0)
        }
    }

    if (eint1_down)
    {
        eint1_down++;

        // If P2.10 is low, button is pressed
        if ((LPC_GPIO2->FIOPIN & (1 << 11)) == 0)
        {
            if (eint1_down == 2 && BUTTON_Handlers[BTN_INT_SRC_EINT1])
                BUTTON_Handlers[BTN_INT_SRC_EINT1]();
        }
        // Button released
        else
        {
            eint1_down = 0;
            NVIC_EnableIRQ(EINT1_IRQn);       // Enabling the interrupt again
            SET_BIT(LPC_PINCON->PINSEL4, 22); // Set P2.11 back to 01 (EINT1)
        }
    }

    if (eint2_down)
    {
        eint2_down++;

        // If P2.10 is low, button is pressed
        if ((LPC_GPIO2->FIOPIN & (1 << 12)) == 0)
        {
            if (eint2_down == 2 && BUTTON_Handlers[BTN_INT_SRC_EINT2])
                BUTTON_Handlers[BTN_INT_SRC_EINT2]();
        }
        // Button released
        else
        {
            eint2_down = 0;
            NVIC_EnableIRQ(EINT2_IRQn);       // Enabling the interrupt again
            SET_BIT(LPC_PINCON->PINSEL4, 24); // Set P2.12 back to 01 (EINT2)
        }
    }
}

_PRIVATE inline void handle_joystick_controls(void)
{
    if ((LPC_GPIO1->FIOPIN & (1 << 29)) == 0 && JOYSTICK_Functions[JOY_ACTION_UP])
        JOYSTICK_Functions[JOY_ACTION_UP]();

    if ((LPC_GPIO1->FIOPIN & (1 << 25)) == 0 && JOYSTICK_Functions[JOY_ACTION_SEL])
        JOYSTICK_Functions[JOY_ACTION_SEL]();

    if ((LPC_GPIO1->FIOPIN & (1 << 26)) == 0 && JOYSTICK_Functions[JOY_ACTION_DOWN])
        JOYSTICK_Functions[JOY_ACTION_DOWN]();

    if ((LPC_GPIO1->FIOPIN & (1 << 27)) == 0 && JOYSTICK_Functions[JOY_ACTION_LEFT])
        JOYSTICK_Functions[JOY_ACTION_LEFT]();

    if ((LPC_GPIO1->FIOPIN & (1 << 28)) == 0 && JOYSTICK_Functions[JOY_ACTION_RIGHT])
        JOYSTICK_Functions[JOY_ACTION_RIGHT]();
}

// INTERRUPT HANDLER

_INT_HANDLER RIT_IRQHandler(void)
{
    handle_joystick_controls();
    handle_button_debouncing();

    // Getting samples from PM every <RIT ival>
    ADC_PMGetSample();

    LPC_RIT->RICOUNTER = 0;      // Reset counter
    SET_BIT(LPC_RIT->RICTRL, 0); // Clear interrupt flag
}
