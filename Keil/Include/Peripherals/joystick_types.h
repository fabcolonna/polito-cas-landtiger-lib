#ifndef __JOYSTICK_TYPES_H
#define __JOYSTICK_TYPES_H

#include "types.h"

typedef enum
{
    /// @brief No error.
    JOY_ERR_OK = 0,
    /// @brief During initialization, POLL_WITH_RIT was requested
    ///        but the RIT was not enabled.
    JOY_ERR_RIT_UNINIT,
} JOYSTICK_Error;

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

#define JOY_ACTION_ALL (u8)(-1)

#endif