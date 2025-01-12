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

// STATE VARIABLES

_PRIVATE LCD_FontID s_font14, s_font20;

_PRIVATE bool s_game_is_ready = false, s_game_playing = false;
_PRIVATE LCD_Coordinate s_maze_pos;
_PRIVATE PM_GameStats s_game_stats;

_PRIVATE LCD_ObjID s_pm_id;
_PRIVATE PM_MovementDir s_pm_dir = PM_MOV_NONE;

_PRIVATE TP_ButtonArea s_splash_start_tp_area;
_PRIVATE LCD_ObjID s_splash_id;

_PRIVATE bool s_stat_titles_visible = false, s_stat_vals_visible = false;
_PRIVATE LCD_ObjID s_stat_titles_id = -1, s_stat_vals_id = -1;

// MOVEMENT CALLBACKS

// clang-format off
_PRIVATE _CBACK void move_pm_up(void) { s_pm_dir = PM_MOV_UP; }
_PRIVATE _CBACK void move_pm_down(void) { s_pm_dir = PM_MOV_DOWN; }
_PRIVATE _CBACK void move_pm_left(void) { s_pm_dir = PM_MOV_LEFT; }
_PRIVATE _CBACK void move_pm_right(void) { s_pm_dir = PM_MOV_RIGHT;}
// clang-format on

// DRAWING

// clang-format off
enum RenderMode { RM_DRAW = 0x1, RM_HIDE = 0x2, RM_UPDATE = 0x3 };
// clang-format on

_PRIVATE void stat_titles_render(u8 mode)
{
    if (mode & RM_DRAW)
    {
        if (s_stat_titles_id < 0) // Never drawn before
        {
            // clang-format off
            LCD_OBJECT(&s_stat_titles_id, {
                LCD_TEXT2(5, 5, {
                    .text = "GAME OVER",
                    .font = s_font14, .char_spacing = 2,
                    .text_color = LCD_COL_WHITE, .bg_color = LCD_COL_NONE,
                }),
                LCD_TEXT2(LCD_GetWidth() / 2 - 10, 5, {
                    .text = "SCORE",
                    .font = s_font14, .char_spacing = 2,
                    .text_color = LCD_COL_WHITE, .bg_color = LCD_COL_NONE,
                }),
                LCD_TEXT2(LCD_GetWidth() - 60, 5, {
                    .text = "RECORD",
                    .font = s_font14, .char_spacing = 2,
                    .text_color = LCD_COL_WHITE, .bg_color = LCD_COL_NONE,
                }),
            });
            // clang-format on

            LCD_RQRender();
        }
        else if (!LCD_IsObjectVisible(s_stat_titles_id))
            LCD_RQSetObjectVisibility(s_stat_titles_id, true, false);

        s_stat_titles_visible = true;
        return;
    }

    if (mode & RM_HIDE)
    {
        if (s_stat_titles_id < 0)
            return; // It's already hidden -> never been rendered!

        if (LCD_IsObjectVisible(s_stat_titles_id))
            LCD_RQSetObjectVisibility(s_stat_titles_id, false, false);

        s_stat_titles_visible = false;
        return;
    }
}

