#include "LPC17xx.h"
#include "button_types.h"
#include "debouncer.h"

#include <stdbool.h>

/// @brief Array of function pointers to the interrupt handlers for each BUTTON peripheral.
///        This is declared in button_irq.c and referenced here since the same handlers will
///        be used by the debouncing code as well. Hence, the array is marked _DECL_EXTERNALLY.
_DECL_EXTERNALLY BUTTON_Interrupt_Handler BUTTON_Handlers[];

/// @brief Flag to indicate if the button is currently pressed. Used for debouncing.
_USED_EXTERNALLY u8 eint0_down = 0, eint1_down = 0, eint2_down = 0;

/// @brief Flag to indicate if the debouncer is currently active. Set in debouncer.c.
_DECL_EXTERNALLY bool debouncer_on;

// PRIVATE FUNCTIONS

_PRIVATE inline void reset_deb(void)
{
    LPC_RIT->RICOUNTER = 0;
}

// INTERRUPT HANDLER

_INT_HANDLER RIT_IRQHandler(void)
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

    reset_deb();
    SET_BIT(LPC_RIT->RICTRL, 0); // Clear interrupt flag
}
