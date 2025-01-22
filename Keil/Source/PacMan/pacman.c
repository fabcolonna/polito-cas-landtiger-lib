#include "pacman.h"
#include "allocator.h"
#include "peripherals.h"

#include "cl_list.h"
#include "cl_prioqueue.h"

#include <stdio.h>

// Assets
#include "Assets/Fonts/font-upheaval14.h"
#include "Assets/Fonts/font-upheaval20.h"

// Images
#include "Assets/Images/pacman-life.h"
#include "Assets/Images/pacman-logo.h"
#include "Assets/Images/pacman-maze.h"
#include "Assets/Images/pacman-sad.h"
#include "Assets/Images/pacman-victory.h"

// Ghosts
#include "Assets/Images/Ghosts/pacman-red-ghost.h"
#include "Assets/Images/Ghosts/pacman-yellow-ghost.h"

// PRIVATE TYPES

// clang-format off
enum Anchor { ANC_TOP_LEFT, ANC_CENTER };

// STATE VARIABLES

_PRIVATE PM_Game sGame;
_PRIVATE LCD_ObjID sPauseID;
_PRIVATE LCD_FontID sFont14, sFont20;
_PRIVATE PM_MazeObj sCurrentMaze[PM_MAZE_SCALED_HEIGHT][PM_MAZE_SCALED_WIDTH];

// Memory manager for the game.
_PRIVATE MEM_Allocator *sAllocator = NULL;
_PRIVATE _MEM_POOL_ALIGN4(sMemoryPool, 20480);

// OTHER VARIABLES

_PRIVATE const LCD_Color sColorOrange = RGB8_TO_RGB565(0xe27417);

// PROTOTYPES

_PROTOTYPE_ONLY _PRIVATE void do_play(void);
_PROTOTYPE_ONLY _PRIVATE void game_victory(void);
_PROTOTYPE_ONLY _PRIVATE void game_defeat(void);

// UTILS

_PRIVATE inline LCD_Coordinate maze_cell_to_coords(PM_MazeCell cell, enum Anchor anchor)
{
    return (LCD_Coordinate){
        .x = sGame.maze_pos.x + (cell.col * PM_MAZE_CELL_SIZE) + (anchor == ANC_CENTER ? PM_MAZE_CELL_SIZE / 2 : 0),
        .y = sGame.maze_pos.y + (cell.row * PM_MAZE_CELL_SIZE) + (anchor == ANC_CENTER ? PM_MAZE_CELL_SIZE / 2 : 0),
    };
}

_PRIVATE inline LCD_Coordinate maze_cell_to_coords_img(PM_MazeCell cell, const LCD_Image *const image, enum Anchor anchor)
{
    return (LCD_Coordinate){
        .x = sGame.maze_pos.x + (cell.col * PM_MAZE_CELL_SIZE) + (anchor == ANC_CENTER ? PM_MAZE_CELL_SIZE / 2 - image->width / 2 : 0),
        .y = sGame.maze_pos.y + (cell.row * PM_MAZE_CELL_SIZE) + (anchor == ANC_CENTER ? PM_MAZE_CELL_SIZE / 2 - image->width / 2 : 0),
    };
}

// VIEW DEFINITIONS

_PRIVATE void init_info(void)
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

_PRIVATE void init_pause(void)
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