_PRIVATE void stat_values_render(u8 mode)
{
    char score_str[10], record_str[10], game_over_str[10];
    snprintf(score_str, 10, "%d", s_game_stats.score);
    snprintf(record_str, 10, "%d", s_game_stats.record);
    snprintf(game_over_str, 10, "%d", s_game_stats.game_over_in);

    const LCD_Color game_over_col = (s_game_stats.game_over_in < 10) ? LCD_COL_RED : LCD_COL_YELLOW;
    if (mode & (RM_HIDE | RM_UPDATE))
    {
        if (s_stat_titles_id < 0 && (mode & RM_HIDE))
            return; // Nothing to do, already deleted from screen.

        if (LCD_IsObjectVisible(s_stat_vals_id))
            LCD_RQSetObjectVisibility(s_stat_vals_id, false, false);

        s_stat_titles_visible = false;
        if (mode & RM_HIDE) // If we only need to hide, return.
            return;
    }

    if (mode & RM_DRAW)
    {
        if (s_stat_vals_id < 0) // Never drawn before
        {
            // clang-format off
            LCD_OBJECT(&s_stat_vals_id, {
                LCD_TEXT2(5, 20, {
                    .text = game_over_str,
                    .font = s_font14, .char_spacing = 2,
                    .text_color = LCD_COL_YELLOW, .bg_color = LCD_COL_YELLOW,
                }),
                LCD_TEXT2(LCD_GetWidth() / 2 - 10, 20, {
                    .text = score_str,
                    .font = s_font14, .char_spacing = 2,
                    .text_color = LCD_COL_YELLOW, .bg_color = LCD_COL_YELLOW,
                }),
                LCD_TEXT2(LCD_GetWidth() - 60, 20, {
                    .text = game_over_str,
                    .font = s_font14, .char_spacing = 2,
                    .text_color = game_over_col, .bg_color = game_over_col,
                }),
            });
            // clang-format on

            LCD_RQRender();
        }
        else if (!LCD_IsObjectVisible(s_stat_vals_id))
            LCD_RQSetObjectVisibility(s_stat_vals_id, true, false);

        s_stat_vals_visible = true;
        return;
    }
}

_PRIVATE void splash_waiting(void)
{
    const LCD_Color orange = rgb888_to_rgb565(0xe27417);
    const LCD_Coordinate logo_pos = {
        .x = (LCD_GetWidth() - Image_PACMAN_Logo.width) / 2,
        .y = (LCD_GetHeight() - Image_PACMAN_Logo.height) / 2 - 30,
    };

    const LCD_Coordinate button_pos = {
        .x = LCD_GetWidth() / 2 - 35,
        .y = logo_pos.y + Image_PACMAN_Logo.height + 10,
    };

    // clang-format off
    LCD_OBJECT(&s_splash_id, {
        LCD_IMAGE(logo_pos, Image_PACMAN_Logo),
        LCD_BUTTON(button_pos, s_splash_start_tp_area, {
            .label = LCD_BUTTON_LABEL({
                .text = "START!", 
                .font = s_font20,
                .text_color = LCD_COL_WHITE,
            }),
            .padding = {3, 5, 3, 5},
            .fill_color = orange, .edge_color = orange,
        }),
    });
    // clang-format on

    LCD_RQRender();
    s_game_is_ready = true;
}

// MAZE DRAWING

// clang-format off
enum Anchor { ANC_TOP_LEFT, ANC_CENTER };
// clang-format on

_PRIVATE inline void cell_to_coords(u16 row, u16 col, u8 anchor, LCD_Coordinate *out_coords)
{
    *out_coords = (LCD_Coordinate){
        .x = s_maze_pos.x + (col * PM_MAZE_CELL_SIZE) + (anchor == ANC_CENTER ? PM_MAZE_CELL_SIZE / 2 : 0),
        .y = s_maze_pos.y + (row * PM_MAZE_CELL_SIZE) + (anchor == ANC_CENTER ? PM_MAZE_CELL_SIZE / 2 : 0),
    };
}

_PRIVATE void draw_maze(void)
{
    PM_MazeObj obj;
    LCD_Coordinate obj_pixel_pos;
    for (u16 row = 0; row < PM_MAZE_SCALED_HEIGHT; row++)
    {
        for (u16 col = 0; col < PM_MAZE_SCALED_WIDTH; col++)
        {
            obj = PM_Maze[row][col];
            switch (obj)
            {
            case PM_WALL:
                cell_to_coords(row, col, ANC_TOP_LEFT, &obj_pixel_pos);
                // clang-format off
                LCD_RENDER_IMM({
                    LCD_RECT(obj_pixel_pos, {
                        .width = PM_MAZE_CELL_SIZE, .height = PM_MAZE_CELL_SIZE,
                        .fill_color = PM_WallColor, .edge_color = PM_WallColor,
                    }),
                });
                // clang-format on
                break;
            case PM_PILL:
            case PM_SUPER_PILL:
                cell_to_coords(row, col, ANC_CENTER, &obj_pixel_pos);
                // clang-format off
                LCD_RENDER_IMM({
                    LCD_CIRCLE({
                        .center = obj_pixel_pos, 
                        .fill_color = (obj == PM_PILL) ? PM_StandardPillColor : PM_SuperPillColor,
                        .edge_color = (obj == PM_PILL) ? PM_StandardPillColor : PM_SuperPillColor,
                        .radius = (obj == PM_PILL) ? PM_STAND_PILL_RADIUS : PM_SUPER_PILL_RADIUS
                    }),
                });
                // clang-format on
                break;
            case PM_PCMN:
                cell_to_coords(row, col, ANC_CENTER, &obj_pixel_pos);
                // clang-format off
                LCD_OBJECT(&s_pm_id, {
                    LCD_CIRCLE({
                        .center = obj_pixel_pos, .radius = PM_PACMAN_RADIUS,
                        .fill_color = PM_PacManColor, .edge_color = PM_PacManColor
                    }),
                });
                // clang-format off
                break;
            default:
                break;
            }
        }
    }

    LCD_RQRender();
}

