#include "rit.h"
#include "LPC17xx.h"
#include "utils.h"
#include <stdbool.h>

/// @brief Flag to indicate if the debouncer is currently active.
_USED_EXTERNALLY bool debouncer_on;

/// @brief PCLK frequency in MHz. Can be checked in the RIT GUI in Keil
_PRIVATE const u32 pclk_mhz = 100;

void RIT_Init(u32 ival_ms, u16 int_priority)
{
    CLR_BITS(LPC_SC->PCLKSEL1, 3, 26); // Clear
    SET_BIT(LPC_SC->PCLKSEL1, 26);     // Set PCLK_RIT to CCLK
    SET_BIT(LPC_SC->PCONP, 16);        // Enable power to RIT

    LPC_RIT->RICOMPVAL = (pclk_mhz * ival_ms * 1000);
    LPC_RIT->RICTRL = 6; // Clear on match + Timer enable for debug
    LPC_RIT->RICOUNTER = 0;

    // Enabling interrupts coming from RIT
    NVIC_EnableIRQ(RIT_IRQn);

    if (!IS_DEF_PRIORITY(int_priority) && IS_BETWEEN_EQ(int_priority, 0, 15))
        NVIC_SetPriority(RIT_IRQn, int_priority);

    debouncer_on = true;
}

// USED IN BUTTON

void RIT_Enable(void)
{
    SET_BIT(LPC_RIT->RICTRL, 3);
}

void RIT_Disable(void)
{
    CLR_BIT(LPC_RIT->RICTRL, 3);
}