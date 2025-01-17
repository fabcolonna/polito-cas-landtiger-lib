#include "pacman.h"
#include "pacman_maze.h"
#include "peripherals.h"
#include <stdio.h>

// Assets
#include "Assets/Fonts/font-upheaval14.h"
#include "Assets/Fonts/font-upheaval20.h"
#include "Assets/Images/pacman-life.h"
#include "Assets/Images/pacman-logo.h"
#include "Assets/Images/pacman-sad.h"
#include "Assets/Images/pacman-victory.h"

// PRIVATE TYPES

// clang-format off
enum Anchor { ANC_TOP_LEFT, ANC_CENTER };

// STATE VARIABLES

_PRIVATE PM_Game sGame;
_PRIVATE LCD_ObjID sPauseID;
_PRIVATE LCD_FontID sFont14, sFont20;

PM_MazeObj copied_maze[PM_MAZE_SCALED_HEIGHT][PM_MAZE_SCALED_WIDTH];

// Memory arena allocation for game.
LCD_MA_ALLOC_STATIC_MEM(sMemory, 4096);

// OTHER VARIABLES

_PRIVATE const LCD_Color sColorOrange = RGB8_TO_RGB565(0xe27417);

// PROTOTYPES

_PROTOTYPE_ONLY _PRIVATE void pDoPlay(void);
_PROTOTYPE_ONLY _PRIVATE void pGameVictory(void);
_PROTOTYPE_ONLY _PRIVATE void pGameDefeat(void);

// UTILS

_PRIVATE inline LCD_Coordinate pMazeCellToCoords(PM_MazeCell cell, enum Anchor anchor)
{
    return (LCD_Coordinate){
        .x = sGame.maze_pos.x + (cell.col * PM_MAZE_CELL_SIZE) + (anchor == ANC_CENTER ? PM_MAZE_CELL_SIZE / 2 : 0),
        .y = sGame.maze_pos.y + (cell.row * PM_MAZE_CELL_SIZE) + (anchor == ANC_CENTER ? PM_MAZE_CELL_SIZE / 2 : 0),
    };
}

// VIEW DEFINITIONS

_PRIVATE void pInitInfoView(void)
{
    // clang-format off
    LCD_OBJECT(&sGame.stat_obj_ids.titles, {
        LCD_TEXT2(5, 5, {
            .text = "GAME OVER", .font = sFont14, .char_spacing = 2, 
            .text_color = LCD_COL_WHITE, .bg_color = LCD_COL_NONE,
        }),
        LCD_TEXT2(LCD_GetWidth() / 2 - 10, 5, {
            .text = "SCORE", .font = sFont14, .char_spacing = 2, 
            .text_color = LCD_COL_WHITE, .bg_color = LCD_COL_NONE,
        }),
        LCD_TEXT2(LCD_GetWidth() - 60, 5, {
            .text = "RECORD", .font = sFont14, .char_spacing = 2, 
            .text_color = LCD_COL_WHITE, .bg_color = LCD_COL_NONE,
        }),
        LCD_TEXT2(LCD_GetWidth() / 2 - 90, LCD_GetHeight() - 20, {
            .text = "LIVES:", .font = sFont14, .char_spacing = 1, 
            .text_color = LCD_COL_WHITE, .bg_color = LCD_COL_NONE,
        }),
    });

    sprintf(sGame.stat_strings.score, "%d", sGame.stat_values.score);
    sprintf(sGame.stat_strings.record, "%d", sGame.stat_values.record);
    sprintf(sGame.stat_strings.game_over_in, "%d", sGame.stat_values.game_over_in);

    LCD_OBJECT(&sGame.stat_obj_ids.game_over_in_value, {
        LCD_TEXT2(5, 20, {
            .text = sGame.stat_strings.game_over_in, .font = sFont14,
            .char_spacing = 2, .text_color = sColorOrange, .bg_color = LCD_COL_NONE,
        }),
    });

    LCD_OBJECT(&sGame.stat_obj_ids.score_record_values, {
        LCD_TEXT2(LCD_GetWidth() / 2 - 10, 20, {
            .text = sGame.stat_strings.score, .font = sFont14,
            .char_spacing = 2, .text_color = sColorOrange, .bg_color = LCD_COL_NONE,
        }),
        LCD_TEXT2(LCD_GetWidth() - 60, 20, {
            .text = sGame.stat_strings.record, .font = sFont14,
            .char_spacing = 2, .text_color = sColorOrange, .bg_color = LCD_COL_NONE,
        }),
    });

    // Defininig lives, which will be visible on manual control.
    LCD_Coordinate life_img_pos;
    for (u8 i = 0; i < MAX_LIVES; i++)
    {
        life_img_pos = (LCD_Coordinate){
            .x = LCD_GetWidth() / 2 - 80 + (25 * i) + (2 * Image_PACMAN_Life.width),
            .y = LCD_GetHeight() - 28,
        };

        LCD_MANUAL_VISIBILITY_OBJECT(&sGame.stat_obj_ids.lives[i], {
            LCD_IMAGE(life_img_pos, Image_PACMAN_Life),
        });
    }

    // 1 life is always present
    LCD_RQSetObjectVisibility(sGame.stat_obj_ids.lives[0], true, false);
    // clang-format on
}

