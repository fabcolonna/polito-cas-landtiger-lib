#ifndef __PACMAN_H
#define __PACMAN_H

#include "pm.h"
#include "pm_types.h"

#include <stdbool.h>

bool PACMAN_Init(PM_GameSpeed speed);

void PACMAN_GameLoop(void);

#endif