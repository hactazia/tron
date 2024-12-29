#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "model.h"

#define MAX_PLAYERS 6
#define MIN_PLAYER 2
#define DELAY 5

// Forward declaration of the Tron struct
typedef struct Tron Tron;

// Controller struct definition
typedef struct Controller {
    Tron* game; // Pointer to the Tron game instance
} Controller;

/**
 * @brief Get the height of the game field.
 * @param self Pointer to the Controller instance.
 * @return The height of the game field.
 */
int Controller_get_height(const Controller* self);

/**
 * @brief Get the width of the game field.
 * @param self Pointer to the Controller instance.
 * @return The width of the game field.
 */
int Controller_get_width(const Controller* self);

/**
 * @brief Get the number of players in the game.
 * @param self Pointer to the Controller instance.
 * @return The number of players.
 */
int Controller_get_player_count(const Controller* self);

/**
 * @brief Get a player by index.
 * @param self Pointer to the Controller instance.
 * @param index The index of the player.
 * @return Pointer to the Player instance.
 */
Player* Controller_get_player(const Controller* self, const int index);

/**
 * @brief Get the number of walls in the game.
 * @param self Pointer to the Controller instance.
 * @return The number of walls.
 */
int Controller_get_wall_count(const Controller* self);

/**
 * @brief Get a wall by index.
 * @param self Pointer to the Controller instance.
 * @param index The index of the wall.
 * @return Pointer to the Wall instance.
 */
Wall* Controller_get_wall(const Controller* self, const int index);

/**
 * @brief Get the current state of the game.
 * @param self Pointer to the Controller instance.
 * @return The current game state.
 */
GameState Controller_get_state(const Controller* self);

/**
 * @brief Set the state of the game.
 * @param self Pointer to the Controller instance.
 * @param state The new game state.
 */
void Controller_set_state(const Controller* self, const GameState state);

/**
 * @brief Add a new player to the game.
 * @param self Pointer to the Controller instance.
 */
void Controller_new_player(Controller* self);

/**
 * @brief Remove a player from the game by index.
 * @param self Pointer to the Controller instance.
 * @param index The index of the player to remove.
 */
void Controller_remove_player(Controller* self, const int index);

/**
 * @brief Start playing the game with specified width and height.
 * @param self Pointer to the Controller instance.
 * @param width The width of the game field.
 * @param height The height of the game field.
 */
void Controller_play(const Controller* self, const int width, const int height);

/**
 * @brief Cancel the current game and return to the menu.
 * @param self Pointer to the Controller instance.
 */
void Controller_cancel(const Controller* self);

/**
 * @brief End the current game and set the state to game over.
 * @param self Pointer to the Controller instance.
 */
void Controller_game_over(const Controller* self);

/**
 * @brief Update the game state, moving players and checking for collisions.
 * @param self Pointer to the Controller instance.
 */
void Controller_update(Controller* self);

/**
 * @brief Get the wall associated with a player.
 * @param self Pointer to the Controller instance.
 * @param index The index of the player.
 * @param output Pointer to the Wall instance to store the result.
 * @param distance Pointer to an integer to store the distance.
 */
void Controller_get_player_wall(const Controller* self, const int index, Wall* output, int* distance);

/**
 * @brief Check if a position is out of bounds.
 * @param self Pointer to the Controller instance.
 * @param x The x-coordinate.
 * @param y The y-coordinate.
 * @return True if out of bounds, false otherwise.
 */
bool Controller_out_of_bounds(const Controller* self, const int x, const int y);

/**
 * @brief Move a player in a specified direction.
 * @param self Pointer to the Controller instance.
 * @param index The index of the player.
 * @param direction The direction to move the player.
 */
void Controller_move_player(Controller* self, const int index, const Direction direction);

#endif // CONTROLLER_H