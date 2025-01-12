#include "pacman.h"
#include "pacman-maze.h"
#include "peripherals.h"

#include <stdio.h>

// Images
#include "Assets/Images/pacman-logo.h"

// Fonts
#include "Assets/Fonts/font-upheavtt14.h"
#include "Assets/Fonts/font-upheavtt20.h"

// PRIVATE TYPES

typedef struct
{
    u16 row, col;
} PM_MazeCell;

typedef enum
{
    ANCHOR_TOP_LEFT,
    ANCHOR_CENTER,
} PM_Anchor;

typedef enum
{
    PM_PILL_NORMAL,
    PM_PILL_SUPER,
} PM_PillType;

typedef struct
{
    PM_MazeCell cell_pos;
    LCD_Coordinate pixel_pos;
    u8 type;
} PM_Pill;

// VARIABLES

_PRIVATE LCD_FontID font_upheaven14, font_upheaven20;

_PRIVATE LCD_Coordinate g_maze_pos;

const u8 val_y_padding = 15;

const LCD_Coordinate go_pos = {
    5, 5}; /* record_pos = {LCD_GetWidth() - 60, 5}, score_pos = {LCD_GetWidth() / 2 - 10, 5}; */

// OBJECT IDs

_PRIVATE PM_GameStats g_stats;

_PRIVATE LCD_ObjID stats_titles, game_over_obj, score_obj, record_obj;

/// @brief Saving the coordinates of every pill, so that they can be removed from the screen when eaten.
_PRIVATE PM_Pill pills_coords[PM_PILL_COUNT] = {0};

/// @brief Saving the ObjID of PacMan, so that it can be moved around.
_PRIVATE LCD_ObjID pacman_id;
_PRIVATE PM_MovementDirection pacman_dir = PM_MOVE_NONE;

// PRIVATE FUNCTIONS

_PRIVATE inline void draw_go_stats(void)
{
    if (!LCD_IsObjectVisible(game_over_obj))
        return;

    char go_str[10];
    snprintf(go_str, 10, "%d", g_stats.game_over_in);

    LCD_RQRemoveObject(game_over_obj, false);
    LCD_OBJECT(&game_over_obj, {
                                   LCD_TEXT2(go_pos.x, go_pos.y + val_y_padding,
                                             {
                                                 .text = go_str,
                                                 .font = font_upheaven14,
                                                 .text_color = LCD_COL_YELLOW,
                                                 .bg_color = LCD_COL_NONE,
                                             }),
                               });

    LCD_RQRender();
}

_PRIVATE inline LCD_Coordinate cell_to_coords(PM_MazeCell cell, u16 cell_sz, LCD_Coordinate maze_pos, PM_Anchor anchor)
{
    return (LCD_Coordinate){
        .x = maze_pos.x + (cell.col * cell_sz) + (anchor == ANCHOR_CENTER ? cell_sz / 2 : 0),
        .y = maze_pos.y + (cell.row * cell_sz) + (anchor == ANCHOR_CENTER ? cell_sz / 2 : 0),
    };
}

