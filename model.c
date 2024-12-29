#include "model.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"


void Model_init(Model* self, int width, int height)
{
    self->height = height;
    self->width = width;
    self->players = NULL;
    self->num_players = 0;
    self->walls = NULL;
    self->num_walls = 0;
    self->state = GAME_STATE_MENU;
}

bool Model_add_player(Model* self, const int x, const int y, const int direction)
{
    // use allocated_player to resize the array
    int allocated_players = self->allocated_players;
    int to_allocate = (self->num_players + 1) + PLAYER_CHUNK - (self->num_players + 1) % PLAYER_CHUNK;
    if (allocated_players < to_allocate)
    {
        Player* tmp = realloc(self->players, to_allocate * sizeof(Player));
        if (tmp == NULL)
            return false;
        self->players = tmp;
        self->allocated_players = to_allocate;
    }

    self->players[self->num_players].x = x;
    self->players[self->num_players].y = y;
    self->players[self->num_players].direction = direction;
    self->players[self->num_players].score = 0;
    self->players[self->num_players].state = PLAYER_STATE_ALIVE;
    self->num_players++;
    return true;
}

bool Model_add_wall(Model* self, const int x, const int y, const int direction, const int length, const int player)
{
    // use allocated_walls to resize the array
    int allocated_walls = self->allocated_walls;
    int to_allocate = (self->num_walls + 1) + WALL_CHUNK - (self->num_walls + 1) % WALL_CHUNK;
    if (allocated_walls < to_allocate)
    {
        Wall* tmp = realloc(self->walls, to_allocate * sizeof(Wall));
        if (tmp == NULL)
            return false;
        self->walls = tmp;
        self->allocated_walls = to_allocate;
    }
    self->walls[self->num_walls].x = x;
    self->walls[self->num_walls].y = y;
    self->walls[self->num_walls].direction = direction;
    self->walls[self->num_walls].length = length;
    self->walls[self->num_walls].player = player;
    self->num_walls++;
    return true;
}

void Model_clear_walls(Model* self)
{
    free(self->walls);
    self->walls = NULL;
    self->num_walls = 0;
    self->allocated_walls = 0;
}

void Model_destroy(Model* self)
{
    free(self->players);
    free(self->walls);
    self->players = NULL;
    self->walls = NULL;
    self->num_players = 0;
    self->num_walls = 0;
    self->allocated_players = 0;
    self->allocated_walls = 0;
}

Direction Model_get_opposite_direction(const Direction direction)
{
    switch (direction)
    {
    case DIRECTION_UP:
        return DIRECTION_DOWN;
    case DIRECTION_DOWN:
        return DIRECTION_UP;
    case DIRECTION_LEFT:
        return DIRECTION_RIGHT;
    case DIRECTION_RIGHT:
        return DIRECTION_LEFT;
    }
    return DIRECTION_UP;
}

bool Model_hit_wall(const Wall* wall, const int x, const int y)
{
    switch (wall->direction)
    {
    case DIRECTION_UP:
        return wall->x == x && wall->y <= y && y < wall->y + wall->length;
    case DIRECTION_DOWN:
        return wall->x == x && wall->y - wall->length < y && y <= wall->y;
    case DIRECTION_LEFT:
        return wall->y == y && wall->x <= x && x < wall->x + wall->length;
    case DIRECTION_RIGHT:
        return wall->y == y && wall->x - wall->length < x && x <= wall->x;
    default:
        return false;
    }
}

bool Model_try_hit_walls(const Model* model, const int x, const int y, Wall* output)
{
    for (int i = 0; i < model->num_walls; i++)
        if (Model_hit_wall(&model->walls[i], x, y))
        {
            *output = model->walls[i];
            return true;
        }
    return false;
}

bool Model_hit_player(const Player* player, const int x, const int y)
{
    return player->x == x && player->y == y;
}