_PRIVATE void draw_maze(void)
{
    PM_MazeObj obj;
    PM_MazeCell obj_cell;
    for (u16 row = 0; row < PM_MAZE_SCALED_HEIGHT; row++)
    {
        for (u16 col = 0; col < PM_MAZE_SCALED_WIDTH; col++)
        {
            obj = sCurrentMaze[row][col];
            obj_cell = (PM_MazeCell){row, col};

            // clang-format off
            switch (obj)
            {
            case PM_WALL:
                LCD_RENDER_IMM({
                    LCD_RECT(maze_cell_to_coords(obj_cell, ANC_TOP_LEFT), {
                        .width = PM_MAZE_CELL_SIZE, .height = PM_MAZE_CELL_SIZE,
                        .fill_color = PM_WALL_COLOR, .edge_color = PM_WALL_COLOR,
                    }),
                });
                break;
            case PM_PILL:
                LCD_OBJECT(&sGame.pill_ids[obj_cell.row][obj_cell.col],{
                    LCD_CIRCLE({
                        .center = maze_cell_to_coords(obj_cell, ANC_CENTER),
                        .fill_color = PM_STD_PILL_COLOR,
                        .edge_color = PM_STD_PILL_COLOR,
                        .radius = PM_STD_PILL_RADIUS,
                    }),
                });
                break;
            case PM_PCMN:
                LCD_OBJECT(&sGame.pacman.id, {
                    LCD_CIRCLE({
                        .center = maze_cell_to_coords(obj_cell, ANC_CENTER), .radius = PM_PACMAN_RADIUS,
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

_PRIVATE inline bool super_pill_spawn_second_already_taken(u32 sec, u16 super_pills_arr_length)
{
    for (u8 i = 0; i < super_pills_arr_length; i++)
        if (sGame.super_pills[i].spawn_sec == sec)
            return true;

    return false;
}

_PRIVATE void init_super_pills(void)
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
        if (sCurrentMaze[row][col] != PM_PILL)
            continue;

        sGame.super_pills[i].cell = (PM_MazeCell){row, col};

        // clang-format off
        LCD_MANUAL_VISIBILITY_OBJECT(&sGame.super_pills[i].id, {
            LCD_CIRCLE({
                .center = maze_cell_to_coords(sGame.super_pills[i].cell, ANC_CENTER),
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
        } while (super_pill_spawn_second_already_taken(spawn_sec, i));

        sGame.super_pills[i].spawn_sec = spawn_sec;
        i++;
    }

    PRNG_Release();
}

// GHOSTS

typedef struct AStarNode
{
    PM_MazeCell cell;
    // f = g + h
    // g = cost to reach the cell from the start
    // h = heuristic function (Manhattan distance in this case)
    u16 f, g, h;
    struct AStarNode *parent; // Used to reconstruct the path
} AStarNode;

// Comparison function for the priority queue in the A* algorithm.
_COMPARE_FN(compare_f_values, a, b, {
    // a and b point to pointers to AStarNode
    const AStarNode *nodeA = *(const AStarNode **)a;
    const AStarNode *nodeB = *(const AStarNode **)b;
    return (int)nodeA->f - (int)nodeB->f;
});

// Using the Manhattan distance as the heuristic function for the A* algorithm.
// The Manhattan distance is the sum of the absolute differences of the x and y
// coordinates of the two points. It is a good heuristic for this game, since
// the ghosts can only move in 4 directions, and the maze is a grid.
_PRIVATE inline u16 heuristic(PM_MazeCell a, PM_MazeCell b)
{
    return abs(a.row - b.row) + abs(a.col - b.col);
}

// Generates the neighbors of a cell in the maze, paying attention to the maze boundaries (e.g. walls).
_PRIVATE void neighbors_finder(PM_MazeCell cell, PM_MazeCell *out_neighbors_arr, u8 *out_neighbors_sz)
{
    PM_MazeCell neighbors[] = {
        {cell.row - 1, cell.col}, // Up
        {cell.row + 1, cell.col}, // Down
        {cell.row, cell.col - 1}, // Left
        {cell.row, cell.col + 1}, // Right
    };

    // Ensuring the neighbors are within the maze boundaries and not walls.
    *out_neighbors_sz = 0;
    PM_MazeCell *neighbor;
    PM_MazeObj obj;
    for (u8 i = 0; i < 4; i++)
    {
        neighbor = &neighbors[i];

        // If the row or column is out of bounds, we skip this neighbor.
        if (!IS_BETWEEN_EQ(neighbor->row, 0, PM_MAZE_SCALED_HEIGHT - 1) ||
            !IS_BETWEEN_EQ(neighbor->col, 0, PM_MAZE_SCALED_WIDTH - 1))
            continue;

        // If the cell is a pill, a super pill, or none, we can move there.
        obj = sCurrentMaze[neighbor->row][neighbor->col];
        if (obj == PM_WALL || obj == PM_LTPL || obj == PM_RTPL)
            continue; // Walls and teleporters are not valid neighbors.

        out_neighbors_arr[*out_neighbors_sz] = *neighbor;
        (*out_neighbors_sz)++;
    }
}

// Once we dequeue the goal cell from the open set, we can reconstruct the path
// to the star cell by backtracking using the parent pointers of each node.
_PRIVATE CL_List *reconstruct_path(AStarNode *const goal)
{
    // Reconstructing the path from the goal cell to the start cell.
    CL_List *const path_list = CL_ListAlloc(sAllocator, sizeof(PM_MazeCell));
    if (!path_list)
        return NULL;

    AStarNode *current_node = goal;
    while (current_node)
    {
        CL_ListPushFront(path_list, &(current_node->cell));
        current_node = current_node->parent;
    }

    return path_list;
}

/*
Using the A* algorithm to find the shortest path from the ghost to the goal cell. A* selects the path
that minimizes the cost function f = g + h, where g is the cost to reach the cell from the start,
and h is the heuristic function (in this case, the Manhattan distance), that estimates the cost
to reach the goal from the current cell.

The algorithm uses two lists:
    - Open list: Contains the cell that  need to be evaluated. Initially, it only contains the start cell.
    - Closed list: Contains the cells that have already been evaluated. Initially, it is empty.

The open list is implemented as a priority queue, where the cell having the smallest f value has the
highest priority, thus it will be dequeued first. The algorithm finishes when:
    - The open list is empty
    - The goal cell is added to the closed list.

If the former happens (and not the latter), it means that there's no path to the goal cell from the
starting point; if the latter happens, the path has been found, and we can backtrack from the goal
cell to the start cell to reconstruct the path.
*/
_PRIVATE CL_List *find_path_to_goal(PM_MazeCell start, PM_MazeCell goal)
{
    CL_PQueue *const open_set = CL_PQueueAlloc(sAllocator, sizeof(AStarNode *), compare_f_values);
    if (!open_set)
        return NULL;

    // Visited cells array to keep track of the cells that have already been evaluated.
    static bool visited_cells[PM_MAZE_SCALED_HEIGHT][PM_MAZE_SCALED_WIDTH];
    memset(visited_cells, 0, sizeof(visited_cells));

    // Array of AStarNodes for each cell in the maze.
    static AStarNode nodes[PM_MAZE_SCALED_HEIGHT][PM_MAZE_SCALED_WIDTH];
    memset(nodes, 0, sizeof(nodes));

    // Filling the start node with the initial values.
    const u16 heuristic_value = heuristic(start, goal); // Heuristic function (Manhattan distance)
    AStarNode *const start_node = &nodes[start.row][start.col];
    *start_node = (AStarNode){start, heuristic_value, 0, heuristic_value, NULL};
    CL_PQueueEnqueue(open_set, &start_node);

    // Iterating while the open set is not empty.
    AStarNode *cur_node, *neigh_node;
    PM_MazeCell neighs[4], *neigh_cell;
    u8 neigh_count;
    u16 new_g;
    bool not_visited;
    while (!CL_PQueueIsEmpty(open_set))
    {
        // Retrieving the cell with the lowest f value from the open set.
        CL_PQueueDequeue(open_set, &cur_node);

        // Checking if the current cell is in the closed set, meaning that it has been evaluated.
        if (visited_cells[cur_node->cell.row][cur_node->cell.col])
            continue;

        // If not the goal, add the current cell to the closed set.
        visited_cells[cur_node->cell.row][cur_node->cell.col] = true;

        // Checking if the current cell is the goal cell.
        if (cur_node->cell.row == goal.row && cur_node->cell.col == goal.col)
        {
            CL_List *const path = reconstruct_path(cur_node);
            CL_PQueueFree(open_set);
            return path;
        }

        // If the current cell is not the goal cell, we need to find its neighbors.
        neighbors_finder(cur_node->cell, neighs, &neigh_count);
        for (u8 i = 0; i < neigh_count; i++)
        {
            neigh_cell = &neighs[i];

            // Checking if the neighbor is in the closed set
            if (visited_cells[neigh_cell->row][neigh_cell->col])
                continue;

            new_g = cur_node->g + 1; // The cost to move from one cell to its neighbor is always 1.

            // If the neighbor has g = 0 and it's not the start cell, OR the new g < current g => update.
            neigh_node = &nodes[neigh_cell->row][neigh_cell->col];
            not_visited = !neigh_node->g && (neigh_node->cell.row != start.row || neigh_node->cell.col != start.col);
            if (not_visited || new_g < neigh_node->g)
            {
                neigh_node->cell = *neigh_cell;
                neigh_node->g = new_g;
                neigh_node->h = heuristic(*neigh_cell, goal);
                neigh_node->f = new_g + neigh_node->h;
                neigh_node->parent = cur_node;
                CL_PQueueEnqueue(open_set, &neigh_node);
            }
        }
    }

    CL_PQueueFree(open_set);
    return NULL;
}

_PRIVATE void init_ghost(bool scared)
{
    PM_MazeObj obj;
    PM_MazeCell cell;
    LCD_Coordinate coords;
    for (u8 i = 0; i < PM_MAZE_SCALED_HEIGHT; i++)
    {
        for (u8 j = 0; j < PM_MAZE_SCALED_WIDTH; j++)
        {
            obj = sCurrentMaze[i][j];
            if (obj != PM_GHSR)
                continue;

            cell = (PM_MazeCell){i, j};
            sGame.ghost.cell = cell;

            // clang-format off
            const LCD_Image img = scared ? Image_PACMAN_YellowGhost : Image_PACMAN_RedGhost;
            coords = maze_cell_to_coords_img(cell, &img, ANC_CENTER);
            LCD_OBJECT(&sGame.ghost.id, {
                LCD_IMAGE(coords, img),
            });
            // clang-format on

            return;
        }
    }
}

_PRIVATE PM_MazeCell find_farthest_cell_from_pacman(PM_MazeCell pacman)
{
    PM_MazeCell farthest_cell;
    u16 record = 0, distance;
    for (u16 i = 0; i < PM_MAZE_SCALED_HEIGHT; i++)
    {
        for (u16 j = 0; j < PM_MAZE_SCALED_WIDTH; j++)
        {
            if (sCurrentMaze[i][j] == PM_WALL || sCurrentMaze[i][j] == PM_LTPL || sCurrentMaze[i][j] == PM_RTPL)
                continue;

            distance = abs(i - pacman.row) + abs(j - pacman.col);
            if (distance > record)
            {
                record = distance;
                farthest_cell = (PM_MazeCell){i, j};
            }
        }
    }

    return farthest_cell;
}

// GHOST CALLBACKS

_PRIVATE _CBACK void red_ghost_ai(void)
{
    // If the game is not playing, we don't need to move the ghost.
    if (!sGame.playing_now)
        return;

    PM_Ghost *const ghost = &sGame.ghost;

    // If the path is not valid, we need to recalculate it.
    if (!ghost->path.is_valid || !ghost->path.path || CL_ListSize(ghost->path.path) < 5)
    {
        if (ghost->path.path)
            CL_ListFree(ghost->path.path);

        // Recalculating the path to PacMan, or to the farthest cell from Pacman if he's scared.
        PM_MazeCell goal =
            sGame.ghost.is_scared ? find_farthest_cell_from_pacman(sGame.pacman.cell) : sGame.pacman.cell;

        ghost->path.path = find_path_to_goal(ghost->cell, goal);
        ghost->path.is_valid = (ghost->path.path && !CL_ListIsEmpty(ghost->path.path));
    }

    if (ghost->path.is_valid && !CL_ListIsEmpty(ghost->path.path)) // Moving the ghost along the path.
    {
        PM_MazeCell next_cell;
        CL_ListPopFront(ghost->path.path, &next_cell);
        LCD_RQMoveObject(ghost->id, maze_cell_to_coords_img(next_cell, &Image_PACMAN_RedGhost, ANC_CENTER), true);

        // Need to restore the previous cell to its original state and update the current cell.
        // Checking if at the actual ghost position there is a pill or a super pill.
        if (sGame.pill_ids[ghost->cell.row][ghost->cell.col] > 0)
            sCurrentMaze[ghost->cell.row][ghost->cell.col] = PM_PILL;
        else
        {
            // Checking for a super pill.
            bool super_pill = false;
            for (u8 i = 0; i < PM_SUP_PILL_COUNT; i++)
            {
                if (sGame.super_pills[i].cell.row == ghost->cell.row &&
                    sGame.super_pills[i].cell.col == ghost->cell.col)
                {
                    sCurrentMaze[ghost->cell.row][ghost->cell.col] = PM_SUPER_PILL;
                    super_pill = true;
                    break;
                }
            }

            // If there is no super pill, the cell is empty.
            if (!super_pill)
                sCurrentMaze[ghost->cell.row][ghost->cell.col] = PM_NONE;
        }

        sCurrentMaze[ghost->cell.row][ghost->cell.col] = PM_NONE;
        sCurrentMaze[next_cell.row][next_cell.col] = PM_GHSR;
        ghost->cell = next_cell;
    }
}

_PRIVATE _CBACK void ghost_path_invalidator(void)
{
    // If PACMAN is still, we don't need to invalidate the path.
    if (sGame.pacman.dir == PM_MOV_NONE)
        return;

    // If the game is not playing, we invalidate the path so that the ghost won't move
    if (!sGame.playing_now)
    {
        sGame.ghost.path.is_valid = false;
        return;
    }
}

_PRIVATE _CBACK void ghost_scared_counter(void)
{
    if (!sGame.playing_now || !sGame.ghost.is_scared)
        return;

    // If the ghost is scared, we need to decrement the counter.
    if (sGame.ghost.scared_counter < PM_GHOST_SCARED_DURATION)
        sGame.ghost.scared_counter++;

    // If the counter reaches 0, the ghost is no longer scared.
    if (sGame.ghost.scared_counter == PM_GHOST_SCARED_DURATION)
    {
        sGame.ghost.is_scared = false;
        sGame.ghost.scared_counter = 0;

        // Reinitializing the ghost to its original color.
        LCD_RQRemoveObject(sGame.ghost.id, false);
        init_ghost(false);

        // Disabling the scared counter job.
        RIT_DisableJob(ghost_scared_counter);
    }
}

// CALLBACKS

_PRIVATE _CBACK void render_loop(void)
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
        if (pacman.row > 0 && sCurrentMaze[pacman.row - 1][pacman.col] != PM_WALL)
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
        if (pacman.row < PM_MAZE_SCALED_HEIGHT - 1 && sCurrentMaze[pacman.row + 1][pacman.col] != PM_WALL)
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
        if (pacman.col > 0 && sCurrentMaze[pacman.row][pacman.col - 1] != PM_WALL)
        {
            new.row = pacman.row;
            // If it's a teleport, jump to the right one.
            new.col =
                (sCurrentMaze[pacman.row][pacman.col - 1] == PM_LTPL) ? (PM_MAZE_SCALED_WIDTH - 2) : pacman.col - 1;
            break;
        }
        else
        {
            sGame.pacman.dir = PM_MOV_NONE;
            return;
        }
    case PM_MOV_RIGHT:
        if (pacman.col < PM_MAZE_SCALED_WIDTH - 1 && sCurrentMaze[pacman.row][pacman.col + 1] != PM_WALL)
        {
            new.row = pacman.row;
            // If it's a teleport, jump to the left one.
            new.col = (sCurrentMaze[pacman.row][pacman.col + 1] == PM_RTPL) ? 1 : pacman.col + 1;
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
    const PM_MazeObj new_obj = sCurrentMaze[new.row][new.col];
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
            game_victory();

        if (new_obj == PM_PILL)
        {
            // Removing the pill from the screen, and setting its ID to -1 in the array.
            LCD_RQRemoveObject(sGame.pill_ids[new.row][new.col], false);
            sGame.pill_ids[new.row][new.col] = -1;
        }
        // If we ate a super pill, we need to scare the ghost.
        else if (new_obj == PM_SUPER_PILL)
        {
            // Removing it from the screen, and setting its ID to -1 in the array.
            for (u8 i = 0; i < PM_SUP_PILL_COUNT; i++)
            {
                if (sGame.super_pills[i].cell.row == new.row && sGame.super_pills[i].cell.col == new.col)
                {
                    LCD_RQRemoveObject(sGame.super_pills[i].id, false);
                    sGame.super_pills[i].id = -1;
                    break;
                }
            }

            sGame.ghost.is_scared = true;
            RIT_EnableJob(ghost_scared_counter);
            LCD_RQRemoveObject(sGame.ghost.id, false);
            init_ghost(true);
        }
    }

    // Finally, moving PacMan.
    LCD_RQMoveObject(sGame.pacman.id, maze_cell_to_coords(new, ANC_CENTER), false);
    sCurrentMaze[pacman.row][pacman.col] = PM_NONE;
    sCurrentMaze[new.row][new.col] = PM_PCMN;
    sGame.pacman.cell = new;
}

_PRIVATE _CBACK void stats_updater(void)
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

_PRIVATE _CBACK void game_over_counter(void)
{
    // The GameOverIn counter counts down every second, not need to
    // update at the same frequency as the other stats. That's why I kept it separate.

    // Still need to count down if PacMan is moving, provided that
    // we're actually in the playing state.
    if (!sGame.playing_now)
        return;

    if (sGame.stat_values.game_over_in == 0 &&
        (sGame.stat_values.pills_eaten < (PM_STD_PILL_COUNT + PM_SUP_PILL_COUNT)))
        game_defeat();
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
                sCurrentMaze[pill->cell.row][pill->cell.col] = PM_SUPER_PILL;
            }
        }

        // Every 20 seconds, we increase the speed of the ghosts by a little bit.
        if (IS_BETWEEN_EQ(sGame.stat_values.game_over_in, 1, 59) && sGame.stat_values.game_over_in % 20 == 0)
        {
            const u8 current_factor = RIT_GetJobMultiplierFactor(red_ghost_ai);
            RIT_SetJobMultiplierFactor(red_ghost_ai, current_factor - 1);
        }
    }
}

_PRIVATE _CBACK void pause_resume_game(void)
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
_PRIVATE _CBACK void move_up(void) { sGame.pacman.dir = PM_MOV_UP; }
_PRIVATE _CBACK void move_down(void) { sGame.pacman.dir = PM_MOV_DOWN; }
_PRIVATE _CBACK void move_left(void) { sGame.pacman.dir = PM_MOV_LEFT; }
_PRIVATE _CBACK void move_right(void) { sGame.pacman.dir = PM_MOV_RIGHT; }
// clang-format on

// CALLBACK CONTROL

_PRIVATE void bind_cbacks(void)
{
    RIT_EnableJob(render_loop);
    RIT_EnableJob(game_over_counter);
    RIT_EnableJob(stats_updater);

    RIT_EnableJob(red_ghost_ai);
    RIT_EnableJob(ghost_path_invalidator);

    JOYSTICK_EnableAction(JOY_ACTION_ALL);
    BUTTON_EnableSource(BTN_SRC_EINT0, 1);
}

_PRIVATE void unbind_cbacks(void)
{
    RIT_DisableJob(render_loop);
    RIT_DisableJob(game_over_counter);
    RIT_DisableJob(stats_updater);

    RIT_DisableJob(red_ghost_ai);
    RIT_DisableJob(ghost_path_invalidator);

    JOYSTICK_DisableAction(JOY_ACTION_ALL);
    BUTTON_DisableSource(BTN_SRC_EINT0);
}

// GAMEPLAY CONTROL

/**
 * @brief Initializes the system for a new game.
 */
void do_play(void)
{
    sGame.stat_values = (PM_GameStatValues){
        .pills_eaten = 0,
        .game_over_in = 60,
        .lives = 1,
        .record = 0,
        .score = 0,
    };

    // Copying the maze into a new array, so we can modify it without affecting the original.
    memcpy((void *)sCurrentMaze, PACMAN_BaseMaze, sizeof(PACMAN_BaseMaze));

    LCD_RQClear();
    init_info();
    init_pause();
    init_super_pills();
    draw_maze();
    init_ghost(false);

    // Determining the initial PacMan position
    for (u16 row = 0; row < PM_MAZE_SCALED_HEIGHT; row++)
    {
        for (u16 col = 0; col < PM_MAZE_SCALED_WIDTH; col++)
        {
            // Always starting from the original position.
            if (PACMAN_BaseMaze[row][col] == PM_PCMN)
            {
                sGame.pacman.cell = (PM_MazeCell){row, col};
                break;
            }
        }
    }

    // Enabling controls & RIT
    bind_cbacks();

    // Playing.
    sGame.playing_now = true;
}

_PRIVATE void game_victory(void)
{
    LCD_SetBackgroundColor(LCD_COL_BLACK, false);

    // Disabling controls & stopping the game
    unbind_cbacks();

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
    do_play(); // The user clicked!
}

_PRIVATE void game_defeat(void)
{
    LCD_SetBackgroundColor(LCD_COL_BLACK, false);

    // Disabling controls & stopping the game
    unbind_cbacks();

    sGame.playing_now = false;
    sGame.pacman.dir = PM_MOV_NONE;

    // Showing a defeat view.
    char score_str[10];
    sprintf(score_str, "SCORE: %d", sGame.stat_values.score);

    // Showing a victory view.
    const LCD_Coordinate image_pos = {
        .x = LCD_GetWidth() / 2 - (Image_PACMAN_Sad.width / 2),
        .y = LCD_GetHeight() / 2 - (Image_PACMAN_Sad.height / 2) - 50,
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
    do_play(); // The user clicked!
}

// PUBLIC FUNCTIONS

void PACMAN_Init(void)
{
    // Initializing the general purpose memory for the memory manager.
    sAllocator = MEM_Init(sMemoryPool, sizeof(sMemoryPool));
    if (!sAllocator)
        return;

    // Initializing the GLCD with the arena we just allocated.
    if (LCD_Init(LCD_ORIENT_VER, sAllocator, NULL) != LCD_ERR_OK)
        return;

    // Initializing the TouchPanel, and starting its calibration phase.
    TP_Init(false);

    // Initializing the RIT to 50ms, with a very high priority, since
    // it will be heavily used by the game.
    RIT_Init(sAllocator, 50, 1);
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
    RIT_AddJob(render_loop, speed);    // 50ms * speed(0|1|2) = 0|50|100ms update
    RIT_AddJob(game_over_counter, 20); // 50ms * 20 = 1sec update
    RIT_AddJob(stats_updater, 10);     // 50ms * 10 = 0.5sec update

    // Ghosts jobs
    RIT_AddJob(red_ghost_ai, speed + 2);   // Changes every 20secs // 50ms * (2|3|4) = 100|150|200ms update
    RIT_AddJob(ghost_path_invalidator, 5); // 50ms * 5 = 0.25sec update
    RIT_AddJob(ghost_scared_counter, 20);  // 50ms * 20 = 1sec update

    // Setting up joystick controls
    JOYSTICK_SetFunction(JOY_ACTION_UP, move_up);
    JOYSTICK_SetFunction(JOY_ACTION_DOWN, move_down);
    JOYSTICK_SetFunction(JOY_ACTION_LEFT, move_left);
    JOYSTICK_SetFunction(JOY_ACTION_RIGHT, move_right);

    // Setting up pause button
    BUTTON_SetFunction(BTN_SRC_EINT0, pause_resume_game);

    // If here, user clicked on START.
    do_play();
}