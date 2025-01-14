#include "pm.h"
#include "peripherals.h"
#include "pm_maze.h"

#include <assert.h>
#include <stdio.h>

// Images
#include "Assets/Images/pacman-logo.h"

// Fonts
#include "Assets/Fonts/font-upheaval14.h"
#include "Assets/Fonts/font-upheaval20.h"

// PRIVATE TYPES

// clang-format off
enum Anchor { ANC_TOP_LEFT, ANC_CENTER };
// clang-format on

// STATE VARIABLES

_PRIVATE LCD_FontID s_font14, s_font20;
_PRIVATE LCD_Coordinate s_maze_pos;

_PRIVATE bool s_game_playing;
_PRIVATE PM_GameStats s_game_stats;

_PRIVATE PM_MazeCell s_pm_cell;
_PRIVATE PM_MovementDir s_pm_dir;
_PRIVATE LCD_ObjID s_pm_id;

// INFO VARIANTS TYPES & FUNCTIONS

_PRIVATE LCD_ObjID s_stats_values_id, s_stats_titles_id, s_pause_id;
_PRIVATE char record_value_str[10], score_value_str[10], game_over_in_value_str[10];

_PRIVATE void DrawInfoViews(void)
{
    // clang-format off
    LCD_OBJECT(&s_stats_titles_id, {
        LCD_TEXT2(5, 5, {
            .text = "GAME OVER", .font = s_font14,
            .char_spacing = 2, .text_color = LCD_COL_WHITE, .bg_color = LCD_COL_NONE,
        }),
        LCD_TEXT2(LCD_GetWidth() / 2 - 10, 5, {
            .text = "SCORE", .font = s_font14,
            .char_spacing = 2, .text_color = LCD_COL_WHITE, .bg_color = LCD_COL_NONE,
        }),
        LCD_TEXT2(LCD_GetWidth() - 60, 5, {
            .text = "RECORD", .font = s_font14,
            .char_spacing = 2, .text_color = LCD_COL_WHITE, .bg_color = LCD_COL_NONE,
        }),
    });

    const u8 y_off = 15;
    snprintf(record_value_str, 10, "----");
    snprintf(score_value_str, 10, "----");
    snprintf(game_over_in_value_str, 10, "--");

    LCD_OBJECT(&s_stats_values_id, {
        LCD_TEXT2(5, 5 + y_off, {
            .text = game_over_in_value_str, .font = s_font14,
            .char_spacing = 2, .text_color = PM_NiceOrange, .bg_color = LCD_COL_NONE,
        }),
        LCD_TEXT2(LCD_GetWidth() / 2 - 10, 5 + y_off, {
            .text = score_value_str, .font = s_font14,
            .char_spacing = 2, .text_color = PM_NiceOrange, .bg_color = LCD_COL_NONE,
        }),
        LCD_TEXT2(LCD_GetWidth() - 60, 5 + y_off, {
            .text = record_value_str, .font = s_font14,
            .char_spacing = 2, .text_color = PM_NiceOrange, .bg_color = LCD_COL_NONE,
        }),
    });
    // clang-format on
    LCD_RQRender();
}

_PRIVATE void InitPauseView(void)
{
    // clang-format off
    LCD_MANUAL_VISIBILITY_OBJECT(&s_pause_id, {
        LCD_TEXT2(LCD_GetWidth() / 2 - 50, 5, {
            .text = "PAUSED", .char_spacing = 2, .font = s_font20,
            .text_color = PM_NiceOrange, .bg_color = LCD_COL_NONE,
        }),
        LCD_TEXT2(LCD_GetWidth() / 2 - 88, 25, {
            .text = "PRESS INT0 TO RESUME!", .char_spacing = 1,
            .font = s_font14, .text_color = LCD_COL_WHITE, .bg_color = LCD_COL_NONE,
        }),
    });
    // clang-format on
}

// MAZE FUNCTIONS