bool Model_try_hit_players(const Model* model, const int index, int* output)
{
    const Player* player = &model->players[index];
    if (player->state != PLAYER_STATE_ALIVE) return false;
    for (int i = 0; i < model->num_players; i++)
    {
        if (index == i) continue;
        const Player* other = &model->players[i];
        if (other->state != PLAYER_STATE_ALIVE) continue;
        if (Model_hit_player(other, player->x, player->y))
        {
            *output = i;
            return true;
        }
    }
    return false;
}

bool Model_out_of_bounds(const Model* model, const int x, const int y)
{
    return x < 0 || x >= model->width || y < 0 || y >= model->height;
}

bool Model_try_hit_raycast(const Model* model, const int x, const int y, const int direction, Wall* output,
                           int* distance)
{
    int dx = 0;
    int dy = 0;
    switch (direction)
    {
    case DIRECTION_UP:
        dy = -1;
        break;
    case DIRECTION_DOWN:
        dy = 1;
        break;
    case DIRECTION_LEFT:
        dx = -1;
        break;
    case DIRECTION_RIGHT:
        dx = 1;
        break;
    }
    int cx = x;
    int cy = y;

    if (cx < 0)
        cx = 0;
    if (cx >= model->width)
        cx = model->width - 1;
    if (cy < 0)
        cy = 0;
    if (cy >= model->height)
        cy = model->height - 1;

    while (!Model_out_of_bounds(model, cx, cy))
    {
        if (Model_try_hit_walls(model, cx, cy, output))
        {
            switch (direction)
            {
            case DIRECTION_UP:
                *distance = y - cy;
                break;
            case DIRECTION_DOWN:
                *distance = cy - y;
                break;
            case DIRECTION_LEFT:
                *distance = x - cx;
                break;
            case DIRECTION_RIGHT:
                *distance = cx - x;
                break;
            }
            return true;
        }
        cx += dx;
        cy += dy;
    }
    *distance = -1;
    return false;
}

bool Model_change_direction(Model* model, const int index, const int direction)
{
    Player* player = &model->players[index];
    // check if the direction is the same as the current one, if so, return true
    const Direction current = player->direction;
    if (direction == current)
        return true;

    // check if the direction is the opposite of the current one, if so, return false
    const Direction opposite = Model_get_opposite_direction(current);
    if (direction == opposite)
        return false;

    // add a wall in the direction of the player until it hits a wall
    Wall wall;
    int distance;
    if (Model_try_hit_raycast(model, player->x, player->y, opposite, &wall, &distance))
    {
        debug_logf("[ADD] Wall %d %d %d %d", wall.x, wall.y, wall.direction, wall.length);
        if (!Model_add_wall(model, player->x, player->y, current, distance, index))
            return false;
    }

    player->direction = direction;
    return true;
}

void Model_move_player(const Model* model, const int index, const int speed)
{
    Player* player = &model->players[index];
    int dx = 0;
    int dy = 0;

    switch (player->direction)
    {
    case DIRECTION_UP:
        dy = -speed;
        break;
    case DIRECTION_DOWN:
        dy = speed;
        break;
    case DIRECTION_LEFT:
        dx = -speed;
        break;
    case DIRECTION_RIGHT:
        dx = speed;
        break;
    default: ;
    }

    player->x += dx;
    player->y += dy;
}

void Model_place_player(const Model* self, Player* player, const int index)
{
    // 1. get the radius
    const int radius = self->width / 2 < self->height
                           ? self->width / 6
                           : self->height / 3;

    // 2. get the angle
    const double angle = (float)index / (float)self->num_players * 2.0 * M_PI;

    // 3. get the position
    player->x = (int)(self->width / 2.0 + cos(angle) * radius * 2.0);
    player->y = (int)(self->height / 2.0 + sin(angle) * radius);

    // 4. get the direction
    Direction d = (int)(angle / M_PI_2) % 4;

    switch (d)
    {
    case DIRECTION_LEFT:
        d = DIRECTION_LEFT;
        break;
    case DIRECTION_RIGHT:
        d = DIRECTION_UP;
        break;
    case DIRECTION_UP:
        d = DIRECTION_RIGHT;
        break;
    case DIRECTION_DOWN:
        d = DIRECTION_DOWN;
        break;
    }

    player->direction = d;
}

