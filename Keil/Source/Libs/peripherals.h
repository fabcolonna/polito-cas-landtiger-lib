#ifndef __INCLUDES_H
#define __INCLUDES_H

// IMPLEMENTATION DEFINES: GLCD

/// @brief This macro controls the timing between the low level operations with
///        the GLCD controller. Since they may introduce overhead, they're turned off. If you
///        have rendering problems, reintroduce this define by uncommenting it. This also
///        stands true for the GLCD_INLINE_AMAP
// #define GLCD_REQUIRES_DELAY
// #define GLCD_INLINE_AMAP

/// @brief Maximum number of components that can be rendered on the screen.
/// @note RQ only referneces objects stored in the memory arena, hence it's memory
///       footprint in the ZI section is not huge. We can afford to have a larger number.
#define MAX_RQ_ITEMS 512

/// @brief Setting a limit to the number of components that a given object can have.
#define MAX_COMPS_PER_OBJECT 12

/// @brief Maximum number of fonts that can be loaded into the LCD.
#define MAX_FONTS 16

// IMPLEMENTATION DEFINES: RIT

/// @brief This macro controls how many jobs the RIT library is able to handle. Edit this
///        according to your needs and memory limitations.
#define RIT_JOBS_COUNT 20

// INCLUDES

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "adc_pm.h"
#include "button.h"
#include "dac.h"
#include "glcd.h"
#include "joystick.h"
#include "led.h"
#include "power.h"
#include "prng.h"
#include "rit.h"
#include "timer.h"
#include "touch.h"

#endif