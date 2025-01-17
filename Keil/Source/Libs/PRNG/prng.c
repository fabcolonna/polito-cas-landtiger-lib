#include "prng.h"
#include "LPC17xx.h" // For accessing SysTick and system peripherals

// Static variables to hold the current seed and state
_PRIVATE u32 prng_state = 0;

void PRNG_Set(u32 seed)
{
    if (seed == (u32)PRNG_USE_AUTO_SEED)
    {
        // Starting SysTick if it's not already running
        if (!(SysTick->CTRL & SysTick_CTRL_ENABLE_Msk))
        {
            SysTick->LOAD = 0xFFFFFF; // Set the maximum value for a 24-bit counter
            SysTick->VAL = 0;         // Reset the counter
            SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk; // Enable the counter
        }

        // Use SysTick counter as a hardware entropy source for automatic seeding
        prng_state = SysTick->VAL ^ (SysTick->VAL << 16) ^ (SysTick->LOAD);
        return;
    }

    // Use the provided seed
    prng_state = seed;
}

/// @brief Releases the HW entropy sources used to seed the PRNG.
void PRNG_Release(void)
{
    // Stopping SysTick
    SysTick->CTRL = 0;
    prng_state = 0;
}

/// @brief Generates the next pseudo-random number.
/// @return The PRN
u32 PRNG_Next(void)
{
    // A simple linear congruential generator (LCG) for pseudo-random numbers
    prng_state = (1103515245 * prng_state + 12345) & 0x7FFFFFFF;
    return prng_state;
}

/// @brief Generates a pseudo-random number in the given range, inclusive.
/// @param min Lower bound
/// @param max Upper bound
/// @return The PRN
u32 PRNG_Range(u32 min, u32 max)
{
    if (min > max)
    {
        // Swap min and max if inputs are invalid
        u32 temp = min;
        min = max;
        max = temp;
    }

    u32 range = max - min + 1;
    return (PRNG_Next() % range) + min;
}