_PRIVATE void render_maze(LCD_Coordinate maze_pos)
{
    const u16 cell_sz = PM_MAZE_CELL_SIZE;

    u16 idx = 0;
    PM_MazeCell cell;
    LCD_Color cell_col;
    PM_MazeObject cell_obj;
    LCD_Coordinate cell_pos;
    for (u16 row = 0; row < PM_MAZE_SCALED_HEIGHT; row++)
    {
        for (u16 col = 0; col < PM_MAZE_SCALED_WIDTH; col++)
        {
            cell.row = row, cell.col = col;
            cell_obj = PM_Maze[row][col];
            switch (cell_obj)
            {
            case PM_WALL:
                cell_col = rgb888_to_rgb565(PM_WALL_COL);
                cell_pos = cell_to_coords(cell, cell_sz, maze_pos, ANCHOR_TOP_LEFT);

                // clang-format off
                // Rendering walls as immediate objects, as they are static and 
                // don't need to be selectively removed from the screen.
                LCD_RENDER_IMM({
                    LCD_RECT(cell_pos, {
                        .width = cell_sz, .height = cell_sz,
                        .fill_color = cell_col, .edge_color = cell_col,
                    }),
                });
                // clang-format on
                break;
            case PM_PILL:
                cell_pos = cell_to_coords(cell, cell_sz, maze_pos, ANCHOR_CENTER);

                // clang-format off
                // Rendering pills as immediate objects, but saving their coordinates
                // so that they can be removed from the screen when eaten.
                pills_coords[idx++] = (PM_Pill){
                    .cell_pos = cell, .pixel_pos = cell_pos, .type = PM_PILL_NORMAL,
                };
                // clang-format on
                break;
            case PM_PCMN:
                cell_col = rgb888_to_rgb565(PM_PCMN_COL);
                cell_pos = cell_to_coords(cell, cell_sz, maze_pos, ANCHOR_CENTER);

                // clang-format off
                // Rendering PacMan as a identifiable object, as it will be moved around
                // and needs to be selectively removed from the screen.
                LCD_OBJECT(&pacman_id, {
                    LCD_CIRCLE({
                        .center = cell_pos, .radius = 4,
                        .fill_color = cell_col, .edge_color = cell_col,
                    }),
                });
                // clang-format on
            }
        }
    }

    PRNG_Seed(USE_AUTO_SEED);
    u16 index;
    for (u8 i = 0; i < 6;)
    {
        index = PRNG_Range(0, PM_PILL_COUNT - 1);
        if (pills_coords[index].type == PM_PILL_SUPER)
            continue;

        pills_coords[index].type = PM_PILL_SUPER;
        i++;
    }
    PRNG_Deinit();

    // Rendering pills
    PM_Pill *pill;

    const LCD_Color std_pill_col = rgb888_to_rgb565(PM_STD_PILL_COL),
                    super_pill_col = rgb888_to_rgb565(PM_SUP_PILL_COL);
    for (u16 i = 0; i < PM_PILL_COUNT; i++)
    {
        pill = &pills_coords[i];
        LCD_RENDER_IMM({
            LCD_CIRCLE({
                .center = pill->pixel_pos,
                .radius = pill->type == PM_PILL_NORMAL ? 1 : 2,
                .fill_color = pill->type == PM_PILL_NORMAL ? std_pill_col : super_pill_col,
                .edge_color = pill->type == PM_PILL_NORMAL ? std_pill_col : super_pill_col,
            }),
        });
    }

    LCD_RQRender();
}

_PRIVATE void render_splash(void)
{
    const LCD_Coordinate logo_pos = {
        .x = (LCD_GetWidth() - Image_PACMAN_Logo.width) / 2,
        .y = (LCD_GetHeight() - Image_PACMAN_Logo.height) / 2 - 30,
    };

    const LCD_Coordinate button_pos = {
        .x = LCD_GetWidth() / 2 - 35,
        .y = logo_pos.y + Image_PACMAN_Logo.height + 10,
    };

    LCD_ObjID welcome_id;
    TP_ButtonArea start_button_area;
    const LCD_Color nice_orange = rgb888_to_rgb565(0xe27417);

    // clang-format off
    LCD_OBJECT(&welcome_id, {
        LCD_IMAGE(logo_pos, Image_PACMAN_Logo),
        LCD_BUTTON(button_pos, start_button_area, {
            .label = LCD_BUTTON_LABEL({
                .text = "START!", 
                .font = font_upheaven20,
                .text_color = LCD_COL_WHITE,
            }),
            .padding = {3, 5, 3, 5},
            .fill_color = nice_orange,
            .edge_color = nice_orange,
        }),
    });
    // clang-format on

    LCD_RQRender();
    TP_WaitForButtonPress(start_button_area);
    LCD_RQRemoveObject(welcome_id, false);
}

// MOVEMENT FUNCTIONS
// When the joystick is moved in a direction, Pacman will move in that direction
// until it hits a wall, the maze's boundaries, or another joystick input is received.

_PRIVATE void pm_move_up(void)
{
    pacman_dir = PM_MOV_UP;
}

_PRIVATE void pm_move_down(void)
{
    pacman_dir = PM_MOV_DOWN;
}

_PRIVATE void pm_move_left(void)
{
    pacman_dir = PM_MOV_LEFT;
}

