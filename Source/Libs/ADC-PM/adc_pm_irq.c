#include "adc_pm.h"
#include <LPC17xx.h>
#include <stdlib.h>

/// @brief Holds the handler that is executed when the conversion is done.
_PRIVATE ADC_PMInterruptHandler done_action = NULL;

/// @brief Used to store the previous value of the conversion.
_PRIVATE u16 prv_value = 0x0;

// PUBLIC FUNCTIONS

void ADC_PMSetSampleReadyAction(ADC_PMInterruptHandler action)
{
    done_action = action;
}

void ADC_PMUnsetSampleReadyAction(void)
{
    done_action = NULL;
}

// INTERRUPT HANDLER

extern void ADC_IRQHandler(void)
{
    // The interrupt reads info from the ADGDR register.
    const u16 new_value = (LPC_ADC->ADGDR >> 4) & 0xFFF;

    if (done_action)
        done_action(prv_value, new_value);

    if (new_value != prv_value)
        prv_value = new_value;
}