_PRIVATE void pInitPauseView(void)
{
    // clang-format off
    LCD_MANUAL_VISIBILITY_OBJECT(&sPauseID, {
        LCD_TEXT2(LCD_GetWidth() / 2 - 50, 5, {
            .text = "PAUSED", .char_spacing = 2, .font = sFont20,
            .text_color = sColorOrange, .bg_color = LCD_COL_NONE,
        }),
        LCD_TEXT2(LCD_GetWidth() / 2 - 88, 25, {
            .text = "PRESS INT0 TO RESUME!", .char_spacing = 1,
            .font = sFont14, .text_color = LCD_COL_WHITE, .bg_color = LCD_COL_NONE,
        }),
    });
    // clang-format on
}

_PRIVATE void pDrawMaze(void)
{
    PM_MazeObj obj;
    PM_MazeCell obj_cell;
    for (u16 row = 0; row < PM_MAZE_SCALED_HEIGHT; row++)
    {
        for (u16 col = 0; col < PM_MAZE_SCALED_WIDTH; col++)
        {
            obj = copied_maze[row][col];
            obj_cell = (PM_MazeCell){row, col};

            // clang-format off
            switch (obj)
            {
            case PM_WALL:
                LCD_RENDER_IMM({
                    LCD_RECT(pMazeCellToCoords(obj_cell, ANC_TOP_LEFT), {
                        .width = PM_MAZE_CELL_SIZE, .height = PM_MAZE_CELL_SIZE,
                        .fill_color = PM_WALL_COLOR, .edge_color = PM_WALL_COLOR,
                    }),
                });
                break;
            case PM_PILL:
            case PM_SUPER_PILL:
                LCD_RENDER_IMM({
                    LCD_CIRCLE({
                        .center = pMazeCellToCoords(obj_cell, ANC_CENTER),
                        .fill_color = (obj == PM_PILL) ? PM_STD_PILL_COLOR : PM_SUP_PILL_COLOR,
                        .edge_color = (obj == PM_PILL) ? PM_STD_PILL_COLOR : PM_SUP_PILL_COLOR,
                        .radius = (obj == PM_PILL) ? PM_STD_PILL_RADIUS : PM_SUP_PILL_RADIUS,
                    }),
                });
                break;
            case PM_PCMN:
                LCD_OBJECT(&sGame.pacman.id, {
                    LCD_CIRCLE({
                        .center = pMazeCellToCoords(obj_cell, ANC_CENTER), .radius = PM_PACMAN_RADIUS,
                        .fill_color = PM_PACMAN_COLOR, .edge_color = PM_PACMAN_COLOR
                    }),
                });
                break;
            default:
                break;
            }
            // clang-format on
        }
    }
}

// POWER PILLS

