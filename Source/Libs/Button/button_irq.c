#include "LPC17xx.h"
#include "button.h"

#include <stdlib.h>

/// @brief Array of function pointers to the interrupt handlers for each BUTTON peripheral.
///        This is defined regardless of the debouncing option, since the handlers will be
///        used by the debouncing code as well. Hence the array is marked _USED_EXTERNALLY.
/// @note This array is updated by BUTTON_SetInterruptHandler() and BUTTON_UnsetInterruptHandler().
_USED_EXTERNALLY BUTTON_Interrupt_Handler BUTTON_Handlers[BTN_INT_SRC_COUNT] = {NULL};

/// @brief Flag to indicate if the button is currently pressed. Used for debouncing.
_DECL_EXTERNALLY u8 eint0_down, eint1_down, eint2_down;

/// @brief Flag to indicate if the debouncer is currently active. Set in debouncer.c.
_DECL_EXTERNALLY bool debouncer_on;

// PUBLIC FUNCTIONS

void BUTTON_SetInterruptHandler(BUTTON_Interrupt_Source source, BUTTON_Interrupt_Handler handler)
{
    BUTTON_Handlers[source] = handler;
}

void BUTTON_UnsetInterruptHandler(BUTTON_Interrupt_Source source)
{
    BUTTON_Handlers[source] = NULL;
}

// INTERRUPT HANDLERS

/// @note 3rd button on the board (next to RESET)
_INT_HANDLER EINT0_IRQHandler(void)
{
    if (debouncer_on)
    {
        eint0_down = 1;
        NVIC_DisableIRQ(EINT0_IRQn);
        CLR_BIT(LPC_PINCON->PINSEL4, 20); // Set P2.10 to 00 (GPIO, previously EINT0)
    }
    else
    {
        if (BUTTON_Handlers[BTN_INT_SRC_EINT0])
            BUTTON_Handlers[BTN_INT_SRC_EINT0]();
    }

    LPC_SC->EXTINT &= 1; // Clear the interrupt flag
}

/// @note Left-most button on the board
_INT_HANDLER EINT1_IRQHandler(void)
{
    if (debouncer_on)
    {
        eint1_down = 1;
        NVIC_DisableIRQ(EINT1_IRQn);
        CLR_BIT(LPC_PINCON->PINSEL4, 22); // Set P2.11 to 00 (GPIO, previously EINT1)
    }
    else
    {
        if (BUTTON_Handlers[BTN_INT_SRC_EINT1])
            BUTTON_Handlers[BTN_INT_SRC_EINT1]();
    }

    LPC_SC->EXTINT &= 1 << 1; // Clear the interrupt flag
}

/// @note 2nd button from the left on the board
_INT_HANDLER EINT2_IRQHandler(void)
{
    if (debouncer_on)
    {
        eint2_down = 1;
        NVIC_DisableIRQ(EINT2_IRQn);
        CLR_BIT(LPC_PINCON->PINSEL4, 24); // Set P2.12 to 00 (GPIO, previously EINT2)
    }
    else
    {
        if (BUTTON_Handlers[BTN_INT_SRC_EINT2])
            BUTTON_Handlers[BTN_INT_SRC_EINT2]();
    }

    LPC_SC->EXTINT &= 1 << 2; // Clear the interrupt flag
}