_PRIVATE inline LCD_Coordinate MazeCellToCoords(PM_MazeCell cell, u8 anchor)
{
    return (LCD_Coordinate){
        .x = s_maze_pos.x + (cell.col * PM_MAZE_CELL_SIZE) + (anchor == ANC_CENTER ? PM_MAZE_CELL_SIZE / 2 : 0),
        .y = s_maze_pos.y + (cell.row * PM_MAZE_CELL_SIZE) + (anchor == ANC_CENTER ? PM_MAZE_CELL_SIZE / 2 : 0),
    };
}

_PRIVATE void DrawMaze(void)
{
    PM_MazeObj obj;
    PM_MazeCell obj_cell;
    for (u16 row = 0; row < PM_MAZE_SCALED_HEIGHT; row++)
    {
        for (u16 col = 0; col < PM_MAZE_SCALED_WIDTH; col++)
        {
            obj = PM_Maze[row][col];
            obj_cell = (PM_MazeCell){row, col};

            // clang-format off
            switch (obj)
            {
            case PM_WALL:
                LCD_RENDER_IMM({
                    LCD_RECT(MazeCellToCoords(obj_cell, ANC_TOP_LEFT), {
                        .width = PM_MAZE_CELL_SIZE, .height = PM_MAZE_CELL_SIZE,
                        .fill_color = PM_WallColor, .edge_color = PM_WallColor,
                    }),
                });
                break;
            case PM_PILL:
            case PM_SUPER_PILL:
                LCD_RENDER_IMM({
                    LCD_CIRCLE({
                        .center = MazeCellToCoords(obj_cell, ANC_CENTER),
                        .fill_color = (obj == PM_PILL) ? PM_StandardPillColor : PM_SuperPillColor,
                        .edge_color = (obj == PM_PILL) ? PM_StandardPillColor : PM_SuperPillColor,
                        .radius = (obj == PM_PILL) ? PM_STAND_PILL_RADIUS : PM_SUPER_PILL_RADIUS
                    }),
                });
                break;
            case PM_PCMN:
                LCD_OBJECT(&s_pm_id, {
                    LCD_CIRCLE({
                        .center = MazeCellToCoords(obj_cell, ANC_CENTER),
                        .radius = PM_PACMAN_RADIUS,
                        .fill_color = PM_PacManColor, .edge_color = PM_PacManColor
                    }),
                });
                break;
            default:
                break;
            }
            // clang-format on
        }
    }

    LCD_RQRender();
}

// LOOP CALLBACKS

