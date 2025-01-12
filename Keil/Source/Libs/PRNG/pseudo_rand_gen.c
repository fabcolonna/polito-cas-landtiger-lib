#include "pseudo_rand_gen.h"
#include "peripherals.h"

#include <LPC17xx.h>

// PRIVATE VARIABLES

_PRIVATE u32 prng_state;

// PRIVATE FUNCTIONS

_PRIVATE inline u8 rd_adc_noise(void)
{
    LPC_ADC->ADCR |= (1 << 24); // Start conversion
    while (!(LPC_ADC->ADSTAT & (1 << 5)))
        ;

    return (u8)(((LPC_ADC->ADDR5 >> 4) & 0xFFF) & 0x7);
}

_PRIVATE inline u8 rd_gpio_lcd_noise(void)
{
    return (u8)(LPC_GPIO2->FIOPIN & 0x3);
}

/// @brief Combines the two entropy sources retrieved
///        from the ADC and GPIO into a 32-bit value.
_PRIVATE inline u32 get_hw_entropy(void)
{
    u32 entropy = 0;
    for (int i = 0; i < 32; i += 2)
    {
        entropy ^= (rd_adc_noise() << i);            // Combine ADC noise
        entropy ^= (rd_gpio_lcd_noise() << (i + 1)); // Combine GPIO noise
    }
    return entropy;
}

// PUBLIC FUNCTIONS

void PRNG_Seed(u32 seed)
{
    if (seed == USE_AUTO_SEED && !ADC_PMIsInitialized())
    {
        // Initialize ADC for channel 5 (PM)
        POWER_TurnOnPeripheral(POW_PCADC);
        LPC_PINCON->PINSEL0 |= (1 << 8); // Set P0.4 to ADC mode (ADC channel 5)
        LPC_ADC->ADCR = (1 << 5) |       // Select ADC channel 5
                        (4 << 8) |       // ADC clock = PCLK/5
                        (1 << 21);       // Enable the ADC

        prng_state = get_hw_entropy();
        return;
    }

    prng_state = seed;
}

void PRNG_Deinit(void)
{
    if (ADC_PMIsInitialized())
        return; // Maybe it's being used, not turning off

    ADC_PMDeinit();
}

u32 PRNG_Next(void)
{
    prng_state = (1664525 * prng_state + 1013904223) % 0xFFFFFFFF; // LCG algorithm
    return prng_state;
}

u32 PRNG_Range(u32 min, u32 max)
{
    if (min >= max)
        return (u32)-1;

    return (PRNG_Next() % (max - min + 1)) + min;
}
