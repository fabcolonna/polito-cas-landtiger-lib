#ifndef __JOYSTICK_H
#define __JOYSTICK_H

#include "joystick_types.h"
#include "types.h"

/// @brief Initializes the joystick peripheral with the given options.
/// @param options Joystick options from the JOYSTICK_Config enum
JOYSTICK_Error JOYSTICK_Init(void);

/// @brief Deinitializes the joystick, by removing all functions associated
///        with the actions, and removing the polling job from the RIT.
void JOYSTICK_Deinit(void);

/// @brief  Enables the specified action, thus enabling the binding with
///         the associated functionality, if set.
/// @param action The action to enable.
void JOYSTICK_EnableAction(JOYSTICK_Action action);

/// @brief Disables the specific action, and disables the binding with
///        its functionality, without un-binding it completely.
/// @param action The action to disable.
void JOYSTICK_DisableAction(JOYSTICK_Action action);

/// @brief Sets a functionality for the given action.
/// @param action The action associated to the functionality
/// @param function The functionality.
void JOYSTICK_SetFunction(JOYSTICK_Action action, JOYSTICK_Function function);

/// @brief Unsets a functionality for the given action, if previously set.
/// @param action The action to un-bind.
void JOYSTICK_UnsetFunction(JOYSTICK_Action action);

#endif