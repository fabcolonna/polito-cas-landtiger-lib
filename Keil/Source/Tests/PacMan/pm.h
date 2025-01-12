#ifndef __PACMAN_H
#define __PACMAN_H

#include "pm.h"
#include "pm_types.h"

#include <stdbool.h>

bool PACMAN_Init(void);

void PACMAN_GameLoop(PM_GameSpeed speed);

#endif