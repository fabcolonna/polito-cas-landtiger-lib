#include "buttons.h"
#include "rit.h"

#include <LPC17xx.h>
#include <stdbool.h>
#include <stdlib.h>

// PRIVATE TYPES

typedef struct
{
    BUTTON_Function handler;
    bool source_enabled;
} InterruptHandlerWrapper;

/// @brief Array of function pointers to the interrupt handlers for each BUTTON peripheral.
///        This is defined regardless of the debouncing option, since the handlers will be
///        used by the debouncing code as well. Hence the array is marked _USED_EXTERNALLY.
/// @note This array is updated by BUTTON_SetInterruptHandler() and BUTTON_UnsetInterruptHandler().
_PRIVATE InterruptHandlerWrapper handlers[BTN_SRC_COUNT] = {NULL};

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
            if (eint0_down == 2 && handlers[BTN_SRC_EINT0].handler)
                handlers[BTN_SRC_EINT0].handler();
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
            if (eint1_down == 2 && handlers[BTN_SRC_EINT1].handler)
                handlers[BTN_SRC_EINT1].handler();
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
            if (eint2_down == 2 && handlers[BTN_SRC_EINT2].handler)
                handlers[BTN_SRC_EINT2].handler();
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

BUTTON_Error BUTTON_EnableSource(BUTTON_Source source, u8 int_priority)
{
    if (!handlers[source].handler)
        return BTN_ERR_NO_HANDLER_TO_ENABLE; // No handler associated

    handlers[source].source_enabled = true;

    switch (source)
    {
    case BTN_SRC_EINT0:
        NVIC_EnableIRQ(EINT0_IRQn);
        if (!IS_DEF_PRIORITY(int_priority) && IS_BETWEEN_EQ(int_priority, 0, 15))
            NVIC_SetPriority(EINT0_IRQn, int_priority);
        else
            return BTN_ERR_INT_PRIO_INVALID;
        break;
    case BTN_SRC_EINT1:
        NVIC_EnableIRQ(EINT1_IRQn);
        if (!IS_DEF_PRIORITY(int_priority) && IS_BETWEEN_EQ(int_priority, 0, 15))
            NVIC_SetPriority(EINT1_IRQn, int_priority);
        else
            return BTN_ERR_INT_PRIO_INVALID;
        break;
    case BTN_SRC_EINT2:
        NVIC_EnableIRQ(EINT2_IRQn);
        if (!IS_DEF_PRIORITY(int_priority) && IS_BETWEEN_EQ(int_priority, 0, 15))
            NVIC_SetPriority(EINT2_IRQn, int_priority);
        else
            return BTN_ERR_INT_PRIO_INVALID;
        break;
    default:
        return BTN_ERR_NO_HANDLER_TO_ENABLE;
    }

    return BTN_ERR_OK;
}

void BUTTON_DisableSource(BUTTON_Source source)
{
    if (!handlers[source].handler)
        return; // No handler associated

    handlers[source].source_enabled = false;
}

void BUTTON_SetFunction(BUTTON_Source source, BUTTON_Function func)
{
    handlers[source] = (InterruptHandlerWrapper){
        .handler = func,
        .source_enabled = false,
    };
}

void BUTTON_UnsetFunction(BUTTON_Source source)
{
    handlers[source] = (InterruptHandlerWrapper){
        .handler = NULL,
        .source_enabled = false,
    };
}

// INTERRUPT HANDLERS

/// @note 3rd button on the board (next to RESET)
extern void EINT0_IRQHandler(void)
{
    if (!handlers[BTN_SRC_EINT0].source_enabled || !handlers[BTN_SRC_EINT0].handler)
        return;

    if (debouncer_on)
    {
        eint0_down = 1;
        NVIC_DisableIRQ(EINT0_IRQn);
        CLR_BIT(LPC_PINCON->PINSEL4, 20); // Set P2.10 to 00 (GPIO, previously EINT0)
    }
    else
        handlers[BTN_SRC_EINT0].handler();

    LPC_SC->EXTINT &= 1; // Clear the interrupt flag
}

/// @note Left-most button on the board
extern void EINT1_IRQHandler(void)
{
    if (!handlers[BTN_SRC_EINT1].source_enabled || !handlers[BTN_SRC_EINT1].handler)
        return;

    if (debouncer_on)
    {
        eint1_down = 1;
        NVIC_DisableIRQ(EINT1_IRQn);
        CLR_BIT(LPC_PINCON->PINSEL4, 22); // Set P2.11 to 00 (GPIO, previously EINT1)
    }
    else
        handlers[BTN_SRC_EINT1].handler();

    LPC_SC->EXTINT &= 1 << 1; // Clear the interrupt flag
}

/// @note 2nd button from the left on the board
extern void EINT2_IRQHandler(void)
{
    if (!handlers[BTN_SRC_EINT2].source_enabled || !handlers[BTN_SRC_EINT1].handler)
        return;

    if (debouncer_on)
    {
        eint2_down = 1;
        NVIC_DisableIRQ(EINT2_IRQn);
        CLR_BIT(LPC_PINCON->PINSEL4, 24); // Set P2.12 to 00 (GPIO, previously EINT2)
    }
    else
        handlers[BTN_SRC_EINT2].handler();

    LPC_SC->EXTINT &= 1 << 2; // Clear the interrupt flag
}