_PRIVATE inline bool pSuperPillSpawnSecAlreadyTaken(u32 sec, u16 super_pills_arr_length)
{
    for (u8 i = 0; i < super_pills_arr_length; i++)
        if (sGame.super_pills[i].spawn_sec == sec)
            return true;

    return false;
}

_PRIVATE void pInitSuperPills(void)
{
    // 6 Super pills need to be generated at random positions in the maze
    // (i.e. we have to randomize the row & col values of the maze cells),
    // and at random times (i.e. we have to randomize the time intervals
    // between the generation of each power pill), till the end of the 60s.
    PRNG_Set(PRNG_USE_AUTO_SEED);

    u16 i = 0, row, col, spawn_sec = 0;
    while (i < PM_SUP_PILL_COUNT)
    {
        // Generate random position in maze
        row = PRNG_Range(1, PM_MAZE_SCALED_HEIGHT - 1);
        col = PRNG_Range(1, PM_MAZE_SCALED_WIDTH - 1);

        // Checking if the cell is not a wall, another power pill, PacMan,
        // a teleport, or a ghost. If it is, we need to generate another one.
        if (copied_maze[row][col] != PM_PILL)
            continue;

        sGame.super_pills[i].cell = (PM_MazeCell){row, col};

        // clang-format off
        LCD_MANUAL_VISIBILITY_OBJECT(&sGame.super_pills[i].id, {
            LCD_CIRCLE({
                .center = pMazeCellToCoords(sGame.super_pills[i].cell, ANC_CENTER),
                .fill_color = PM_SUP_PILL_COLOR,
                .edge_color = PM_SUP_PILL_COLOR,
                .radius = PM_SUP_PILL_RADIUS,
            }),
        });
        // clang-format on

        // Generate random times for the power pills to spawn
        do
        {
            spawn_sec = PRNG_Range(5, 59);
        } while (pSuperPillSpawnSecAlreadyTaken(spawn_sec, i));

        sGame.super_pills[i].spawn_sec = spawn_sec;
        i++;
    }

    PRNG_Release();
}

// CALLBACKS

_PRIVATE _CBACK void pRenderLoop(void)
{
    // If not playing or moving, nothing new to render.
    if (!sGame.playing_now || sGame.pacman.dir == PM_MOV_NONE)
        return;

    // Checking PacMan sorroundings. If there is a wall or the maze's boundaries
    // along the direction of movement, he can't move and just stops till new dir.
    PM_MazeCell new, pacman = sGame.pacman.cell;
    switch (sGame.pacman.dir)
    {
    case PM_MOV_UP:
        // If the cell above (row - 1) is not a wall, continue.
        if (pacman.row > 0 && copied_maze[pacman.row - 1][pacman.col] != PM_WALL)
        {
            new.row = pacman.row - 1;
            new.col = pacman.col;
            break;
        }
        else
        {
            sGame.pacman.dir = PM_MOV_NONE;
            return;
        }
    case PM_MOV_DOWN:
        if (pacman.row < PM_MAZE_SCALED_HEIGHT - 1 && copied_maze[pacman.row + 1][pacman.col] != PM_WALL)
        {
            new.row = pacman.row + 1;
            new.col = pacman.col;
            break;
        }
        else
        {
            sGame.pacman.dir = PM_MOV_NONE;
            return;
        }
    case PM_MOV_LEFT:
        // If the cell to the left (col - 1) is not a wall, continue.
        if (pacman.col > 0 && copied_maze[pacman.row][pacman.col - 1] != PM_WALL)
        {
            new.row = pacman.row;
            // If it's a teleport, jump to the right one.
            new.col =
                (copied_maze[pacman.row][pacman.col - 1] == PM_LTPL) ? (PM_MAZE_SCALED_WIDTH - 2) : pacman.col - 1;
            break;
        }
        else
        {
            sGame.pacman.dir = PM_MOV_NONE;
            return;
        }
    case PM_MOV_RIGHT:
        if (pacman.col < PM_MAZE_SCALED_WIDTH - 1 && copied_maze[pacman.row][pacman.col + 1] != PM_WALL)
        {
            new.row = pacman.row;
            // If it's a teleport, jump to the left one.
            new.col = (copied_maze[pacman.row][pacman.col + 1] == PM_RTPL) ? 1 : pacman.col + 1;
            break;
        }
        else
        {
            sGame.pacman.dir = PM_MOV_NONE;
            return;
        }
    default:
        return;
    }

    // Checking for pills or super pills in the next cell he's going to visit.
    u16 cur_score = sGame.stat_values.score;
    const PM_MazeObj new_obj = copied_maze[new.row][new.col];
    if (new_obj == PM_PILL || new_obj == PM_SUPER_PILL)
    {
        cur_score += (new_obj == PM_PILL) ? PM_STD_PILL_POINTS : PM_SUP_PILL_POINTS;
        if (cur_score > sGame.stat_values.record)
        {
            sGame.prev_record = sGame.stat_values.record;
            sGame.stat_values.record = cur_score;
        }

        if (cur_score > 0 && cur_score % 1000 == 0 && IS_BETWEEN_EQ(sGame.stat_values.lives, 1, MAX_LIVES - 1))
            sGame.stat_values.lives++;

        // Incrementing the score variable
        sGame.stat_values.score = cur_score;

        if (++sGame.stat_values.pills_eaten == (PM_SUP_PILL_COUNT + PM_STD_PILL_COUNT))
            pGameVictory();
    }

    // Finally, moving PacMan.
    LCD_RQMoveObject(sGame.pacman.id, pMazeCellToCoords(new, ANC_CENTER), false);
    copied_maze[pacman.row][pacman.col] = PM_NONE;
    copied_maze[new.row][new.col] = PM_PCMN;
    sGame.pacman.cell = new;
}

