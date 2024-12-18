#ifndef __JOYSTICK_H
#define __JOYSTICK_H

#include "joystick_types.h"
#include "utils.h"

void JOYSTICK_Init(void);

void JOYSTICK_SetFunction(JOYSTICK_Action action, JOYSTICK_Function function);

void JOYSTICK_ClearFunction(JOYSTICK_Action action);

#endif