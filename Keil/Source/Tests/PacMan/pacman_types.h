#ifndef __PACMAN_TYPES_H
#define __PACMAN_TYPES_H

#include "glcd_types.h"
#include "utils.h"
#include <stdbool.h>

// MAZE TYPES & VARIABLES

#define PM_PACMAN_RADIUS 4
#define PM_PACMAN_COLOR 0xFFD868

#define PM_STD_PILL_RADIUS 1
#define PM_STD_PILL_COLOR 0xE27417
#define PM_STD_PILL_COUNT 240
#define PM_STD_PILL_POINTS 10

#define PM_SUP_PILL_RADIUS 2
#define PM_SUP_PILL_COLOR 0xFDB897
#define PM_SUP_PILL_COUNT 6
#define PM_SUP_PILL_POINTS 50

#define MAX_LIVES 5

typedef struct
{
    u16 row, col;
} PM_MazeCell;

// GAME TYPES

typedef struct
{
    PM_MazeCell cell;
    LCD_ObjID id;

    /// @brief The second of the game between 5 and 59, at
    ///        which the power pill will spawn. This implements
    ///        the randomization of the power pill in time.
    u16 spawn_sec;
} PM_SuperPill;

typedef struct
{
    u16 score, record, lives;
    u16 game_over_in, pills_eaten;
} PM_GameStatValues;

typedef struct
{
    LCD_ObjID titles, lives[MAX_LIVES];
    LCD_ObjID score_record_values, game_over_in_value;
} PM_GameStatIDs;

typedef struct
{
    char record[10], score[10], game_over_in[10];
} PM_GameStatStrings;

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
} PM_Speed;

typedef struct
{
    LCD_ObjID id;
    PM_Speed speed;
    PM_MazeCell cell;
    PM_MovementDir dir;
} PM_PacMan;

typedef struct
{
    u16 prev_record;
    PM_PacMan pacman;
    PM_SuperPill super_pills[PM_SUP_PILL_COUNT];
    LCD_Coordinate maze_pos;
    bool playing_now;
    PM_GameStatValues stat_values;
    PM_GameStatIDs stat_obj_ids;
    PM_GameStatStrings stat_strings;
} PM_Game;

#endif