// PACMAN MOVEMENT

_PRIVATE inline void get_pm_cell(u16 *out_row, u16 *out_col)
{
    for (u16 row = 0; row < PM_MAZE_SCALED_HEIGHT; row++)
    {
        for (u16 col = 0; col < PM_MAZE_SCALED_WIDTH; col++)
        {
            if (PM_Maze[row][col] == PM_PCMN)
            {
                *out_row = row;
                *out_col = col;
                return;
            }
        }
    }
}

_PRIVATE inline bool can_pm_move_along(PM_MovementDir dir, u16 pm_row, u16 pm_col)
{
    // Checking PacMan sorroundings. If there is a wall or the maze's boundaries
    // along the direction of movement, he can't move in that direction.
    switch (dir)
    {
    case PM_MOV_UP:
        // If the cell above (row - 1) is a wall
        return !(PM_Maze[pm_row - 1][pm_col] == PM_WALL);
    case PM_MOV_DOWN:
        // If the cell below (row + 1) is a wall
        return !(PM_Maze[pm_row + 1][pm_col] == PM_WALL);
    case PM_MOV_LEFT:
        // If the cell to the left (col - 1) is a wall
        return !(PM_Maze[pm_row][pm_col - 1] == PM_WALL);
    case PM_MOV_RIGHT:
        // If the cell to the right (col + 1) is a wall
        return !(PM_Maze[pm_row][pm_col + 1] == PM_WALL);
    default:
        return false;
    }

    return false; // This should never be reached.
}

_PRIVATE inline void move_pm_to(u16 old_row, u16 old_col, u16 new_row, u16 new_col)
{
    LCD_Coordinate new_pm_coords;
    cell_to_coords(new_row, new_col, ANC_CENTER, &new_pm_coords);

    LCD_Obj *obj;
    LCD_RQGetObject(s_pm_id, &obj);
    obj->comps->object.circle.center = new_pm_coords;
    LCD_RQUpdateObject(s_pm_id, false);

    PM_Maze[old_row][old_col] = PM_NONE;
    PM_Maze[new_row][new_col] = PM_PCMN;
}

_PRIVATE void try_move_pm_along(PM_MovementDir dir, u16 pm_row, u16 pm_col)
{
    // In this function, we replace PCMN with PM_NONE, as the pill checking
    // is done prior to this function call.

    u16 new_row = 0, new_col = 0;
    const bool can_move = can_pm_move_along(dir, pm_row, pm_col);
    switch (dir)
    {
    case PM_MOV_UP:
        if (can_move)
            new_row = --pm_row, new_col = pm_col;
        break;
    case PM_MOV_DOWN:
        if (can_move)
            new_row = ++pm_row, new_col = pm_col;
        break;
    case PM_MOV_LEFT:
        if (can_move)
            new_row = pm_row, new_col = --pm_col;
        break;
    case PM_MOV_RIGHT:
        if (can_move)
            new_row = pm_row, new_col = ++pm_col;
        break;
    default:
        return;
    }

    move_pm_to(pm_row, pm_col, new_row, new_col);
}


// LOOP CALLBACKS

