#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "model.h"

typedef struct Tron Tron; // Forward declaration

typedef struct Controller {
    Tron *game;
} Controller;

int Controller_get_height(const Controller *self);
int Controller_get_width(const Controller *self);
int Controller_get_player_count(const Controller *self);
Player *Controller_get_player(const Controller *self, const int index);
int Controller_get_wall_count(const Controller *self);
Wall *Controller_get_wall(const Controller *self, const int index);
GameState Controller_get_state(const Controller *self);
void Controller_set_state(const Controller *self, const GameState state);
void Controller_new_player(Controller *self);
void Controller_play(const Controller *self, const int width, const int height);
void Controller_update(Controller *self);
void Controller_get_player_wall(const Controller *self, const int index, Wall *output, int *distance);

#endif // CONTROLLER_H
