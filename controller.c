#include "controller.h"
#include "utils.h"

#include "tron.h"

int Controller_get_height(const Controller* self)
{
    return self->game->model->height;
}

int Controller_get_width(const Controller* self)
{
    return self->game->model->width;
}

int Controller_get_player_count(const Controller* self)
{
    return self->game->model->num_players;
}

Player* Controller_get_player(const Controller* self, const int index)
{
    return &self->game->model->players[index];
}

int Controller_get_wall_count(const Controller* self)
{
    return self->game->model->num_walls;
}

Wall* Controller_get_wall(const Controller* self, const int index)
{
    return &self->game->model->walls[index];
}

GameState Controller_get_state(const Controller* self)
{
    return self->game->model->state;
}

void Controller_set_state(const Controller* self, const GameState state)
{
    const GameState old_state = self->game->model->state;
    if (old_state == state)
    {
        debug_log("Game already in this state");
        return;
    }

    if (old_state == GAME_STATE_GAME_OVER && state == GAME_STATE_MENU)
    {
        Model_reset(self->game->model);
    }
    else if ((old_state == GAME_STATE_MENU || old_state == GAME_STATE_GAME_OVER) && state == GAME_STATE_PLAYING)
    {
        Model_start(self->game->model);
    }
    else if (old_state == GAME_STATE_PLAYING && state == GAME_STATE_MENU)
    {
        Model_cancel(self->game->model);
    }
    else if (old_state == GAME_STATE_PLAYING && state == GAME_STATE_GAME_OVER)
    {
        Model_game_over(self->game->model);
    }
    else debug_log("Invalid state transition");
}

void Controller_play(const Controller* self, const int width, const int height)
{
    if (self->game->model->state == GAME_STATE_PLAYING)
    {
        debug_log("Game already playing");
        return;
    }
    self->game->model->width = width;
    self->game->model->height = height;
    Controller_set_state(self, GAME_STATE_PLAYING);
    debug_logf("New game state: %d", Controller_get_state(self));
}

void Controller_cancel(const Controller* self)
{
    if (self->game->model->state == GAME_STATE_MENU)
    {
        debug_log("Game already stopped");
        return;
    }
    Controller_set_state(self, GAME_STATE_MENU);
    debug_logf("New game state: %d", Controller_get_state(self));
}

void Controller_game_over(const Controller* self)
{
    if (self->game->model->state == GAME_STATE_GAME_OVER)
    {
        debug_log("Game already over");
        return;
    }
    Controller_set_state(self, GAME_STATE_GAME_OVER);
    debug_logf("New game state: %d", Controller_get_state(self));
}

void Controller_new_player(Controller* self)
{
    Model_add_player(self->game->model, 0, 0, DIRECTION_UP);
}

void Controller_remove_player(Controller* self, const int index)
{
    Model_remove_player(self->game->model, index);
}

void Controller_update(Controller* self)
{
    if (self->game->model->state != GAME_STATE_PLAYING) return;

    for (int i = 0; i < self->game->model->num_players; i++)
    {
        Player* player = Controller_get_player(self, i);
        if (player->state != PLAYER_STATE_ALIVE) continue;
        Model_move_player(self->game->model, i, 1);
    }

    for (int i = 0; i < self->game->model->num_players; i++)
    {
        Player* player = Controller_get_player(self, i);
        const PlayerState state = player->state;
        if (state == PLAYER_STATE_DEAD) continue;
        Model_calculate_player_state(self->game->model, i);
    }

    Wall wall;
    int distance;
    for (int i = 0; i < self->game->model->num_players; i++)
    {
        Player* player = Controller_get_player(self, i);
        if (player->state == PLAYER_STATE_TO_DEATH)
        {
            player->state = PLAYER_STATE_DEAD;
            Direction opposite = Model_get_opposite_direction(player->direction);
            Direction current = player->direction;
            int dx, dy;
            Model_get_relative_direction(opposite, &dx, &dy);
            if (Model_try_hit_raycast(self->game->model, player->x + dx, player->y + dy, opposite, &wall, &distance))
                Model_add_wall(self->game->model, player->x, player->y, current, distance, i);
        }
        else if (player->state == PLAYER_STATE_ALIVE)
            player->score++;
    }

    int sum_player_alive = 0;
    for (int i = 0; i < self->game->model->num_players; i++)
    {
        Player* player = Controller_get_player(self, i);
        if (player->state == PLAYER_STATE_ALIVE)
            sum_player_alive++;
    }

    if (sum_player_alive <= 1)
        Controller_set_state(self, GAME_STATE_GAME_OVER);
}

void Controller_get_player_wall(const Controller* self, const int index, Wall* output, int* distance)
{
    Model_get_player_wall(self->game->model, index, output, distance);
}

bool Controller_out_of_bounds(const Controller* self, const int x, const int y)
{
    return Model_out_of_bounds(self->game->model, x, y);
}

void Controller_move_player(Controller* self, const int index, const Direction direction)
{
    Player* player = Controller_get_player(self, index);
    if (player->state != PLAYER_STATE_ALIVE) return;
    Model_change_direction(self->game->model, index, direction);
}
