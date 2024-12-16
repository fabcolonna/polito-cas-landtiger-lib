#include "debouncer.h"
#include "LPC17xx.h"
#include "utils.h"
#include <stdbool.h>

_USED_EXTERNALLY bool debouncer_on;

void DEB_Init(u32 ival_ms, u32 pclk_mhz, u16 int_priority)
{
    CLR_BITS(LPC_SC->PCLKSEL1, 3, 26);
    SET_BIT(LPC_SC->PCLKSEL1, 26);
    SET_BIT(LPC_SC->PCONP, 16);

    LPC_RIT->RICOMPVAL = (pclk_mhz * ival_ms * 1000);
    LPC_RIT->RICTRL = 6; // Clear on match + Timer enable for debug
    LPC_RIT->RICOUNTER = 0;

    // Enabling interrupts coming from RIT
    NVIC_EnableIRQ(RIT_IRQn);

    if (!IS_DEF_PRIORITY(int_priority) && IS_BETWEEN_EQ(int_priority, 0, 15))
        NVIC_SetPriority(RIT_IRQn, int_priority);

    debouncer_on = false;
}

void DEB_Enable(void)
{
    SET_BIT(LPC_RIT->RICTRL, 3);
    debouncer_on = true;
}

void DEB_Disable(void)
{
    CLR_BIT(LPC_RIT->RICTRL, 3);
    debouncer_on = false;
}

void DEB_Reset(void)
{
    LPC_RIT->RICOUNTER = 0;
}