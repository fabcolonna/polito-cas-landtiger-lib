#ifndef __PACMAN_TYPES_H
#define __PACMAN_TYPES_H

#include "utils.h"

typedef struct
{
    u16 score, record;
    u16 game_over_in, lives;
} PM_GameStats;

typedef enum
{
    PM_MOV_NONE,
    PM_MOV_UP,
    PM_MOV_DOWN,
    PM_MOV_LEFT,
    PM_MOV_RIGHT,
} PM_MovementDir;

typedef enum
{
    PM_SPEED_FAST = 1,
    PM_SPEED_NORMAL,
    PM_SPEED_SLOW
} PM_GameSpeed;

#endif