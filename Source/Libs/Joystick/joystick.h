#ifndef __JOYSTICK_H
#define __JOYSTICK_H

#include "joystick_types.h"
#include "utils.h"

/// @brief Initializes the joystick peripheral with the given options.
/// @param config Joystick options from the JOYSTICK_Config enum
void JOYSTICK_Init(u8 config);

void JOYSTICK_Deinit(void);

void JOYSTICK_SetFunction(JOYSTICK_Action action, JOYSTICK_Function function);
void JOYSTICK_UnsetFunction(JOYSTICK_Action action);

#endif