void Model_start(Model* self)
{
    const GameState state = self->state;
    if (state == GAME_STATE_PLAYING)
    {
        debug_log("Game already playing");
        return;
    }
    if (self->num_players == 0)
    {
        debug_log("No players");
        return;
    }

    // set the players with default values
    for (int i = 0; i < self->num_players; i++)
    {
        self->players[i].score = 0;
        self->players[i].state = PLAYER_STATE_ALIVE;
        Model_place_player(self, &self->players[i], i);
    }

    // set the walls with default values
    Model_clear_walls(self);
    debug_logf("Players: %d", self->num_players);
    debug_logf("Walls: %d", self->num_walls);

    for (int i = 0; i < self->num_players; i++)
        if (!Model_add_wall(self, self->players[i].x, self->players[i].y, self->players[i].direction, 1, i))
        {
            debug_log("Failed to add wall");
            return;
        }
    // set the state to playing
    self->state = GAME_STATE_PLAYING;
}

void Model_calculate_player_state(Model* self, const int index)
{
    // check if the player is out of bounds
    Player* player = &self->players[index];
    if (Model_out_of_bounds(self, player->x, player->y))
    {
        player->state = PLAYER_STATE_TO_DEATH;
        debug_logf("[DEATH] Player %d is out of bounds at %d %d", index, player->x, player->y);
        return;
    }

    // check if the player hit a wall
    Wall wall;
    if (Model_try_hit_walls(self, player->x, player->y, &wall))
    {
        player->state = PLAYER_STATE_TO_DEATH;
        debug_logf("[DEATH] Player %d hit a wall at %d %d", index, player->x, player->y);
        return;
    }

    // check if the player hit another player
    int output;
    if (Model_try_hit_players(self, index, &output))
    {
        Player* other = &self->players[output];
        player->state = PLAYER_STATE_TO_DEATH;
        other->state = PLAYER_STATE_TO_DEATH;
        debug_logf("[DEATH] Player %d hit player %d at %d %d", index, output, player->x, player->y);
        return;
    }

    // check if the player hit a player wall
    int distance;
    for (int i = 0; i < self->num_players; i++)
    {
        if (i == index) continue;
        Model_get_player_wall(self, i, &wall, &distance);
        const Player other = self->players[i];
        wall.direction = Model_get_opposite_direction(other.direction);
        wall.length = distance;
        if (distance > 0 && Model_hit_wall(&wall, player->x, player->y))
        {
            player->state = PLAYER_STATE_TO_DEATH;
            debug_logf("[DEATH] Player %d hit player %d wall at %d %d", index, i, player->x, player->y);
            return;
        }
    }
}


void Model_get_player_wall(const Model* self, const int index, Wall* output, int* distance)
{
    const Player* player = &self->players[index];
    Model_try_hit_raycast(self, player->x, player->y, Model_get_opposite_direction(player->direction), output,
                          distance);
}

void Model_cancel(Model* self)
{
    self->state = GAME_STATE_MENU;
}

void Model_game_over(Model* self)
{
    self->state = GAME_STATE_GAME_OVER;
}

void Model_remove_player(Model* self, const int index)
{
    if (index < 0 || index >= self->num_players)
        return;
    for (int i = index; i < self->num_players - 1; i++)
        self->players[i] = self->players[i + 1];
    self->num_players--;

    // use allocated_player to free the memory
    int allocated_player = self->num_players + PLAYER_CHUNK - self->num_players % PLAYER_CHUNK;
    if (allocated_player != self->allocated_players)
    {
        Player* tmp = realloc(self->players, allocated_player * sizeof(Player));
        if (tmp == NULL)
            return;
        self->players = tmp;
        self->allocated_players = allocated_player;
    }
}

void Model_reset(Model* self)
{
    Model_clear_walls(self);
    self->state = GAME_STATE_MENU;
}
