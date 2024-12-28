#include "controller.h"

#include <stdlib.h>

#include "tron.h"

int Controller_get_height(const Controller *self) {
    return self->game->model->height;
}

int Controller_get_width(const Controller *self) {
    return self->game->model->width;
}

int Controller_get_player_count(const Controller *self) {
    return self->game->model->num_players;
}

Player *Controller_get_player(const Controller *self, const int index) {
    return &self->game->model->players[index];
}

int Controller_get_wall_count(const Controller *self) {
    return self->game->model->num_walls;
}

Wall *Controller_get_wall(const Controller *self, const int index) {
    return &self->game->model->walls[index];
}

GameState Controller_get_state(const Controller *self) {
    return self->game->model->state;
}

void Controller_set_state(const Controller *self, const GameState state) {
    const GameState old_state = self->game->model->state;
    if (old_state == state) return;

    if (old_state == GAME_STATE_MENU && state == GAME_STATE_PLAYING) {
        Model_start(self->game->model);
    } else if (old_state == GAME_STATE_PLAYING && state == GAME_STATE_MENU) {
        Model_cancel(self->game->model);
    }
}

void Controller_play(const Controller *self, const int width, const int height) {
    if (self->game->model->state == GAME_STATE_PLAYING) return;
    self->game->model->width = width;
    self->game->model->height = height;
    Controller_set_state(self, GAME_STATE_PLAYING);
}

void Controller_new_player(Controller *self) {
    Model_add_player(self->game->model, 0, 0, DIRECTION_UP);
}

void Controller_update(Controller *self) {
    if (self->game->model->state != GAME_STATE_PLAYING) return;

    for (int i = 0; i < self->game->model->num_players; i++) {
        Player* player = Controller_get_player(self, i);
        if (player->state != PLAYER_STATE_ALIVE) continue;
        Model_move_player(self->game->model, player, 1);
    }

    for (int i = 0; i < self->game->model->num_players; i++) {
        Player* player = Controller_get_player(self, i);
        const PlayerState state = player->state;
        if (state == PLAYER_STATE_DEAD) continue;
        Model_calculate_player_state(self->game->model, i);
    }

    for (int i = 0; i < self->game->model->num_players; i++) {
        Player* player = Controller_get_player(self, i);
        if (player->state == PLAYER_STATE_TO_DEATH) {
            player->state = PLAYER_STATE_DEAD;
            player->score = 0;
        }else if (player->state == PLAYER_STATE_ALIVE) {
            player->score++;
        }
    }
}

void Controller_get_player_wall(const Controller *self, const int index, Wall *output, int *distance) {
    const Player *player = &self->game->model->players[index];
    Model_try_hit_raycast(self->game->model, player->x, player->y, Model_get_opposite_direction(player->direction), output, distance);
}

