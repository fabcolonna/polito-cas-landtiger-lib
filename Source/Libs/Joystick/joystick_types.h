#ifndef __JOYSTICK_TYPES_H
#define __JOYSTICK_TYPES_H

#include "utils.h"

typedef enum
{
    JOY_POLL_WITH_RIT = 0x1
} JOYSTICK_Config;

/// @brief Joystick function handler function pointer
typedef void (*JOYSTICK_Function)(void);

// Each function is mapped to the correspoding GPIO pin, so
// check can be simplified.
typedef enum
{
    JOY_ACTION_SEL = 0, // GPIO 1.25
    JOY_ACTION_DOWN,
    JOY_ACTION_LEFT,
    JOY_ACTION_RIGHT,
    JOY_ACTION_UP, // GPIO 1.29
    JOY_ACTION_COUNT
} JOYSTICK_Action;

#endif