#include "model.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>



void Model_init(Model *self, int width, int height) {
    self->height = height;
    self->width = width;
    self->players = NULL;
    self->num_players = 0;
    self->walls = NULL;
    self->num_walls = 0;
    self->state = GAME_STATE_MENU;
}

bool Model_add_player(Model *self, const int x, const int y, const int direction) {
    if (self->num_players % PLAYER_CHUNK == 0) {
        Player* tmp = realloc(self->players, (self->num_players + PLAYER_CHUNK) * sizeof(Player));
        if (tmp == NULL)
            return false;
        self->players = tmp;
    }
    self->players[self->num_players].x = x;
    self->players[self->num_players].y = y;
    self->players[self->num_players].direction = direction;
    self->players[self->num_players].score = 0;
    self->players[self->num_players].state = PLAYER_STATE_ALIVE;
    self->num_players++;
    return true;
}

bool Model_add_wall(Model *self, const int x, const int y, const int direction, const int length) {
    if (self->num_walls % WALL_CHUNK == 0) {
        Wall* tmp = realloc(self->walls, (self->num_walls + WALL_CHUNK) * sizeof(Wall));
        if (tmp == NULL)
            return false;
        self->walls = tmp;
    }
    self->walls[self->num_walls].x = x;
    self->walls[self->num_walls].y = y;
    self->walls[self->num_walls].direction = direction;
    self->walls[self->num_walls].length = length;
    self->num_walls++;
    return true;
}

void Model_destroy(Model *self) {
    free(self->players);
    free(self->walls);
}

Direction Model_get_opposite_direction(const Direction direction) {
    switch (direction) {
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

bool Model_hit_wall(const Wall* wall, const int x, const int y) {
    switch (wall->direction) {
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

bool Model_try_hit_walls(const Model* model, const int x, const int y, Wall** output) {
    for (int i = 0; i < model->num_walls; i++)
        if (Model_hit_wall(&model->walls[i], x, y)) {
            *output = &model->walls[i];
            return true;
        }
    return false;
}

bool Model_hit_player(const Player* player, const int x, const int y) {
    return player->x == x && player->y == y;
}

bool Model_try_hit_players(const Model* model, const int index, int* output) {
    const Player* player = &model->players[index];
    if (player->state != PLAYER_STATE_ALIVE) return false;
    for (int i = 0; i < model->num_players; i++) {
        if (index == i) continue;
        const Player* other = &model->players[i];
        if (other->state != PLAYER_STATE_ALIVE) continue;
        if (Model_hit_player(other, player->x, player->y)) {
            *output = i;
            return true;
        }
    }
    return false;
}

bool Model_out_of_bounds(const Model* model, const int x, const int y) {
    return x < 0 || x >= model->width || y < 0 || y >= model->height;
}

bool Model_try_hit_raycast(const Model* model, const int x, const int y, const int direction, Wall** output, int* distance) {
    int dx = 0;
    int dy = 0;
    switch (direction) {
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
    while (!Model_out_of_bounds(model, cx, cy)) {
        if (Model_try_hit_walls(model, cx, cy, output)) {
            switch (direction) {
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

bool Model_change_direction(Model* model, Player* player, const int direction) {
    // check if the direction is the same as the current one, if so, return true
    const Direction current = player->direction;
    if (direction == current)
        return true;

    // check if the direction is the opposite of the current one, if so, return false
    const Direction opposite = Model_get_opposite_direction(current);
    if (direction == opposite)
        return false;

    // add a wall in the direction of the player until it hits a wall
    Wall *wall;
    int distance;
    if (Model_try_hit_raycast(model, player->x, player->y, opposite, &wall, &distance)) {
        if (!Model_add_wall(model, player->x, player->y, opposite, distance))
            return false;
    }

    player->direction = direction;
    return true;
}

void Model_move_player(const Model* model, Player* player, const int speed) {
    int dx = 0;
    int dy = 0;
    switch (player->direction) {
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

void Model_place_player(const Model *self, Player *player, const int index) {
    // 1. get the radius
    const int radius = self->width / 2 < self->height
        ? self->width / 6
        : self->height / 3;

    // 2. get the angle
    const float angle = (float)index / self->num_players * 2 * M_PI;

    // 3. get the position
    player->x = self->width / 2 + cos(angle) * radius * 2;
    player->y = self->height / 2 + sin(angle) * radius;

    // 4. get the direction
    player->direction = (int)(angle / M_PI_2) % 4;
}

void Model_start(Model *self) {
    const GameState state = self->state;
    if (state == GAME_STATE_PLAYING)
        return;
    if (self->num_players == 0)
        return;

    // set the players with default values
    for (int i = 0; i < self->num_players; i++) {
        self->players[i].score = 0;
        self->players[i].state = PLAYER_STATE_ALIVE;
        Model_place_player(self, &self->players[i], i);
    }

    // set the walls with default values
    self->num_walls = 0;
    for (int i = 0; i < self->num_players; i++)
        if (!Model_add_wall(self, self->players[i].x, self->players[i].y, self->players[i].direction, 1))
            return;

    // set the state to playing
    self->state = GAME_STATE_PLAYING;
}

void Model_calculate_player_state(Model *self, const int index) {
    // check if the player is out of bounds
    Player* player = &self->players[index];
    if (Model_out_of_bounds(self, player->x, player->y)) {
        player->state = PLAYER_STATE_TO_DEATH;
        return;
    }

    // check if the player hit a wall
    Wall *wall;
    if (Model_try_hit_walls(self, player->x, player->y, &wall)) {
        player->state = PLAYER_STATE_TO_DEATH;
        return;
    }

    // check if the player hit another player
    int output;
    if (Model_try_hit_players(self, index, &output)) {
        Player *other = &self->players[output];
        player->state = PLAYER_STATE_TO_DEATH;
        other->state = PLAYER_STATE_TO_DEATH;
        return;
    }
}

void Model_cancel(Model *self) {
    self->state = GAME_STATE_MENU;
}