_PRIVATE _CBACK void pStatsUpdater(void)
{
    // Not updating anything if game is not playing or PacMan is still.
    if (!sGame.playing_now || sGame.pacman.dir == PM_MOV_NONE)
        return;

    LCD_OBJECT_UPDATE(sGame.stat_obj_ids.score_record_values, false, {
        sprintf(sGame.stat_strings.record, "%d", sGame.stat_values.record);
        sprintf(sGame.stat_strings.score, "%d", sGame.stat_values.score);
    });

    // If lives incremented, need to make another icon visible.
    LCD_RQSetObjectVisibility(sGame.stat_obj_ids.lives[sGame.stat_values.lives - 1], true, false);
}

_PRIVATE _CBACK void pGameOverInCounter(void)
{
    // The GameOverIn counter counts down every second, not need to
    // update at the same frequency as the other stats. That's why I kept it separate.

    // Still need to count down if PacMan is moving, provided that
    // we're actually in the playing state.
    if (!sGame.playing_now)
        return;

    if (sGame.stat_values.game_over_in == 0 &&
        (sGame.stat_values.pills_eaten < (PM_STD_PILL_COUNT + PM_SUP_PILL_COUNT)))
        pGameDefeat();
    else
    {
        // Simply update the GameOverIn counter string to the new (decremented) value.
        sGame.stat_values.game_over_in--;

        // clang-format off
        LCD_OBJECT_UPDATE(sGame.stat_obj_ids.game_over_in_value, false, {
            sprintf(sGame.stat_strings.game_over_in, "%d", sGame.stat_values.game_over_in);
        });
        // clang-format on

        // Checking if there's a super pill to spawn at the given time.
        PM_SuperPill *pill;
        for (u8 i = 0; i < PM_SUP_PILL_COUNT; i++)
        {
            pill = &(sGame.super_pills[i]);
            if (pill->spawn_sec == sGame.stat_values.game_over_in)
            {
                LCD_RQSetObjectVisibility(pill->id, true, false);
                copied_maze[pill->cell.row][pill->cell.col] = PM_SUPER_PILL;
            }
        }
    }
}

