#ifndef VUE_SDL_H
#define VUE_SDL_H

#include <SDL_ttf.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL.h>
#include "controller.h"
#include "vue.h"

#define MENU_FPS 15
#define GAME_OVER_DELAY 3
#define SCOREBOARD_WIDTH 150
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define VueSDL_FLAG_CENTER 1
#define VueSDL_FLAG_RIGHT 2
#define VueSDL_FLAG_BOTTOM 4
#define VueSDL_FLAG_TOP 8
#define VueSDL_FLAG_MIDDLE 16
#define VueSDL_FLAG_LEFT 32


typedef enum VueSDL_KeyDirection
{
    VUE_SDL_KEY_UP,
    VUE_SDL_KEY_LEFT,
    VUE_SDL_KEY_DOWN,
    VUE_SDL_KEY_RIGHT
} VueSDL_KeyDirection;

extern const SDL_Keycode VueSDL_PLAYER_KEYS[MAX_PLAYERS][4];

typedef struct VueSDL
{
    Vue base;
} VueSDL;

typedef enum MenuState
{
    MENU_STATE_NONE,
    MENU_STATE_MAIN,
    MENU_STATE_OPTIONS,
    MENU_STATE_PLAY,
    MENU_STATE_PAUSE,
    MENU_STATE_GAME_OVER
} MenuState;

typedef struct VueSDL_Modal
{
    char* title;
    char* message1;
    char* message2;
    char* button1_text;
    char* button2_text;
    bool button1_clicked;
    bool button2_clicked;
} VueSDL_Modal;

typedef enum VueSDL_ClickState
{
    VUE_SDL_CLICK_DOWN,
    VUE_SDL_CLICK_UP,
    VUE_SDL_CLICK_ALREADY_DOWN,
    VUE_SDL_CLICK_ALREADY_UP
} VueSDL_ClickState;

typedef struct VueSDK_HandlerKey
{
    SDL_Keycode key;
    bool pressed;
} VueSDK_HandlerKey;

typedef struct SDLData
{
    Vue* self;
    time_t* start_date;
    SDL_Window* window;
    SDL_Renderer* renderer;
    MenuState menu_state;
    TTF_Font* font;
    VueSDL_Modal* modal;
    int click_state;
    VueSDK_HandlerKey* handler_key;
} SDLData;

/**
 * @brief Main function for the SDL Vue
 * @param self The Vue
 */
int VueSDL_main(Vue* self);

/**
 * @brief Convert a Uint8 to a float
 * @param value The Uint8 value
 * @return The float value
 */
float VueSDL_Uint8_to_float(Uint8 value);

/**
 * @brief Convert a float to a Uint8
 * @param value The float value
 * @return The Uint8 value
 */
Uint8 VueSDL_float_to_Uint8(float value);

/**
 * @brief Mix two colors
 * @param color The resulting color
 * @param color1 The first color
 * @param color2 The second color
 * @param ratio The ratio to mix the colors
 */
void VueSDL_mix_color(SDL_Color* color, const SDL_Color* color1, const SDL_Color* color2, float ratio);

/**
 * @brief Check if the mouse is clicked
 * @param data The SDL data
 * @return True if clicked, false otherwise
 */
bool VueSDL_is_clicked(SDLData* data);

/**
 * @brief Draw a box
 * @param data The SDL data
 * @param x The x position
 * @param y The y position
 * @param w The width
 * @param h The height
 * @param color The color
 */
void VueSDL_box(SDLData* data, int x, int y, int w, int h, SDL_Color color);

/**
 * @brief Check if a key is pressed
 * @param data The SDL data
 * @param key The key to check
 * @return True if pressed, false otherwise
 */
bool VueSDL_is_key_pressed(SDLData* data, SDL_Keycode key);

/**
 * @brief Draw a label
 * @param data The SDL data
 * @param text The text
 * @param x The x position
 * @param y The y position
 * @param color The color
 * @param flags The flags
 */
void VueSDL_label(SDLData* data, const char* text, int x, int y, SDL_Color color, int flags);

/**
 * @brief Draw a slider
 * @param data The SDL data
 * @param x The x position
 * @param y The y position
 * @param w The width
 * @param h The height
 * @param value The value
 * @param background The background color
 * @param select The selected color
 * @param min The minimum value
 * @param max The maximum value
 * @return True if clicked, false otherwise
 */
bool VueSDL_slider(SDLData* data, int x, int y, int w, int h, int* value, SDL_Color background, SDL_Color select,
                   int min, int max);

/**
 * @brief Draw a button
 * @param data The SDL data
 * @param text The text
 * @param x The x position
 * @param y The y position
 * @param w The width
 * @param h The height
 * @param background The background color
 * @param text_color The text color
 * @return True if clicked, false otherwise
 */
bool VueSDL_button(SDLData* data, const char* text, int x, int y, int w, int h, SDL_Color background,
                   SDL_Color text_color);

/**
 * @brief Check if a modal is valid
 * @param data The SDL data
 * @return True if valid, false otherwise
 */
bool VueSDL_valid_modal(SDLData* data);

/**
 * @brief Show a modal
 * @param data The SDL data
 * @param title The title
 * @param message1 The first message
 * @param message2 The second message
 * @param button1_text The first button text
 * @param button2_text The second button text
 */
void VueSDL_show_modal(SDLData* data, const char* title, const char* message1, const char* message2,
                       const char* button1_text, const char* button2_text);

/**
 * @brief Render a modal
 * @param data The SDL data
 */
void VueSDL_render_modal(SDLData* data);

/**
 * @brief Destroy a modal
 * @param data The SDL data
 */
void VueSDL_destroy_modal(SDLData* data);

/**
 * @brief Initialize SDL
 * @param data The SDL data
 * @return 0 on success, 1 on failure
 */
int VueSDL_init(SDLData* data);

/**
 * @brief Get the color text for a player
 * @param player The player index
 * @return The color text
 */
char* VueSDL_get_color_text(int player);

/**
 * @brief Get the color value for a player
 * @param player The player index
 * @return The color value
 */
SDL_Color VueSDL_get_color_value(int player);

/**
 * @brief Destroy SDL
 * @param data The SDL data
 */
void VueSDL_destroy(SDLData* data);

/**
 * @brief Delay for a given FPS
 * @param fps The FPS
 */
void VueSDL_Delay_FPS(Uint32 fps);

/**
 * @brief Render the main menu
 * @param data The SDL data
 */
void VueSDL_main_menu(SDLData* data);

/**
 * @brief Render the options menu
 * @param data The SDL data
 */
void VueSDL_option_menu(SDLData* data);

/**
 * @brief Render the game
 * @param data The SDL data
 */
void VueSDL_render_game(SDLData* data);

/**
 * @brief Render the play menu
 * @param data The SDL data
 */
void VueSDL_play_menu(SDLData* data);

/**
 * @brief Render the game over menu
 * @param data The SDL data
 */
void VueSDL_game_over_menu(SDLData* data);

/**
 * @brief Clear the keys
 * @param data The SDL data
 */
void VueSDL_clear_keys(SDLData* data);

/**
 * @brief Handle a key press
 * @param data The SDL data
 * @param key The key
 */
void VueSDL_handle_key(SDLData* data, SDL_Keycode key);

/**
 * @brief Main loop for SDL
 * @param data The SDL data
 */
void VueSDL_loop(SDLData* data);

#endif // VUE_SDL_H