_PRIVATE void pm_move_right(void)
{
    pacman_dir = PM_MOV_RIGHT;
}

// RENDER LOOP

PM_MazeCell get_pacman_pos(void)
{
    // don't have a function for getting pacman pos, need to look at the PM_PCMN in the maze
    PM_MazeCell pacman_cell;
    for (u16 row = 0; row < PM_MAZE_SCALED_HEIGHT; row++)
    {
        for (u16 col = 0; col < PM_MAZE_SCALED_WIDTH; col++)
        {
            if (PM_Maze[row][col] == PM_PCMN)
            {
                pacman_cell.row = row;
                pacman_cell.col = col;
                return pacman_cell;
            }
        }
    }

    // should never reach this point
    return pacman_cell;
}

bool can_pm_move_in_dir(PM_MazeCell cell, PM_MovementDirection dir)
{
    // Checking pacman sorroundings. If there is a wall or the maze's boundaries along
    // the direction of movement, PacMan can't move in that direction.
    switch (dir)
    {
    case PM_MOV_UP:
        if (cell.row - 1 == 0 || PM_Maze[cell.row - 1][cell.col] == PM_WALL)
            return false;
        break;

    case PM_MOV_DOWN:
        if (cell.row + 1 == PM_MAZE_SCALED_HEIGHT || PM_Maze[cell.row + 1][cell.col] == PM_WALL)
            return false;
        break;

    case PM_MOV_LEFT:
        if (cell.col == 0 || PM_Maze[cell.row][cell.col - 1] == PM_WALL)
            return false;
        break;

    case PM_MOV_RIGHT:
        if (cell.col == PM_MAZE_SCALED_WIDTH || PM_Maze[cell.row][cell.col + 1] == PM_WALL)
            return false;
        break;
    }

    return true;
}

void move_pacman_1_cell(PM_MazeCell cell, PM_MovementDirection dir)
{
    PM_MazeCell new_cell;
    // Moving PacMan 1 cell along the direction
    switch (dir)
    {
    case PM_MOV_UP:
        PM_Maze[cell.row][cell.col] = PM_NONE;
        PM_Maze[cell.row - 1][cell.col] = PM_PCMN;
        new_cell.row = cell.row - 1;
        new_cell.col = cell.col;
        break;

    case PM_MOV_DOWN:
        PM_Maze[cell.row][cell.col] = PM_NONE;
        PM_Maze[cell.row + 1][cell.col] = PM_PCMN;
        new_cell.row = cell.row + 1;
        new_cell.col = cell.col;
        break;

    case PM_MOV_LEFT:
        PM_Maze[cell.row][cell.col] = PM_NONE;
        PM_Maze[cell.row][cell.col - 1] = PM_PCMN;
        new_cell.row = cell.row;
        new_cell.col = cell.col - 1;
        break;

    case PM_MOV_RIGHT:
        PM_Maze[cell.row][cell.col] = PM_NONE;
        PM_Maze[cell.row][cell.col + 1] = PM_PCMN;
        new_cell.row = cell.row;
        new_cell.col = cell.col + 1;
        break;
    }

    // Mapping new cell to pixels
    LCD_Coordinate pacman_pos = cell_to_coords(new_cell, PM_MAZE_CELL_SIZE, g_maze_pos, ANCHOR_CENTER);
    LCD_RQMoveObject(pacman_id, pacman_pos, false);
}

