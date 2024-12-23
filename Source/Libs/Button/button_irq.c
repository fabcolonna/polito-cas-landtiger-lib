#include "button.h"
#include "rit.h"

#include <LPC17xx.h>
#include <stdbool.h>
#include <stdlib.h>

/// @brief Array of function pointers to the interrupt handlers for each BUTTON peripheral.
///        This is defined regardless of the debouncing option, since the handlers will be
///        used by the debouncing code as well. Hence the array is marked _USED_EXTERNALLY.
/// @note This array is updated by BUTTON_SetInterruptHandler() and BUTTON_UnsetInterruptHandler().
_PRIVATE BUTTON_InterruptHandler handlers[BTN_INT_SRC_COUNT] = {NULL};

/// @brief Flag to indicate if the button is currently pressed. Used for debouncing.
_PRIVATE u8 eint0_down, eint1_down, eint2_down;

/// @brief Flag to indicate if the debouncer is currently active
_DECL_EXTERNALLY bool debouncer_on;

/// @brief RIT job for managing debouncing
void handle_debouncing(void)
{
    if (eint0_down)
    {
        eint0_down++;

        // If P2.10 is low, button is pressed
        if ((LPC_GPIO2->FIOPIN & (1 << 10)) == 0)
        {
            if (eint0_down == 2 && handlers[BTN_INT_SRC_EINT0])
                handlers[BTN_INT_SRC_EINT0]();
        }
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
        if ((LPC_GPIO2->FIOPIN & (1 << 11)) == 0)
        {
            if (eint1_down == 2 && handlers[BTN_INT_SRC_EINT1])
                handlers[BTN_INT_SRC_EINT1]();
        }
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
        if ((LPC_GPIO2->FIOPIN & (1 << 12)) == 0)
        {
            if (eint2_down == 2 && handlers[BTN_INT_SRC_EINT2])
                handlers[BTN_INT_SRC_EINT2]();
        }
        else
        {
            eint2_down = 0;
            NVIC_EnableIRQ(EINT2_IRQn);       // Enabling the interrupt again
            SET_BIT(LPC_PINCON->PINSEL4, 24); // Set P2.12 back to 01 (EINT2)
        }
    }
}

// PUBLIC FUNCTIONS

void BUTTON_SetInterruptHandler(BUTTON_InterruptSource source, BUTTON_InterruptHandler handler)
{
    handlers[source] = handler;
}

void BUTTON_UnsetInterruptHandler(BUTTON_InterruptSource source)
{
    handlers[source] = NULL;
}

// INTERRUPT HANDLERS

/// @note 3rd button on the board (next to RESET)
extern void EINT0_IRQHandler(void)
{
    if (debouncer_on)
    {
        eint0_down = 1;
        NVIC_DisableIRQ(EINT0_IRQn);
        CLR_BIT(LPC_PINCON->PINSEL4, 20); // Set P2.10 to 00 (GPIO, previously EINT0)
    }
    else
    {
        if (handlers[BTN_INT_SRC_EINT0])
            handlers[BTN_INT_SRC_EINT0]();
    }

    LPC_SC->EXTINT &= 1; // Clear the interrupt flag
}

/// @note Left-most button on the board
extern void EINT1_IRQHandler(void)
{
    if (debouncer_on)
    {
        eint1_down = 1;
        NVIC_DisableIRQ(EINT1_IRQn);
        CLR_BIT(LPC_PINCON->PINSEL4, 22); // Set P2.11 to 00 (GPIO, previously EINT1)
    }
    else
    {
        if (handlers[BTN_INT_SRC_EINT1])
            handlers[BTN_INT_SRC_EINT1]();
    }

    LPC_SC->EXTINT &= 1 << 1; // Clear the interrupt flag
}

/// @note 2nd button from the left on the board
extern void EINT2_IRQHandler(void)
{
    if (debouncer_on)
    {
        eint2_down = 1;
        NVIC_DisableIRQ(EINT2_IRQn);
        CLR_BIT(LPC_PINCON->PINSEL4, 24); // Set P2.12 to 00 (GPIO, previously EINT2)
    }
    else
    {
        if (handlers[BTN_INT_SRC_EINT2])
            handlers[BTN_INT_SRC_EINT2]();
    }

    LPC_SC->EXTINT &= 1 << 2; // Clear the interrupt flag
}