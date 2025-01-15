#include "button.h"
#include "rit.h"

#include <LPC17xx.h>
#include <stdbool.h>

/// @brief Flag to indicate if the debouncer is currently active. Used in button_irq.c
_USED_EXTERNALLY bool debouncer_on = false;

/// @brief RIT job for handling debouncing. Defined & used in button_irq.c
_DECL_EXTERNALLY void handle_debouncing(void);

// PUBLIC FUNCTIONS

BUTTON_Error BUTTON_Init(u8 options)
{
    // Enabling EINT[0..2] in PINSEL4. In the table, these PINS behave
    // like EINTs when set to 01.
    LPC_PINCON->PINSEL4 |= (0b010101 << 20);

    // EITs mapped to PINs 2.[10..12]. Need to set them to 0, i.e. input.
    LPC_GPIO2->FIODIR &= ~(0b111 << 10);

    // Setting how EINTs are raised using EXTMODE in SC. Setting 1 to the
    // bottom 3 bits to set interrupt edge sensitive. Since we're not modifying
    // EXTPOLAR in SC, ints are raised on FALLING EDGE.
    LPC_SC->EXTMODE = 0x7;
    LPC_SC->EXTPOLAR = 0x0;
    
    if (options & BTN_DEBOUNCE_WITH_RIT)
    {
        if (!RIT_IsEnabled())
            return BTN_ERR_RIT_UNINIT;

        debouncer_on = true;
        RIT_AddJob(handle_debouncing, RIT_NO_DIVIDER);
        RIT_EnableJob(handle_debouncing);
    }

    return BTN_ERR_OK;
}

void BUTTON_Deinit(void)
{
    // Disabling EINT[0..2] in PINSEL4. In the table, these PINS behave
    // like EINTs when set to 01.
    LPC_PINCON->PINSEL4 &= ~(0b010101 << 20);
    LPC_SC->EXTMODE = 0x0;
    LPC_SC->EXTPOLAR = 0x0;

    NVIC_DisableIRQ(EINT0_IRQn);
    NVIC_DisableIRQ(EINT1_IRQn);
    NVIC_DisableIRQ(EINT2_IRQn);

    if (debouncer_on)
    {
        debouncer_on = false;
        RIT_RemoveJob(handle_debouncing);
    }
}