_PRIVATE _CBACK void pPauseResumeGame(void)
{
    // Toggling between playing or paused views. The pause view
    // is located at the same place as the stats, so we don't need to delete/draw
    // the maze (it is always visible, just not controllable during pause).
    sGame.playing_now = !sGame.playing_now;
    sGame.pacman.dir = PM_MOV_NONE;

    if (sGame.playing_now)
    {
        LCD_RQSetObjectVisibility(sPauseID, false, false);
        JOYSTICK_EnableAction(JOY_ACTION_ALL);
    }

    LCD_RQSetObjectVisibility(sGame.stat_obj_ids.titles, sGame.playing_now, false);
    LCD_RQSetObjectVisibility(sGame.stat_obj_ids.score_record_values, sGame.playing_now, false);
    LCD_RQSetObjectVisibility(sGame.stat_obj_ids.game_over_in_value, sGame.playing_now, false);

    // Handling lives
    for (u8 i = 0; i < sGame.stat_values.lives; i++)
        LCD_RQSetObjectVisibility(sGame.stat_obj_ids.lives[i], sGame.playing_now, false);

    // If paused, we set the pause view visible after we've done hiding the playing views,
    // otherwise we would have overlapping.
    if (!sGame.playing_now)
    {
        LCD_RQSetObjectVisibility(sPauseID, true, false);
        JOYSTICK_DisableAction(JOY_ACTION_ALL);
    }
}

// MOVEMENT CALLBACKS

// clang-format off
_PRIVATE _CBACK void pMoveUp(void) { sGame.pacman.dir = PM_MOV_UP; }
_PRIVATE _CBACK void pMoveDown(void) { sGame.pacman.dir = PM_MOV_DOWN; }
_PRIVATE _CBACK void pMoveLeft(void) { sGame.pacman.dir = PM_MOV_LEFT; }
_PRIVATE _CBACK void pMoveRight(void) { sGame.pacman.dir = PM_MOV_RIGHT; }
// clang-format on

// CALLBACK CONTROL

_PRIVATE void pBindCallbacks(void)
{
    RIT_EnableJob(pRenderLoop);
    RIT_EnableJob(pGameOverInCounter);
    RIT_EnableJob(pStatsUpdater);

    JOYSTICK_EnableAction(JOY_ACTION_ALL);
    BUTTON_EnableSource(BTN_SRC_EINT0, 1);
}

_PRIVATE void pUnbindCallbacks(void)
{
    RIT_DisableJob(pRenderLoop);
    RIT_DisableJob(pGameOverInCounter);
    RIT_DisableJob(pStatsUpdater);

    JOYSTICK_DisableAction(JOY_ACTION_ALL);
    BUTTON_DisableSource(BTN_SRC_EINT0);
}

// GAMEPLAY CONTROL

/**
 * @brief Initializes the system for a new game.
 */
void pDoPlay(void)
{
    sGame.stat_values = (PM_GameStatValues){
        .pills_eaten = 0,
        .game_over_in = 60,
        .lives = 1,
        .record = 0,
        .score = 0,
    };

    // Copying the maze into a new array, so we can modify it without affecting the original.
    memcpy(copied_maze, PM_Maze, sizeof(PM_Maze));

    LCD_RQClear();
    pInitInfoView();
    pInitPauseView();
    pInitSuperPills();
    pDrawMaze();

    // Determining the initial PacMan position
    for (u16 row = 0; row < PM_MAZE_SCALED_HEIGHT; row++)
    {
        for (u16 col = 0; col < PM_MAZE_SCALED_WIDTH; col++)
        {
            // Always starting from the original position.
            if (PM_Maze[row][col] == PM_PCMN)
            {
                sGame.pacman.cell = (PM_MazeCell){row, col};
                break;
            }
        }
    }

    // Enabling controls & RIT
    pBindCallbacks();

    // Playing.
    sGame.playing_now = true;
}