void update()
{
    // This function needs to react to events, such as joystick inputs, and update the screen accordingly.

    // If PacMan is moving, we need to check if it can move in the current direction.
    // If it can, we move it and update the screen.
    // If it can't, we stop PacMan and wait for the next joystick input.
    if (pacman_dir != PM_MOVE_NONE)
    {
        // Getting the current pacman position
        PM_MazeCell cell = get_pacman_pos();

        if (cell.row == 12 && cell.col == 0 && pacman_dir == PM_MOV_LEFT)
        {
            PM_MazeCell rtpl_cell = {12, PM_MAZE_SCALED_WIDTH - 1};
            LCD_Coordinate rtpl = cell_to_coords(rtpl_cell, PM_MAZE_CELL_SIZE, g_maze_pos, ANCHOR_CENTER);
            LCD_RQMoveObject(pacman_id, rtpl, false);
        }
        else if (cell.row == 12 && cell.col == PM_MAZE_SCALED_WIDTH - 1 && pacman_dir == PM_MOV_RIGHT)
        {
            PM_MazeCell ltpl_cell = {12, 0};
            LCD_Coordinate ltpl = cell_to_coords(ltpl_cell, PM_MAZE_CELL_SIZE, g_maze_pos, ANCHOR_CENTER);
            pacman_dir = PM_MOVE_NONE;
            LCD_RQMoveObject(pacman_id, ltpl, false);
        }
        else
        {
            // Checking if PacMan can move in the current direction
            bool can_proceed = can_pm_move_in_dir(cell, pacman_dir);
            if (can_proceed)
            {
                // moving pacman 1 cell along the direction
                move_pacman_1_cell(cell, pacman_dir);
            }
            else
            {
                // stopping pacman
                pacman_dir = PM_MOVE_NONE;
            }
        }
    }

    // checking if pacman is on a pill
    PM_MazeCell pacman_cell = get_pacman_pos();
    PM_Pill *pill;
    for (u16 i = 0; i < PM_PILL_COUNT; i++)
    {
        pill = &pills_coords[i];
        if (!memcmp(&pill->cell_pos, &pacman_cell, sizeof(PM_MazeCell)))
        {
            // Pacman is on a cell
            // Removing the pill from the screen. this is done by pacman move 1 cell,
            // which replaces the current pacman position with NONE

            // updating the score
            g_stats.score += pill->type == PM_PILL_NORMAL ? 10 : 50;

            // updating the record
            if (g_stats.score > g_stats.record)
                g_stats.record = g_stats.score;
        }
    }
}

void update_time_left(void)
{
    if (g_stats.game_over_in == 0)
    {
        // game_over
    }

    g_stats.game_over_in--;
    draw_go_stats();
}

// PUBLIC FUNCTIONS

PM_Error PACMAN_Init(void)
{
    if (!LCD_IsInitialized())
        return PM_ERR_UNINIT_LCD;

    LCD_SetBackgroundColor(LCD_COL_BLACK);

    // Adding fonts
    LCD_FMAddFont(Font_Upheavtt14, &font_upheaven14);
    LCD_FMAddFont(Font_Upheavtt20, &font_upheaven20);

    const LCD_Coordinate maze_pos = {
        .x = (LCD_GetWidth() - PM_MazeWidthPixels) / 2,
        .y = LCD_GetHeight() - PM_MazeHeightPixels - 25,
    };

    g_maze_pos = maze_pos;

    // render_splash();

    g_stats = (PM_GameStats){
        .score = 0,
        .record = 0,
        .game_over_in = 60,
    };

    /*
        // clang-format off
            LCD_OBJECT(&stats_titles, {
            LCD_TEXT(go_pos, {
                .text = "GAME OVER",
                .font = font_upheaven14, .char_spacing = 2,
                .text_color = LCD_COL_WHITE, .bg_color = LCD_COL_NONE,
            }),
            LCD_TEXT(score_pos, {
                .text = "SCORE",
                .font = font_upheaven14, .char_spacing = 2,
                .text_color = LCD_COL_WHITE, .bg_color = LCD_COL_NONE,
            }),
            LCD_TEXT(record_pos, {
                .text = "RECORD",
                .font = font_upheaven14, .char_spacing = 2,
                .text_color = LCD_COL_WHITE, .bg_color = LCD_COL_NONE,
            }),
        });
        */

    render_maze(maze_pos);

    // Initializing joystick for PacMan movement
    JOYSTICK_Init(JOY_POLL_WITH_RIT);
    JOYSTICK_SetFunction(JOY_ACTION_UP, pm_move_up);
    JOYSTICK_SetFunction(JOY_ACTION_DOWN, pm_move_down);
    JOYSTICK_SetFunction(JOY_ACTION_LEFT, pm_move_left);
    JOYSTICK_SetFunction(JOY_ACTION_RIGHT, pm_move_right);

    // Adding the render loop to the RIT
    RIT_AddJob(update, 2);

    // 50 ms for 1 second?

    RIT_AddJob(update_time_left, 20); // every 1 second

    return PM_ERR_NONE;
}