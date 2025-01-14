#ifndef __PACMAN_MAZE_H
#define __PACMAN_MAZE_H

#include "glcd_types.h"
#include "utils.h"

// VARIABLES

#define PM_MAZE_SCALED_WIDTH 23
#define PM_MAZE_SCALED_HEIGHT 25
#define PM_MAZE_CELL_SIZE 10

const u16 PM_MazeWidthPixels = PM_MAZE_SCALED_WIDTH * PM_MAZE_CELL_SIZE;
const u16 PM_MazeHeightPixels = PM_MAZE_SCALED_HEIGHT * PM_MAZE_CELL_SIZE;

#define PM_PACMAN_RADIUS 4
#define PM_STAND_PILL_RADIUS 1
#define PM_SUPER_PILL_RADIUS 2

#define PM_STAND_PILL_COUNT 240
#define PM_SUPER_PILL_COUNT 6

#define PM_STAND_PILL_POINTS 10
#define PM_SUPER_PILL_POINTS 50

#define MAX_LIVES 5

// Colors
const LCD_Color PM_WallColor = 0x25283;
const LCD_Color PM_PacManColor = 0xFFD86B;
const LCD_Color PM_StandardPillColor = 0xE27417;
const LCD_Color PM_SuperPillColor = 0xFDB897;

const LCD_Color PM_NiceOrange = RGB8_TO_RGB565(0xe27417);

// TYPES

typedef struct
{
    u16 row, col;
} PM_MazeCell;

typedef enum
{
    PM_WALL, // Maze walls
    PM_NONE, // Empty cell, can be traversed if not sorrounded by walls
    PM_PILL, // Normal pill cell
    PM_SUPER_PILL,
    PM_PCMN, // PacMan
    PM_LTPL, // Left portal
    PM_RTPL, // Right portal
    PM_GHS0, // Ghost 0
    PM_GHS1, // Ghost 1
    PM_GHS2, // Ghost 2
} PM_MazeObj;

// MAZE

// clang-format off
PM_MazeObj PM_Maze[PM_MAZE_SCALED_HEIGHT][PM_MAZE_SCALED_WIDTH] = {
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