_PRIVATE void pGameVictory(void)
{
    LCD_SetBackgroundColor(LCD_COL_BLACK, false);

    // Disabling controls & stopping the game
    pUnbindCallbacks();

    sGame.playing_now = false;
    sGame.pacman.dir = PM_MOV_NONE;

    // Showing a victory view.
    const LCD_Coordinate image_pos = {
        .x = LCD_GetWidth() / 2 - (Image_PACMAN_Victory.width / 2),
        .y = LCD_GetHeight() / 2 - (Image_PACMAN_Victory.height / 2) - 50,
    };

    char score_str[10];
    sprintf(score_str, "SCORE: %d", sGame.stat_values.score);
    const LCD_Coordinate score_pos = {LCD_GetWidth() / 2 - 50, LCD_GetHeight() / 2 + 50};

    // clang-format off
    LCD_RENDER_IMM({
        LCD_IMAGE(image_pos, Image_PACMAN_Victory), 
        LCD_TEXT(score_pos, {
            .text = score_str, .font = sFont20, .char_spacing = 2,
            .text_color = sColorOrange, .bg_color = LCD_COL_NONE,
        }),
    });
    // clang-format on

    // If a new record has been reached, showing the new record message.
    const bool new_record = sGame.stat_values.record > sGame.prev_record;
    if (new_record)
    {
        char new_record_str[30];
        sGame.prev_record = sGame.stat_values.record;
        sprintf(new_record_str, "NEW RECORD: %d", sGame.prev_record);

        // clang-format off
        LCD_RENDER_IMM({
            LCD_TEXT2(score_pos.x - 40, score_pos.y + 20, {
                .text = new_record_str, .font = sFont20, .char_spacing = 2,
                .text_color = sColorOrange, .bg_color = LCD_COL_NONE,
            }),
        });
        // clang-format on
    }

    // Showing a touch button for playing again.
    TP_ButtonArea play_again_btn;

    // clang-format off
    LCD_RENDER_IMM({
        LCD_BUTTON2(score_pos.x - 20, score_pos.y + (new_record ? 40 : 20), play_again_btn, {
            .label = LCD_BUTTON_LABEL({
                .text = "NEW GAME?", .font = sFont20,
                .char_spacing = 2, .text_color = LCD_COL_BLACK,
            }),
            .padding = {4, 5, 4, 5},
            .fill_color = sColorOrange,
        }),
    });
    // clang-format on

    TP_WaitForButtonPress(play_again_btn);
    pDoPlay(); // The user clicked!
}

_PRIVATE void pGameDefeat(void)
{
    LCD_SetBackgroundColor(LCD_COL_BLACK, false);

    // Disabling controls & stopping the game
    pUnbindCallbacks();

    sGame.playing_now = false;
    sGame.pacman.dir = PM_MOV_NONE;

    // Showing a defeat view.
    char score_str[10];
    sprintf(score_str, "SCORE: %d", sGame.stat_values.score);

    // Showing a victory view.
    const LCD_Coordinate image_pos = {
        .x = LCD_GetWidth() / 2 - (Image_PACMAN_Sad.width / 2),
        .y = LCD_GetHeight() / 2 - (Image_PACMAN_Victory.height / 2) - 50,
    };

    const LCD_Coordinate you_lost_pos = {LCD_GetWidth() / 2 - 60, LCD_GetHeight() / 2};
    const LCD_Coordinate score_pos = {LCD_GetWidth() / 2 - 50, LCD_GetHeight() / 2 + 15};

    // clang-format off
    LCD_RENDER_IMM({
        LCD_IMAGE(image_pos, Image_PACMAN_Sad),
        LCD_TEXT(you_lost_pos, {
            .text = "YOU LOST!", .font = sFont20, .char_spacing = 2,
            .text_color = LCD_COL_WHITE, .bg_color = LCD_COL_NONE,
        }),
        LCD_TEXT(score_pos, {
            .text = score_str, .font = sFont14, .char_spacing = 2,
            .text_color = LCD_COL_WHITE, .bg_color = LCD_COL_NONE,
        }),
    });
    // clang-format on

    // Showing a touch button for playing again.
    TP_ButtonArea play_again_btn;

    // clang-format off
    LCD_RENDER_IMM({
        LCD_BUTTON2(score_pos.x - 20, score_pos.y + 25, play_again_btn, {
            .label = LCD_BUTTON_LABEL({
                .text = "NEW GAME?", .font = sFont20,
                .char_spacing = 2, .text_color = LCD_COL_BLACK,
            }),
            .padding = {4, 5, 4, 5},
            .fill_color = sColorOrange,
        }),
    });
    // clang-format on

    TP_WaitForButtonPress(play_again_btn);
    pDoPlay(); // The user clicked!
}

