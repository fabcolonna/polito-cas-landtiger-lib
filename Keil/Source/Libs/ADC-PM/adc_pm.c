#include "adc_pm.h"
#include "power.h"
#include "rit.h"

#include <LPC17xx.h>

// Since channels are not handled, types are not needed
#define ADC_CH5 (1 << 5)
#define ADC_GLOBAL_INTEN (1 << 8)

_PRIVATE bool initialized = false;

void ADC_PMInit(u8 options, u8 clock_divider, u8 int_priority)
{
    POWER_TurnOnPeripheral(POW_PCADC);

    SET_BITS(LPC_PINCON->PINSEL3, 0x3, 30);                     // 11 -> ADC function for AD0.5
    LPC_ADC->ADCR = (1 << 21) | (clock_divider << 8) | ADC_CH5; // Enable ADC, set clock divider, and set channel 5

    LPC_ADC->ADINTEN = ADC_GLOBAL_INTEN;
    NVIC_EnableIRQ(ADC_IRQn);

    if (!IS_DEF_PRIORITY(int_priority) && IS_BETWEEN_EQ(int_priority, 0, 15))
        NVIC_SetPriority(ADC_IRQn, int_priority);

    if (options & ADC_PM_SAMPLE_WITH_RIT && RIT_IsEnabled())
        RIT_AddJob(ADC_PMGetSample, RIT_NO_DIVIDER);

    initialized = true;
}

bool ADC_PMIsInitialized(void)
{
    return initialized;
}

void ADC_PMDeinit(void)
{
    POWER_TurnOffPeripheral(POW_PCADC);
    RIT_RemoveJob(ADC_PMGetSample); // Won't do nothing if absent

    NVIC_DisableIRQ(ADC_IRQn);
    LPC_ADC->ADCR = 0;
    LPC_ADC->ADINTEN = 0;
    initialized = false;
}

void ADC_PMGetSample(void)
{
    SET_BIT(LPC_ADC->ADCR, 24); // Start conversion
}

void ADC_PMSDisableSampling(void)
{
    CLR_BIT(LPC_ADC->ADCR, 24); // Stop conversion
}