_PRIVATE _CBACK void RenderLoop(void)
{
    if (!s_game_playing || s_pm_dir == PM_MOV_NONE)
        return;

    // Checking PacMan sorroundings. If there is a wall or the maze's boundaries
    // along the direction of movement, he can't move in that direction.
    PM_MazeCell new;
    switch (s_pm_dir)
    {
    case PM_MOV_UP:
        // If the cell above (row - 1) is a wall
        if (s_pm_cell.row > 0 && PM_Maze[s_pm_cell.row - 1][s_pm_cell.col] != PM_WALL)
        {
            new.row = s_pm_cell.row - 1;
            new.col = s_pm_cell.col;
            break;
        }
        else
            return;
    case PM_MOV_DOWN:
        // If the cell below (row + 1) is a wall
        if (s_pm_cell.row < PM_MAZE_SCALED_HEIGHT - 1 && PM_Maze[s_pm_cell.row + 1][s_pm_cell.col] != PM_WALL)
        {
            new.row = s_pm_cell.row + 1;
            new.col = s_pm_cell.col;
            break;
        }
        else
            return;
    case PM_MOV_LEFT:
        // If the cell to the left (col - 1) is a wall
        if (s_pm_cell.col > 0 && PM_Maze[s_pm_cell.row][s_pm_cell.col - 1] != PM_WALL)
        {
            new.row = s_pm_cell.row;
            new.col =
                (PM_Maze[s_pm_cell.row][s_pm_cell.col - 1] == PM_LTPL) ? (PM_MAZE_SCALED_WIDTH - 2) : s_pm_cell.col - 1;
            break;
        }
        else
            return;
    case PM_MOV_RIGHT:
        // If the cell to the right (col + 1) is a wall
        if (s_pm_cell.col < PM_MAZE_SCALED_WIDTH - 1 && PM_Maze[s_pm_cell.row][s_pm_cell.col + 1] != PM_WALL)
        {
            new.row = s_pm_cell.row;
            new.col = (PM_Maze[s_pm_cell.row][s_pm_cell.col + 1] == PM_RTPL) ? 1 : s_pm_cell.col + 1;
            break;
        }
        else
            return;
    default:
        return;
    }

    // Checking for pills or super pills in the next cell he's going to visit.
    u16 cur_score = s_game_stats.score;
    const PM_MazeObj new_obj = PM_Maze[new.row][new.col];
    if (new_obj == PM_PILL || new_obj == PM_SUPER_PILL)
    {
        cur_score += (new_obj == PM_PILL) ? PM_STAND_PILL_POINTS : PM_SUPER_PILL_POINTS;
        if (cur_score > s_game_stats.record)
            s_game_stats.record = cur_score;
        if (cur_score % 1000 && IS_BETWEEN_EQ(s_game_stats.lives, 1, MAX_LIVES - 1))
            s_game_stats.lives++;

        s_game_stats.score = cur_score;
    }

    // Finally, moving PacMan.
    LCD_RQMoveObject(s_pm_id, MazeCellToCoords(new, ANC_CENTER), false);
    PM_Maze[s_pm_cell.row][s_pm_cell.col] = PM_NONE;
    PM_Maze[new.row][new.col] = PM_PCMN;
    s_pm_cell = new;
}

_PRIVATE _CBACK void StatsUpdater(void)
{
    LCD_OBJECT_UPDATE(s_stats_values_id, false, {
        snprintf(record_value_str, 10, "%d", s_game_stats.record);
        snprintf(score_value_str, 10, "%d", s_game_stats.score);
    });
}

_PRIVATE _CBACK void GameOverCounter(void)
{
    if (!s_game_playing)
        return;

    if (!s_game_stats.game_over_in)
        return; // TODO: GameOver();
    else
    {
        // clang-format off
        s_game_stats.game_over_in--;
        LCD_OBJECT_UPDATE(s_stats_values_id, false, {
            snprintf(game_over_in_value_str, 10, "%d", s_game_stats.game_over_in);
        });
        // clang-format on
    }
}

// MOVEMENT CALLBACKS

// clang-format off
_PRIVATE _CBACK void MovePacmanUp(void)    { s_pm_dir = s_game_playing ? PM_MOV_UP : PM_MOV_NONE; }
_PRIVATE _CBACK void MovePacmanDown(void)  { s_pm_dir = s_game_playing ? PM_MOV_DOWN : PM_MOV_NONE; }
_PRIVATE _CBACK void MovePacmanLeft(void)  { s_pm_dir = s_game_playing ? PM_MOV_LEFT : PM_MOV_NONE; }
_PRIVATE _CBACK void MovePacmanRight(void) { s_pm_dir = s_game_playing ? PM_MOV_RIGHT : PM_MOV_NONE; }
// clang-format on

// PAUSE CALLBACK

_PRIVATE _CBACK void PauseResumeGame(void)
{
    if (!s_game_playing)
    {
        // Unpausing the game.
        LCD_RQSetObjectVisibility(s_pause_id, false, false);
        LCD_RQSetObjectVisibility(s_stats_titles_id, true, false);
        LCD_RQSetObjectVisibility(s_stats_values_id, true, false);
        s_game_playing = true;
        return;
    }

    // Else, we need to pause it.
    s_game_playing = false;
    s_pm_dir = PM_MOV_NONE;

    // Stopping both render loop & game over counter.
    LCD_RQSetObjectVisibility(s_stats_titles_id, false, false);
    LCD_RQSetObjectVisibility(s_stats_values_id, false, false);
    LCD_RQSetObjectVisibility(s_pause_id, true, false);
    LCD_RQRender();
}