// PUBLIC FUNCTIONS

void PACMAN_Init(void)
{
    // Initializing a memory arena for the GLCD object that will be rendered, which
    // uses static memory allocated ad hoc.
    const LCD_MemoryArena *const lcd_arena = LCD_MAUseMemory(sMemory, sizeof(sMemory));
    if (!lcd_arena)
        return;

    // Initializing the GLCD with the arena we just allocated.
    if (LCD_Init(LCD_ORIENT_VER, lcd_arena, NULL) != LCD_ERR_OK)
        return;

    // Initializing the TouchPanel, and starting its calibration phase.
    TP_Init(false);

    // Initializing the RIT to 50ms, with a very high priority, since
    // it will be heavily used by the game.
    RIT_Init(50, 1);
    RIT_Enable();

    JOYSTICK_Init();
    BUTTON_Init(BTN_DEBOUNCE_WITH_RIT);

    LCD_FMAddFont(Font_Upheaval14, &sFont14);
    LCD_FMAddFont(Font_Upheaval20, &sFont20);

    sGame.maze_pos = (LCD_Coordinate){
        .x = (LCD_GetWidth() - PM_MAZE_PIXEL_WIDTH) / 2,
        .y = LCD_GetHeight() - PM_MAZE_PIXEL_HEIGHT - 35,
    };

    // The previous record is maintained till the board is reset.
    sGame.prev_record = 0;
}

void PACMAN_Play(PM_Speed speed)
{
    // Setting the game speed.
    sGame.pacman.speed = speed;

    // Displaying the splash screen and waiting for user input.
    const LCD_Coordinate logo_pos = {
        .x = (LCD_GetWidth() - Image_PACMAN_Logo.width) / 2,
        .y = (LCD_GetHeight() - Image_PACMAN_Logo.height) / 2 - 30,
    };

    const LCD_Coordinate button_pos = {LCD_GetWidth() / 2 - 35, logo_pos.y + Image_PACMAN_Logo.height + 10};

    TP_ButtonArea button_tp;
    // clang-format off
    LCD_RENDER_IMM({
        LCD_IMAGE(logo_pos, Image_PACMAN_Logo),
        LCD_BUTTON(button_pos, button_tp, {
            .label = LCD_BUTTON_LABEL({.text = "START", .font = sFont20, .text_color = LCD_COL_BLACK}),
            .padding = {4, 5, 4, 5}, .fill_color = sColorOrange, .edge_color = sColorOrange,
        }),
    });
    // clang-format on
    TP_WaitForButtonPress(button_tp);
    LCD_SetBackgroundColor(LCD_COL_BLACK, false);

    // Adding jobs to RIT.
    RIT_AddJob(pRenderLoop, speed);
    RIT_AddJob(pGameOverInCounter, 1000 / RIT_GetIntervalMs());
    RIT_AddJob(pStatsUpdater, 500 / RIT_GetIntervalMs());

    // Setting up joystick controls
    JOYSTICK_SetFunction(JOY_ACTION_UP, pMoveUp);
    JOYSTICK_SetFunction(JOY_ACTION_DOWN, pMoveDown);
    JOYSTICK_SetFunction(JOY_ACTION_LEFT, pMoveLeft);
    JOYSTICK_SetFunction(JOY_ACTION_RIGHT, pMoveRight);

    // Setting up pause button
    BUTTON_SetFunction(BTN_SRC_EINT0, pPauseResumeGame);

    // If here, user clicked on START.
    pDoPlay();
}