#ifndef __PRNG_H
#define __PRNG_H

#include "utils.h"

// PUBLIC TYPES

/// @brief PRNG automatically seeds itself using an hardware entropy source.
#define USE_AUTO_SEED 0

// PUBLIC FUNCTIONS

/// @brief Seeds the PRNG with the given seed. Use USE_AUTO_SEED to seed it
///        automatically using an HW entropy source.
/// @param seed The seed to use.
void PRNG_Seed(u32 seed);

/// @brief Releases the HW entropy sources used to seed the PRNG.
void PRNG_Deinit(void);

/// @brief Generates the next pseudo-random number.
/// @return The PRN
u32 PRNG_Next(void);

/// @brief Generates a pseudo-random number in the given range, inclusive.
/// @param min Lower bound
/// @param max Upper bound
/// @return The PRN
u32 PRNG_Range(u32 min, u32 max);

#endif