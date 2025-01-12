#include "prng.h"

_PRIVATE u32 prng_state;

// PRIVATE FUNCTIONS

/// @brief Combines the two entropy sources retrieved
///        from the ADC and GPIO into a 32-bit value.
_PRIVATE inline u32 get_hw_entropy(void)
{
    return 1;
}

// PUBLIC FUNCTIONS

void PRNG_Seed(u32 seed)
{
    if (seed == USE_AUTO_SEED)
    {
        // Using SYSTICK set at infinite counting as
        // entropy generator.



        prng_state = get_hw_entropy();
        return;
    }

    prng_state = seed;
}

void PRNG_Deinit(void)
{
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