// PUBLIC FUNCTIONS

bool PACMAN_Init(void)
{
    if (!LCD_IsInitialized() || !RIT_IsEnabled() || !TP_IsInitialized())
        return false;

    // LCD_SetBackgroundColor(LCD_COL_BLACK); Unnecessary, already done by TP.
    LCD_FMAddFont(Font_Upheaval14, &s_font14);
    LCD_FMAddFont(Font_Upheaval20, &s_font20);

    s_game_stats = (PM_GameStats){0, 0, 60, 1};
    s_maze_pos = (LCD_Coordinate){
        .x = (LCD_GetWidth() - PM_MazeWidthPixels) / 2,
        .y = LCD_GetHeight() - PM_MazeHeightPixels - 25,
    };

    // Joystick & Buttons initialization. Very high priority to INT0
    // because otherwise it may lose against RIT interrupts.
    JOYSTICK_Init(JOY_POLL_WITH_RIT);
    BUTTON_Init(BTN_DEBOUNCE_WITH_RIT, INT_PRIO_MAX, INT_PRIO_DEF, INT_PRIO_DEF);
    return true;
}

void PACMAN_Play(PM_GameSpeed speed)
{
    const LCD_Coordinate logo_pos = {
        .x = (LCD_GetWidth() - Image_PACMAN_Logo.width) / 2,
        .y = (LCD_GetHeight() - Image_PACMAN_Logo.height) / 2 - 30,
    };

    const LCD_Coordinate button_pos = {
        .x = LCD_GetWidth() / 2 - 35,
        .y = logo_pos.y + Image_PACMAN_Logo.height + 10,
    };
    /*

    LCD_ObjID splash_id;
    TP_ButtonArea button_tp;

    // clang-format off
        LCD_OBJECT(&splash_id, {
            LCD_IMAGE(logo_pos, Image_PACMAN_Logo),
            LCD_BUTTON(button_pos, button_tp, {
                .label = LCD_BUTTON_LABEL({.text = "START", .font = s_font20, .text_color = LCD_COL_WHITE}),
                .padding = {3, 5, 3, 5},
                .fill_color = orange, .edge_color = orange,
            }),
        });
    // clang-format on

    LCD_RQRender();
    TP_WaitForButtonPress(button_tp);
    LCD_RQRemoveObject(splash_id, false);

    */

    // Orchestrating the game
    // TODO: Generate 6 super pills randomly during the game

    DrawInfoViews();
    InitPauseView();
    DrawMaze();

    // Determining the initial PacMan position
    for (u16 row = 0; row < PM_MAZE_SCALED_HEIGHT; row++)
    {
        for (u16 col = 0; col < PM_MAZE_SCALED_WIDTH; col++)
        {
            if (PM_Maze[row][col] == PM_PCMN)
            {
                s_pm_cell = (PM_MazeCell){row, col};
                break;
            }
        }
    }

    // Enabling controls
    JOYSTICK_SetFunction(JOY_ACTION_UP, MovePacmanUp);
    JOYSTICK_SetFunction(JOY_ACTION_DOWN, MovePacmanDown);
    JOYSTICK_SetFunction(JOY_ACTION_LEFT, MovePacmanLeft);
    JOYSTICK_SetFunction(JOY_ACTION_RIGHT, MovePacmanRight);

    // Enabling the Pause Button
    BUTTON_SetInterruptHandler(BTN_INT_SRC_EINT0, PauseResumeGame);

    // Render loop with desired speed & seconds counter
    RIT_AddJob(RenderLoop, speed);
    RIT_AddJob(GameOverCounter, 1000 / RIT_GetIntervalMs());
    RIT_AddJob(StatsUpdater, 500 / RIT_GetIntervalMs());

    s_game_playing = true;
    LCD_OBJECT_UPDATE(s_stats_values_id, false, {
        snprintf(record_value_str, 10, "%d", s_game_stats.record);
        snprintf(score_value_str, 10, "%d", s_game_stats.score);
    });
}