_PRIVATE _CBACK void render_loop(void)
{
    /**
     * Scenarios to handle:
     * - PacMan on a pill / on portals?
     * - Score & record update
     * - Life updating
     * - Super pill spawn
     */
    if (s_pm_dir == PM_MOV_NONE)
        return;

    u16 pm_row, pm_col;
    get_pm_cell(&pm_row, &pm_col);
    const PM_MazeObj obj = PM_Maze[pm_row][pm_col];

    // Checking if on a pill, and in that case, updating score/record.
    if (obj == PM_PILL || obj == PM_SUPER_PILL)
    {
        s_game_stats.score += (obj == PM_PILL) ? PM_STAND_PILL_POINTS : PM_SUPER_PILL_COUNT;
        if (s_game_stats.score > s_game_stats.record)
            s_game_stats.record = s_game_stats.score;

        if (s_game_stats.score % 1000 && IS_BETWEEN_EQ(s_game_stats.lives, 1, 4))
            s_game_stats.lives++;

        stat_values_render(RM_UPDATE);
    }

    // Handling teleport if PCMN is one cell before LTPL / RTPL
    if (PM_Maze[pm_row][pm_col - 1] == PM_LTPL && s_pm_dir == PM_MOV_LEFT)
        move_pm_to(pm_row, pm_col, pm_row, PM_MAZE_SCALED_WIDTH - 1);
    else if (PM_Maze[pm_row][pm_col + 1] == PM_RTPL && s_pm_dir == PM_MOV_RIGHT)
        move_pm_to(pm_row, pm_col, pm_row, 1);
    else 
        try_move_pm_along(s_pm_dir, pm_row, pm_col);
}

_PRIVATE _CBACK void seconds_counter(void)
{
    assert(s_game_playing);

    s_game_stats.game_over_in--;
    if (s_game_stats.game_over_in == 0)
    {
        // TODO: Game over if there are some pills left.
    }

    stat_values_render(RM_UPDATE);
}

// PUBLIC FUNCTIONS

bool PACMAN_Init(void)
{
    if (!LCD_IsInitialized() || !RIT_IsEnabled() || !TP_IsInitialized())
        return false;

    LCD_SetBackgroundColor(LCD_COL_BLACK);
    LCD_FMAddFont(Font_Upheaval14, &s_font14);
    LCD_FMAddFont(Font_Upheaval20, &s_font20);

    s_game_stats = (PM_GameStats){0, 0, 60, 1};
    s_maze_pos = (LCD_Coordinate){
        .x = (LCD_GetWidth() - PM_MazeWidthPixels) / 2,
        .y = LCD_GetHeight() - PM_MazeHeightPixels - 25,
    };

    // Initializing colors
    PM_WallColor = rgb888_to_rgb565(0x25283);
    PM_PacManColor = rgb888_to_rgb565(0xFFD86B);
    PM_StandardPillColor = rgb888_to_rgb565(0xE27417);
    PM_SuperPillColor = rgb888_to_rgb565(0xFDB897);

    // Joystick initialization
    JOYSTICK_Init(JOY_POLL_WITH_RIT);
    splash_waiting();
    return s_game_is_ready;
}

void PACMAN_GameLoop(PM_GameSpeed speed)
{
    if (!s_game_is_ready)
        return;

    TP_WaitForButtonPress(s_splash_start_tp_area);
    LCD_RQRemoveObject(s_splash_id, false);

    // TODO: Generate 6 super pills

    stat_titles_render(RM_DRAW);
    stat_values_render(RM_DRAW);
    draw_maze();

    // Enabling joystick
    JOYSTICK_SetFunction(JOY_ACTION_UP, move_pm_up);
    JOYSTICK_SetFunction(JOY_ACTION_DOWN, move_pm_down);
    JOYSTICK_SetFunction(JOY_ACTION_LEFT, move_pm_left);
    JOYSTICK_SetFunction(JOY_ACTION_RIGHT, move_pm_right);

    // Render loop with desired speed & seconds counter
    RIT_AddJob(render_loop, speed);
    RIT_AddJob(seconds_counter, 1000 / RIT_GetIntervalMs());
    s_game_playing = true;
}
