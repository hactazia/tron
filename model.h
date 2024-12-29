#ifndef MODEL_H
#define MODEL_H
#include <stdbool.h>

typedef struct Tron Tron; // Forward declaration

typedef int PlayerState;
enum {
    PLAYER_STATE_ALIVE,
    PLAYER_STATE_DEAD,
    PLAYER_STATE_TO_DEATH
};

typedef int Direction;
enum {
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT
};

#define PLAYER_CHUNK 4
typedef struct Player {
    int x;
    int y;
    Direction direction;
    int score;
    PlayerState state;
} Player;

#define WALL_CHUNK 4
typedef struct Wall {
    int x;
    int y;
    Direction direction;
    int length;
    int player;
} Wall;

typedef int GameState;
enum {
    GAME_STATE_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_GAME_OVER
};

typedef struct Model {
    Tron *game;

    // Game area dimensions
    int width;
    int height;

    // List of players
    Player *players;
    int num_players;
    int allocated_players;

    // List of walls
    Wall *walls;
    int num_walls;
    int allocated_walls;

    // Game state
    GameState state;
} Model;

/**
 * @brief Initialize the model
 * @param self The model
 * @param width The width of the game area
 * @param height The height of the game area
 */
void Model_init(Model *self, int width, int height);

/**
 * @brief Add a player to the model
 * @param self The model
 * @param x The x position of the player
 * @param y The y position of the player
 * @param direction The direction of the player
 * @return True if the player was added, false otherwise
 */
bool Model_add_player(Model *self, const int x, const int y, const int direction);

/**
 * @brief Add a wall to the model
 * @param self The model
 * @param x The x position of the wall
 * @param y The y position of the wall
 * @param direction The direction of the wall
 * @param length The length of the wall
 * @return True if the wall was added, false otherwise
 */
bool Model_add_wall(Model *self, const int x, const int y, const int direction, const int length, const int player);

/**
 * @brief Destroy the model
 * @param self The model
 */
void Model_destroy(Model *self);

/**
 * @brief Get the opposite direction
 * @param direction The direction
 * @return The opposite direction
 */
Direction Model_get_opposite_direction(const Direction direction);

/**
 * @brief Check if the point hit a wall
 * @param wall The wall
 * @param x The x position
 * @param y The y position
 * @return True if the point hit the wall, false otherwise
 */
bool Model_hit_wall(const Wall* wall, const int x, const int y);

/**
 * @brief Try to hit a wall
 * @param model The model
 * @param x The x position
 * @param y The y position
 * @param output The output wall
 * @return True if the point hit a wall, false otherwise
 */
bool Model_try_hit_walls(const Model* model, const int x, const int y, Wall* output);

/**
 * @brief Check if the point hit a player
 * @param player The player to check
 * @param x The x position
 * @param y The y position
 * @return True if the point hit the player, false otherwise
 */
bool Model_hit_player(const Player* player, const int x, const int y);

/**
 * @brief Try to hit a player
 * @param model The model
 * @param index The index of the player to check
 * @param output The output index of the player
 */
bool Model_try_hit_players(const Model* model, const int index, int* output);

/**
 * @brief Check if the point is out of bounds
 * @param model The model
 * @param x The x position
 * @param y The y position
 * @return True if the point is out of bounds, false otherwise
 */
bool Model_out_of_bounds(const Model* model, const int x, const int y);

/**
 * @brief Try to hit a wall with a raycast
 * @param model The model
 * @param x The x position
 * @param y The y position
 * @param direction The direction of the raycast
 * @param output The output wall
 * @param distance The distance to the wall
 * @return True if the raycast hit a wall, false otherwise
 */
bool Model_try_hit_raycast(const Model* model, const int x, const int y, const int direction, Wall* output, int* distance);

/**
 * @brief Change the direction of a player
 * @param model The model
 * @param index The player index
 * @param direction The new direction
 * @return True if the direction was changed and add a wall, false otherwise
 * @note If the direction is the same as the current one, it will return true
 */
bool Model_change_direction(Model* model, const int index, const int direction);

/**
 * @brief Move a player
 * @param model The model
 * @param index The player index
 * @param speed Amount of movement
 */
void Model_move_player(const Model* model, const int index, const int speed);

/**
 * @brief Start the game
 * @param self The model
 */
void Model_start(Model *self);

/**
 * @brief Cancel the game
 * @param self The model
 */
void Model_cancel(Model *self);

/**
 * @brief End the game
 * @param self The model
 */
void Model_game_over(Model *self);

/**
 * @brief Calculate the player state
 * @param self The model
 * @param index The index of the player
 */
void Model_calculate_player_state(Model *self, const int index);


/**
 * @brief Get the player wall
 * @param self The model
 * @param index The index of the player
 * @param output The output wall
 * @param distance The distance to the wall
 */
void Model_get_player_wall(const Model *self, const int index, Wall *output, int *distance);

/**
 * @brief Remove a player
 * @param self The model
 * @param index The index of the player
 */
void Model_remove_player(Model *self, const int index);

/**
 * @brief Reset the model
 * @param self The model
 */
void Model_reset(Model *self);

#endif // MODEL_H
