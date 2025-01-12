#ifndef __PACMAN_MAZE_H
#define __PACMAN_MAZE_H

#include "utils.h"

#define PM_MAZE_SCALED_WIDTH 23
#define PM_MAZE_SCALED_HEIGHT 25
#define PM_MAZE_CELL_SIZE 10

#define PM_PILL_COUNT 246

// TYPES

typedef struct
{
    u16 row, col;
} PM_MazeMapCell;

typedef enum
{
    PM_WALL, // Maze walls
    PM_NONE, // Empty cell, can be traversed if not sorrounded by walls
    PM_PILL, // Pill cell (both normal and super)
    PM_PCMN, // PacMan
    PM_LTPL, // Left portal
    PM_RTPL, // Right portal
    PM_GHS0, // Ghost 0
    PM_GHS1, // Ghost 1
    PM_GHS2, // Ghost 2
} PM_MazeObject;

#define PM_WALL_COL 0x25283
#define PM_PCMN_COL 0xFFD86B

#define PM_STD_PILL_COL 0xE27417
#define PM_SUP_PILL_COL 0xFDB897

// CONSTS

const u16 PM_MazeWidthPixels = PM_MAZE_SCALED_WIDTH * PM_MAZE_CELL_SIZE;
const u16 PM_MazeHeightPixels = PM_MAZE_SCALED_HEIGHT * PM_MAZE_CELL_SIZE;

// MAZE

// clang-format off

int PM_Maze[PM_MAZE_SCALED_HEIGHT][PM_MAZE_SCALED_WIDTH] = {
    {PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL},
    {PM_WALL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_WALL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_WALL},
    {PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL},
    {PM_WALL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_WALL},
    {PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL},
    {PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_PILL, PM_PILL, PM_PILL, PM_WALL, PM_PILL, PM_PILL, PM_PILL, PM_WALL, PM_PILL, PM_PILL, PM_PILL, PM_WALL, PM_PILL, PM_PILL, PM_PILL, PM_WALL, PM_WALL, PM_PILL, PM_WALL},
    {PM_WALL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_WALL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_WALL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_WALL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_WALL},
    {PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL},
    {PM_WALL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_WALL},
    {PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL},    
    {PM_NONE, PM_NONE, PM_NONE, PM_NONE, PM_NONE, PM_WALL, PM_PILL, PM_WALL, PM_NONE, PM_NONE, PM_NONE, PM_NONE, PM_NONE, PM_NONE, PM_NONE, PM_WALL, PM_PILL, PM_WALL, PM_NONE, PM_NONE, PM_NONE, PM_NONE, PM_NONE},
    {PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL, PM_NONE, PM_WALL, PM_WALL, PM_NONE, PM_WALL, PM_WALL, PM_NONE, PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL},
    {PM_LTPL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_NONE, PM_NONE, PM_WALL, PM_GHS0, PM_GHS1, PM_GHS2, PM_WALL, PM_NONE, PM_NONE, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_RTPL},
    {PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL, PM_NONE, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_NONE, PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL},
    {PM_NONE, PM_NONE, PM_NONE, PM_NONE, PM_NONE, PM_WALL, PM_PILL, PM_WALL, PM_NONE, PM_NONE, PM_NONE, PM_PCMN, PM_NONE, PM_NONE, PM_NONE, PM_WALL, PM_PILL, PM_WALL, PM_NONE, PM_NONE, PM_NONE, PM_NONE, PM_NONE},
    {PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL},
    {PM_WALL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_NONE, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_WALL},
    {PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL},
    {PM_WALL, PM_PILL, PM_WALL, PM_PILL, PM_PILL, PM_PILL, PM_WALL, PM_NONE, PM_NONE, PM_WALL, PM_PILL, PM_WALL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_WALL, PM_PILL, PM_WALL, PM_NONE, PM_NONE, PM_PILL, PM_WALL},
    {PM_WALL, PM_PILL, PM_WALL, PM_PILL, PM_PILL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL},
    {PM_WALL, PM_PILL, PM_WALL, PM_PILL, PM_PILL, PM_PILL, PM_WALL, PM_PILL, PM_PILL, PM_WALL, PM_PILL, PM_WALL, PM_PILL, PM_WALL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_WALL, PM_PILL, PM_WALL},
    {PM_WALL, PM_PILL, PM_WALL, PM_PILL, PM_PILL, PM_PILL, PM_WALL, PM_PILL, PM_PILL, PM_WALL, PM_PILL, PM_WALL, PM_PILL, PM_WALL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_WALL, PM_PILL, PM_WALL},
    {PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL, PM_PILL, PM_PILL, PM_WALL, PM_PILL, PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL, PM_WALL, PM_WALL, PM_PILL, PM_WALL},
    {PM_WALL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_WALL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_PILL, PM_WALL},
    {PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL, PM_WALL},
};

// clang-format on

#endif