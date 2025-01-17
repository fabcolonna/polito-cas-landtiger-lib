#ifndef __PRNG_H
#define __PRNG_H

#include "utils.h"

// PUBLIC TYPES

typedef enum
{
    /// @brief PRNG automatically seeds itself using an hardware entropy source.
    PRNG_USE_AUTO_SEED,
} PRNG_Config;

// PUBLIC FUNCTIONS

/// @brief Initializes the PRNG by seeding it with the given seed. Use 
///        PRNG_USE_AUTO_SEED to seed it automatically using an HW entropy source.
/// @param seed The seed to use.
void PRNG_Set(u32 seed);

/// @brief Releases the HW entropy sources used to seed the PRNG.
void PRNG_Release(void);

/// @brief Generates the next pseudo-random number.
/// @return The PRN
u32 PRNG_Next(void);

/// @brief Generates a pseudo-random number in the given range, inclusive.
/// @param min Lower bound
/// @param max Upper bound
/// @return The PRN
u32 PRNG_Range(u32 min, u32 max);

#endif