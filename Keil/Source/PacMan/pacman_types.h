#ifndef __PACMAN_TYPES_H
#define __PACMAN_TYPES_H

#include "cl_list.h"
#include "glcd_types.h"
#include "types.h"

#include <stdbool.h>

// MAZE TYPES & VARIABLES

#define PM_MAZE_SCALED_WIDTH 23
#define PM_MAZE_SCALED_HEIGHT 25
#define PM_MAZE_CELL_SIZE 10

#define PM_MAZE_PIXEL_WIDTH (PM_MAZE_SCALED_WIDTH * PM_MAZE_CELL_SIZE)
#define PM_MAZE_PIXEL_HEIGHT (PM_MAZE_SCALED_HEIGHT * PM_MAZE_CELL_SIZE)

#define PM_WALL_COLOR 0x25283

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

#define PM_GHOST_SCARED_DURATION 10

typedef enum
{
    PM_WALL, // Maze walls
    PM_NONE, // Empty cell, can be traversed if not sorrounded by walls
    PM_PILL, // Normal pill cell
    PM_SUPER_PILL,
    PM_PCMN, // PacMan
    PM_LTPL, // Left portal
    PM_RTPL, // Right portal
    PM_GHSR, // Ghost
} PM_MazeObj;

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
    CL_List *path;
    bool is_valid;
} PM_GhostPath;

typedef struct
{
    PM_MazeCell cell;
    LCD_ObjID id;
    bool is_scared;
    u8 scared_counter;
    PM_GhostPath path;
} PM_Ghost;

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
    PM_SPEED_FAST = 0,
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
    // GENERIC
    u16 prev_record;
    bool playing_now;
    LCD_Coordinate maze_pos;
    // PACMAN
    PM_PacMan pacman;
    // PILLS & GHOST
    LCD_ObjID pill_ids[PM_MAZE_SCALED_HEIGHT][PM_MAZE_SCALED_WIDTH];
    PM_SuperPill super_pills[PM_SUP_PILL_COUNT];
    PM_Ghost ghost;
    // GAME STATS
    PM_GameStatValues stat_values;
    PM_GameStatIDs stat_obj_ids;
    PM_GameStatStrings stat_strings;
} PM_Game;

#endif