#ifndef __PACMAN_TYPES_H
#define __PACMAN_TYPES_H

#include "peripherals.h"

typedef enum
{
    PM_ERR_NONE = 0,
    PM_ERR_UNINIT_LCD,
    PM_ERR_UNINIT_TP
} PM_Error;

typedef struct
{
    u16 score, record;
    u16 game_over_in;
} PM_GameStats;

typedef enum
{
    PM_MOVE_NONE,
    PM_MOV_UP,
    PM_MOV_DOWN,
    PM_MOV_LEFT,
    PM_MOV_RIGHT,
} PM_MovementDirection;

typedef enum
{
    PM_SPEED_FAST = 1,
    PM_SPEED_NORMAL = 2,
    PM_SPEED_SLOW = 3,
} PM_GameSpeed;

#endif