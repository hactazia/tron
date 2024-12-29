#include "vue_sdl.h"

#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "tron.h"
#include "utils.h"

const SDL_Color COLOR_WHITE = {255, 255, 255, 255};
const SDL_Color COLOR_BLACK = {0, 0, 0, 255};
const SDL_Color COLOR_RED = {255, 0, 0, 255};
const SDL_Color COLOR_GREEN = {0, 255, 0, 255};
const SDL_Color COLOR_BLUE = {0, 0, 255, 255};
const SDL_Color COLOR_YELLOW = {255, 255, 0, 255};
const SDL_Color COLOR_CYAN = {0, 255, 255, 255};
const SDL_Color COLOR_MAGENTA = {255, 0, 255, 255};
const SDL_Color COLOR_TRANSPARENT = {0, 0, 0, 0};
const SDL_Color COLOR_PRIMARY = {62, 62, 64, 255};
const SDL_Color COLOR_BACKGROUND_PRIMARY = {30, 31, 34, 255};
const SDL_Color COLOR_COLOR_PRIMARY = {171, 179, 182, 255};
const SDL_Color COLOR_SECONDARY = {52, 52, 54, 255};
const SDL_Color COLOR_BACKGROUND_SECONDARY = {44, 44, 46, 255};
const SDL_Color COLOR_COLOR_SECONDARY = {171, 179, 182, 255};

const SDL_Keycode VueSDL_PLAYER_KEYS[MAX_PLAYERS][4] = {
    {SDLK_UP, SDLK_LEFT, SDLK_DOWN, SDLK_RIGHT},
    {SDLK_z, SDLK_q, SDLK_s, SDLK_d},
    {SDLK_i, SDLK_j, SDLK_k, SDLK_l},
    {SDLK_t, SDLK_f, SDLK_g, SDLK_h},
    {SDLK_KP_5, SDLK_KP_1, SDLK_KP_2, SDLK_KP_3},
    {SDLK_KP_MINUS, SDLK_KP_7, SDLK_KP_8, SDLK_KP_9}
};

float VueSDL_Uint8_to_float(Uint8 value)
{
    return (float)value / 255;
}

Uint8 VueSDL_float_to_Uint8(float value)
{
    return (Uint8)(value * 255);
}

void VueSDL_mix_color(SDL_Color* color, const SDL_Color* color1, const SDL_Color* color2, float ratio)
{
    color->r = VueSDL_float_to_Uint8(
        VueSDL_Uint8_to_float(color1->r) * (1 - ratio) + VueSDL_Uint8_to_float(color2->r) * ratio);
    color->g = VueSDL_float_to_Uint8(
        VueSDL_Uint8_to_float(color1->g) * (1 - ratio) + VueSDL_Uint8_to_float(color2->g) * ratio);
    color->b = VueSDL_float_to_Uint8(
        VueSDL_Uint8_to_float(color1->b) * (1 - ratio) + VueSDL_Uint8_to_float(color2->b) * ratio);
};

bool VueSDL_is_clicked(SDLData* data)
{
    return data->click_state == VUE_SDL_CLICK_DOWN;
}

void VueSDL_box(SDLData* data, int x, int y, int w, int h, SDL_Color color)
{
    SDL_Rect rect = {x, y, w, h};
    SDL_SetRenderDrawColor(data->renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(data->renderer, &rect);
}

bool VueSDL_is_key_pressed(SDLData* data, SDL_Keycode key)
{
    for (int i = 0; i < 4 * MAX_PLAYERS; i++)
        if (data->handler_key[i].key == key)
            return data->handler_key[i].pressed;
    return false;
}

void VueSDL_label(SDLData* data, const char* text, int x, int y, SDL_Color color, int flags)
{
    SDL_Surface* surface = TTF_RenderText_Solid(data->font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(data->renderer, surface);
    SDL_Rect rect;

    if (flags & VueSDL_FLAG_LEFT)
    {
        rect.x = x;
        rect.y = y;
        rect.w = surface->w;
        rect.h = surface->h;
    }
    else if (flags & VueSDL_FLAG_RIGHT)
    {
        rect.x = x - surface->w;
        rect.y = y;
        rect.w = surface->w;
        rect.h = surface->h;
    }
    else if (flags & VueSDL_FLAG_CENTER)
    {
        rect.x = x - surface->w / 2;
        rect.y = y - surface->h / 2;
        rect.w = surface->w;
        rect.h = surface->h;
    }
    else if (flags & VueSDL_FLAG_TOP)
    {
        rect.x = x;
        rect.y = y;
        rect.w = surface->w;
        rect.h = surface->h;
    }
    else if (flags & VueSDL_FLAG_BOTTOM)
    {
        rect.x = x;
        rect.y = y - surface->h;
        rect.w = surface->w;
        rect.h = surface->h;
    }
    else if (flags & VueSDL_FLAG_MIDDLE)
    {
        rect.x = x - surface->w / 2;
        rect.y = y - surface->h / 2;
        rect.w = surface->w;
        rect.h = surface->h;
    }

    SDL_RenderCopy(data->renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

bool VueSDL_slider(SDLData* data, int x, int y, int w, int h, int* value, SDL_Color background, SDL_Color select,
                   int min, int max)
{
    // get cursor position
    int cursor_x, cursor_y;
    SDL_GetMouseState(&cursor_x, &cursor_y);

    bool isSelected = false;

    // check if cursor is inside the button
    if (cursor_x >= x && cursor_x <= x + w && cursor_y >= y && cursor_y <= y + h)
    {
        VueSDL_mix_color(&background, &background, &COLOR_WHITE, .2f);
        isSelected = true;
    }

    VueSDL_box(data, x, y, w, h, background);
    VueSDL_box(data, x, y, (min >= *value) ? 0 : (max <= *value) ? w : (w * (*value - min) / (max + 1 - min)), h,
               select);

    bool isClicked = false;
    if (isSelected && SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
    {
        isClicked = true;
        *value = min + (max + 1 - min) * cursor_x / w;
        if (*value < min)
            *value = min;
        if (*value > max)
            *value = max;
    }

    return isClicked;
}

bool VueSDL_button(SDLData* data, const char* text, int x, int y, int w, int h, SDL_Color background,
                   SDL_Color text_color)
{
    // get cursor position
    int cursor_x, cursor_y;
    SDL_GetMouseState(&cursor_x, &cursor_y);

    bool isSelected = false;

    // check if cursor is inside the button
    if (cursor_x >= x && cursor_x <= x + w && cursor_y >= y && cursor_y <= y + h)
    {
        VueSDL_mix_color(&background, &background, &COLOR_WHITE, .2f);
        isSelected = true;
    }

    bool isClicked = false;
    if (isSelected && VueSDL_is_clicked(data))
    {
        VueSDL_mix_color(&background, &background, &COLOR_BLACK, .2f);
        isClicked = true;
    }

    VueSDL_box(data, x, y, w, h, background);
    VueSDL_label(data, text, x + w / 2, y + h / 2 - 2, text_color, VueSDL_FLAG_CENTER);
    return isClicked;
}

bool VueSDL_valid_modal(SDLData* data)
{
    return data->modal != NULL;
}

void VueSDL_show_modal(SDLData* data, const char* title, const char* message1, const char* message2,
                       const char* button1_text,
                       const char* button2_text)
{
    if (data->modal == NULL)
    {
        data->modal = malloc(sizeof(VueSDL_Modal));
        if (data->modal == NULL)
            return;
    }

    data->modal->title = (char*)title;
    data->modal->message1 = (char*)message1;
    data->modal->message2 = (char*)message2;
    data->modal->button1_text = (char*)button1_text;
    data->modal->button2_text = (char*)button2_text;
    data->modal->button1_clicked = false;
    data->modal->button2_clicked = false;
}

void VueSDL_render_modal(SDLData* data)
{
    // make sure we have a valid modal
    if (!VueSDL_valid_modal(data))
        return;

    // modal background
    int w, h;
    SDL_GetWindowSize(data->window, &w, &h);
    VueSDL_box(data, 0, 0, w, h, COLOR_TRANSPARENT);

    // box for the modal
    int modal_w = 400;
    int modal_h = 200;
    int modal_x = w / 2 - modal_w / 2;
    int modal_y = h / 2 - modal_h / 2;
    VueSDL_box(data, modal_x, modal_y, modal_w, modal_h, COLOR_SECONDARY);

    // add title if available
    if (strlen(data->modal->title) > 0)
        VueSDL_label(data, data->modal->title, w / 2, modal_y + 10 + 16, COLOR_COLOR_SECONDARY, VueSDL_FLAG_CENTER);


    // add message if available
    int pos_x = modal_y + 78;
    if (strlen(data->modal->message1) > 0)
    {
        VueSDL_label(data, data->modal->message1, w / 2, pos_x, COLOR_COLOR_SECONDARY, VueSDL_FLAG_CENTER);
        pos_x += 32;
    }

    if (strlen(data->modal->message2) > 0)
        VueSDL_label(data, data->modal->message2, w / 2, pos_x, COLOR_COLOR_SECONDARY, VueSDL_FLAG_CENTER);

    // buttons
    int button_w = 150;
    int button_h = 30;
    int button_padding = 10;

    // check if we have buttons
    bool as_button1 = strlen(data->modal->button1_text) > 0;
    bool as_button2 = strlen(data->modal->button2_text) > 0;

    if (as_button1 && as_button2) // two buttons available
    {
        int button_y = h / 2 + modal_h / 2 - button_h - button_padding;
        if (VueSDL_button(data, data->modal->button1_text, w / 2 - button_w - button_padding / 2, button_y, button_w,
                          button_h, COLOR_PRIMARY, COLOR_COLOR_PRIMARY))
            data->modal->button1_clicked = true;
        if (VueSDL_button(data, data->modal->button2_text, w / 2 + button_padding / 2, button_y, button_w, button_h,
                          COLOR_PRIMARY, COLOR_COLOR_PRIMARY))
            data->modal->button2_clicked = true;
    }
    else if (as_button1 && !as_button2) // only first button available
    {
        int button_y = h / 2 + modal_h / 2 - button_h - button_padding;
        if (VueSDL_button(data, data->modal->button1_text, w / 2 - button_w / 2, button_y, button_w, button_h,
                          COLOR_PRIMARY, COLOR_COLOR_PRIMARY))
            data->modal->button1_clicked = true;
    }
    else if (as_button2 && !as_button1) // only second button available
    {
        int button_y = h / 2 + modal_h / 2 - button_h - button_padding;
        if (VueSDL_button(data, data->modal->button2_text, w / 2 - button_w / 2, button_y, button_w, button_h,
                          COLOR_PRIMARY, COLOR_COLOR_PRIMARY))
            data->modal->button2_clicked = true;
    }
    else if (!as_button1 && !as_button2) // no buttons available (show OK button)
    {
        int button_y = h / 2 + modal_h / 2 - button_h - button_padding;
        if (VueSDL_button(data, "OK", w / 2 - button_w / 2, button_y, button_w, button_h, COLOR_PRIMARY,
                          COLOR_COLOR_PRIMARY))
            data->modal->button1_clicked = true;
    }
}

void VueSDL_destroy_modal(SDLData* data)
{
    if (VueSDL_valid_modal(data))
    {
        free(data->modal);
        data->modal = NULL;
    }
}

int VueSDL_init(SDLData* data)
{
    debug_log("VueSDL_init");

    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        debug_log("SDL_Init failed");
        return 1;
    }

    // initialize TTF for text rendering
    if (TTF_Init() == -1)
    {
        debug_log("TTF_Init failed");
        return 1;
    }

    // create window
    SDL_Window* window = SDL_CreateWindow("Tron", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,
                                          SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (window == NULL)
    {
        debug_log("SDL_CreateWindow failed");
        return 1;
    }

    // create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        debug_log("SDL_CreateRenderer failed");
        return 1;
    }

    // set the main font
    data->font = TTF_OpenFont("assets/Roboto-Regular.ttf", 24);
    if (data->font == NULL)
    {
        debug_log("TTF_OpenFont failed");
        return 1;
    }

    data->window = window;
    data->renderer = renderer;

    // add the minimum number of players
    while (Controller_get_player_count(data->self->game->controller) < MIN_PLAYER)
        Controller_new_player(data->self->game->controller);

    return 0;
}

char* VueSDL_get_color_text(int player)
{
    switch (player)
    {
    case 0:
        return "Red";
    case 1:
        return "Green";
    case 2:
        return "Blue";
    case 3:
        return "Yellow";
    case 4:
        return "Cyan";
    case 5:
        return "Magenta";
    }
    return "Unknown";
}

SDL_Color VueSDL_get_color_value(int player)
{
    SDL_Color color = COLOR_WHITE;
    switch (player)
    {
    case 0:
        color = COLOR_RED;
        break;
    case 1:
        color = COLOR_GREEN;
        break;
    case 2:
        color = COLOR_BLUE;
        break;
    case 3:
        color = COLOR_YELLOW;
        break;
    case 4:
        color = COLOR_CYAN;
        break;
    case 5:
        color = COLOR_MAGENTA;
        break;
    }
    VueSDL_mix_color(&color, &COLOR_BLACK, &color, .5f);
    return color;
}


void VueSDL_destroy(SDLData* data)
{
    debug_log("VueSDL_destroy");
    SDL_DestroyRenderer(data->renderer);
    SDL_DestroyWindow(data->window);
    TTF_CloseFont(data->font);
    TTF_Quit();
    SDL_Quit();
    if (VueSDL_valid_modal(data))
        VueSDL_destroy_modal(data);
}

void VueSDL_Delay_FPS(Uint32 fps)
{
    SDL_Delay(1000 / fps);
}

void VueSDL_main_menu(SDLData* data)
{
    int w, h;
    SDL_GetWindowSize(data->window, &w, &h);

    // title
    VueSDL_label(data, "TRON", w / 2, 42, COLOR_COLOR_PRIMARY, VueSDL_FLAG_CENTER);

    // menu buttons
    const char* menu_items[] = {
        "Play",
        "Options",
        "Quit"
    };

    // menu items
    int menu_item_count = sizeof(menu_items) / sizeof(menu_items[0]);
    int menu_item_height = 32;
    int menu_item_padding = 16;
    int menu_height = menu_item_count * menu_item_height + (menu_item_count - 1) * menu_item_padding;
    int menu_y = h / 2 - menu_height / 2;

    for (int i = 0; i < menu_item_count; i++)
    {
        int menu_item_y = menu_y + i * (menu_item_height + menu_item_padding);
        if (VueSDL_button(data, menu_items[i], w / 2 - 100, menu_item_y, 200, menu_item_height, COLOR_PRIMARY,
                          COLOR_COLOR_PRIMARY))
        {
            if (VueSDL_valid_modal(data)) continue;
            if (i == 0) // play, check if we have at least 2 players to start, otherwise show modal
            {
                int player_count = Controller_get_player_count(data->self->game->controller);
                if (player_count < 2)
                    VueSDL_show_modal(data, "Error", "At least 2 players", "are required to play", "OK", "");
                data->menu_state = MENU_STATE_PLAY;
            }
            else if (i == 1) // options
            {
                data->menu_state = MENU_STATE_OPTIONS;
            }
            else if (i == 2) // quit
            {
                SDL_Event event;
                event.type = SDL_QUIT;
                SDL_PushEvent(&event);
            }
        }
    }

    // add credits
    if (VueSDL_button(data, "hactazia/tron", w - 175 - 10, h - 40, 175, 30,
                      COLOR_BACKGROUND_PRIMARY, COLOR_COLOR_PRIMARY))
        SDL_OpenURL("http://github.com/hactazia/tron");

    // modal comportment
    if (VueSDL_valid_modal(data) && data->modal->button1_clicked)
        VueSDL_destroy_modal(data);

    VueSDL_Delay_FPS(MENU_FPS);
}


void VueSDL_option_menu(SDLData* data)
{
    int w, h;
    SDL_GetWindowSize(data->window, &w, &h);

    // title
    VueSDL_label(data, "Options", 10, 10, COLOR_COLOR_PRIMARY, VueSDL_FLAG_LEFT);

    // back button
    if (VueSDL_button(data, "Back", w - 100, 10, 90, 30, COLOR_PRIMARY, COLOR_COLOR_PRIMARY))
        data->menu_state = MENU_STATE_MAIN;


    // options to add or remove players
    int player_count = Controller_get_player_count(data->self->game->controller);
    VueSDL_label(data, "Player Count", 10, 50, COLOR_COLOR_PRIMARY, VueSDL_FLAG_LEFT);
    VueSDL_slider(data, 10, 84, 200, 30, &player_count, COLOR_PRIMARY, COLOR_COLOR_PRIMARY, MIN_PLAYER, MAX_PLAYERS);

    // player count value label
    char player_count_text[10];
    sprintf(player_count_text, "%d", player_count);
    VueSDL_label(data, player_count_text, 220, 80, COLOR_COLOR_PRIMARY, VueSDL_FLAG_LEFT);

    // verify if player count has changed

    // add player if the slider is increased
    while (player_count > Controller_get_player_count(data->self->game->controller))
    {
        debug_logf("Adding player %d", Controller_get_player_count(data->self->game->controller));
        Controller_new_player(data->self->game->controller);
    }

    // remove player if the slider is decreased
    while (player_count < Controller_get_player_count(data->self->game->controller))
    {
        debug_logf("Removing player %d", Controller_get_player_count(data->self->game->controller) - 1);
        Controller_remove_player(data->self->game->controller,
                                 Controller_get_player_count(data->self->game->controller) - 1);
    }

    // label for each player
    char msg[100];
    SDL_Color colorValue;
    char* texts[] = {
        "Arrow keys",
        "Z Q S D",
        "I J K L",
        "T F G H",
        "1 2 3 5 (numpad)",
        "- 7 8 9 (numpad)"
    };
    for (int i = 0; i < player_count; i++)
    {
        colorValue = VueSDL_get_color_value(i);
        VueSDL_box(data, 10, 144 + i * 40, 20, 20, colorValue);
        sprintf(msg, "Player %s: %s", VueSDL_get_color_text(i), texts[i]);
        VueSDL_label(data, msg, 40, 140 + i * 40, COLOR_COLOR_PRIMARY, VueSDL_FLAG_LEFT);
    }

    VueSDL_Delay_FPS(MENU_FPS);
}


void VueSDL_render_game(SDLData* data)
{
    int w, h;
    SDL_GetWindowSize(data->window, &w, &h);

    // separator between scoreboard and game grid
    VueSDL_box(data, SCOREBOARD_WIDTH - 1, 0, 1, h, COLOR_PRIMARY);

    // scoreboard title
    VueSDL_label(data, "Scoreboard", SCOREBOARD_WIDTH / 2, 24, COLOR_COLOR_PRIMARY, VueSDL_FLAG_CENTER);

    // position players by score
    int player_count = Controller_get_player_count(data->self->game->controller);
    int player_scores[player_count][2];
    for (int i = 0; i < player_count; i++)
    {
        const Player* player = Controller_get_player(data->self->game->controller, i);
        player_scores[i][0] = i;
        player_scores[i][1] = player->score;
    }
    sort_array2d(player_scores, player_count);

    // scores for each player (by position)
    char msg[100];
    SDL_Color colorValue;
    for (int i = 0; i < player_count; i++)
    {
        Player* player = Controller_get_player(data->self->game->controller, player_scores[i][0]);
        colorValue = VueSDL_get_color_value(player_scores[i][0]);
        VueSDL_box(data, 10, 50 + i * 40, SCOREBOARD_WIDTH - 20, 30, colorValue);
        sprintf(msg, player->state == PLAYER_STATE_DEAD ? "%d X %d" : "%d - %d", i + 1, player_scores[i][1]);
        VueSDL_label(data, msg, SCOREBOARD_WIDTH / 2, 64 + i * 40, COLOR_COLOR_PRIMARY, VueSDL_FLAG_CENTER);
    }

    // game grid
    int width = Controller_get_width(data->self->game->controller);
    int height = Controller_get_height(data->self->game->controller);
    int cell_w = (w - SCOREBOARD_WIDTH) / width;
    int cell_h = h / height;

    // walls
    int wall_count = Controller_get_wall_count(data->self->game->controller);
    for (int i = 0; i < wall_count; i++)
    {
        Wall* wall = Controller_get_wall(data->self->game->controller, i);
        SDL_Color color = VueSDL_get_color_value(wall->player);
        switch (wall->direction)
        {
        case DIRECTION_UP:
            VueSDL_box(data, SCOREBOARD_WIDTH + wall->x * cell_w, wall->y * cell_h, cell_w, wall->length * cell_h,
                       color);
            break;
        case DIRECTION_DOWN:
            VueSDL_box(data, SCOREBOARD_WIDTH + wall->x * cell_w, (wall->y - wall->length + 1) * cell_h, cell_w,
                       wall->length * cell_h, color);
            break;
        case DIRECTION_LEFT:
            VueSDL_box(data, SCOREBOARD_WIDTH + wall->x * cell_w, wall->y * cell_h, wall->length * cell_w, cell_h,
                       color);
            break;
        case DIRECTION_RIGHT:
            VueSDL_box(data, SCOREBOARD_WIDTH + (wall->x - wall->length + 1) * cell_w, wall->y * cell_h,
                       wall->length * cell_w, cell_h, color);
            break;
        }
    }

    // player_walls
    Wall wall;
    int distance;
    for (int i = 0; i < player_count; i++)
    {
        Controller_get_player_wall(data->self->game->controller, i, &wall, &distance);
        SDL_Color color = VueSDL_get_color_value(i);
        Player* player = Controller_get_player(data->self->game->controller, i);
        switch (player->direction)
        {
        case DIRECTION_UP:
            VueSDL_box(data, SCOREBOARD_WIDTH + player->x * cell_w, player->y * cell_h, cell_w, distance * cell_h,
                       color);
            break;
        case DIRECTION_DOWN:
            VueSDL_box(data, SCOREBOARD_WIDTH + player->x * cell_w, (player->y - distance + 1) * cell_h, cell_w,
                       distance * cell_h, color);
            break;
        case DIRECTION_LEFT:
            VueSDL_box(data, SCOREBOARD_WIDTH + player->x * cell_w, player->y * cell_h, distance * cell_w, cell_h,
                       color);
            break;
        case DIRECTION_RIGHT:
            VueSDL_box(data, SCOREBOARD_WIDTH + (player->x - distance + 1) * cell_w, player->y * cell_h,
                       distance * cell_w, cell_h, color);
            break;
        }
    }

    // players
    for (int i = 0; i < player_count; i++)
    {
        Player* player = Controller_get_player(data->self->game->controller, i);
        SDL_Color color = VueSDL_get_color_value(i);
        VueSDL_box(data, SCOREBOARD_WIDTH + player->x * cell_w, player->y * cell_h, cell_w, cell_h, color);
    }
}

void VueSDL_play_menu(SDLData* data)
{
    int w, h;
    SDL_GetWindowSize(data->window, &w, &h);

    GameState state = Controller_get_state(data->self->game->controller);


    if (state == GAME_STATE_MENU)
    {
        // if game is not started, start it
        // if game isn't started, show modal

        Controller_play(data->self->game->controller, (w - SCOREBOARD_WIDTH) / 2, h / 2);
        debug_logf("Game state: %d", Controller_get_state(data->self->game->controller));
        if (GAME_STATE_PLAYING == Controller_get_state(data->self->game->controller))
        {
            debug_log("Game started!");
            *data->start_date = time(NULL);
        }
        else
        {
            debug_log("Game not started!");
            VueSDL_show_modal(data, "Error", "Game not started!", "", "Back", "");
        }
    }

    // calculate time since game started
    time_t crt = *data->start_date;
    long timer = time(NULL) - crt;

    if (VueSDL_valid_modal(data))
    {
        // comportment of the page when Pause modal is shown

        if (state == GAME_STATE_PLAYING)
            VueSDL_render_game(data);

        if (data->modal->button1_clicked)
        {
            // if resume button is clicked, resume the game with timeout
            VueSDL_destroy_modal(data);
            *data->start_date = time(NULL);
        }
        else if (data->modal->button2_clicked)
        {
            // if game over button is clicked, destroy modal and show game over menu
            VueSDL_destroy_modal(data);
            Controller_game_over(data->self->game->controller);
        }

        VueSDL_Delay_FPS(MENU_FPS);
    }
    else if (state == GAME_STATE_PLAYING && timer > DELAY)
    {
        // if game is playing and timeout is over, update the game and render it

        // handle player movements
        // if the player is alive, move it according to the keys pressed
        // if two keys in the sense of the player are pressed, the player will continue in the same direction
        int player_count = Controller_get_player_count(data->self->game->controller);
        for (int i = 0; i < player_count; i++)
        {
            Player* player = Controller_get_player(data->self->game->controller, i);
            if (player->state != PLAYER_STATE_ALIVE) continue;
            int dx = VueSDL_is_key_pressed(data, VueSDL_PLAYER_KEYS[i][VUE_SDL_KEY_RIGHT])
                - VueSDL_is_key_pressed(data, VueSDL_PLAYER_KEYS[i][VUE_SDL_KEY_LEFT]);
            int dy = VueSDL_is_key_pressed(data, VueSDL_PLAYER_KEYS[i][VUE_SDL_KEY_DOWN])
                - VueSDL_is_key_pressed(data, VueSDL_PLAYER_KEYS[i][VUE_SDL_KEY_UP]);

            Direction new_direction;
            if (dx != 0 && dy == 0)
                new_direction = dx > 0 ? DIRECTION_RIGHT : DIRECTION_LEFT;
            else if (dx == 0 && dy != 0)
                new_direction = dy > 0 ? DIRECTION_DOWN : DIRECTION_UP;
            else new_direction = player->direction;

            Controller_move_player(data->self->game->controller, i, new_direction);
        }


        // update game state
        Controller_update(data->self->game->controller);
        state = Controller_get_state(data->self->game->controller);

        // check if game is over
        if (state == GAME_STATE_PLAYING)
        {
            VueSDL_render_game(data);
            VueSDL_Delay_FPS(GAME_FPS);
        }
        else if (state == GAME_STATE_GAME_OVER)
        {
            // if game is over, show game over page with timeout
            *data->start_date = time(NULL);
        }
    }
    else if (state == GAME_STATE_PLAYING)
    {
        // if game is playing and timeout is not over, show countdown
        VueSDL_render_game(data);
        char msg[100];
        sprintf(msg, "Starting in %ld seconds...", DELAY - timer);
        VueSDL_label(data, msg, (w + SCOREBOARD_WIDTH) / 2, h / 2, COLOR_COLOR_PRIMARY, VueSDL_FLAG_CENTER);
        VueSDL_Delay_FPS(MENU_FPS);
    }

    // recalculate time since game started (for game over page)
    crt = *data->start_date;
    timer = time(NULL) - crt;

    if (state == GAME_STATE_GAME_OVER && timer > GAME_OVER_DELAY)
    {
        // if game is over and timeout is over, show game over menu
        VueSDL_destroy_modal(data);
        data->menu_state = MENU_STATE_GAME_OVER;
    }
    else if (state == GAME_STATE_GAME_OVER)
    {
        // if game is over and timeout is not over, await timeout
        VueSDL_render_game(data);
        VueSDL_label(data, "Game Over", (w + SCOREBOARD_WIDTH) / 2, 42, COLOR_COLOR_PRIMARY, VueSDL_FLAG_CENTER);
        VueSDL_Delay_FPS(MENU_FPS);
    }

    // pause button and show pause modal if clicked
    if (state == GAME_STATE_PLAYING && VueSDL_button(data,
                                                     "Pause",
                                                     10, h - 40,
                                                     SCOREBOARD_WIDTH - 20, 30,
                                                     COLOR_PRIMARY, COLOR_COLOR_PRIMARY))
        VueSDL_show_modal(data, "Pause", "Game is paused", "", "Resume", "Game Over");
}

void VueSDL_game_over_menu(SDLData* data)
{
    int w, h;
    SDL_GetWindowSize(data->window, &w, &h);

    // title
    VueSDL_label(data, "Game Over", w / 2, 42, COLOR_COLOR_PRIMARY, VueSDL_FLAG_CENTER);

    // back button
    if (VueSDL_button(data, "Main Menu", w / 2 - 100, h - 40 - 10, 200, 30, COLOR_PRIMARY, COLOR_COLOR_PRIMARY))
    {
        data->menu_state = MENU_STATE_MAIN;
        Controller_set_state(data->self->game->controller, GAME_STATE_MENU);
    }

    // sort players by score
    int player_count = Controller_get_player_count(data->self->game->controller);
    int player_scores[player_count][2];
    for (int i = 0; i < player_count; i++)
    {
        const Player* player = Controller_get_player(data->self->game->controller, i);
        player_scores[i][0] = i;
        player_scores[i][1] = player->score;
    }
    sort_array2d(player_scores, player_count);

    // show top players
    int top_count = player_count > 6 ? 6 : player_count;
    int padding = 10;
    int height = 30;
    int y = h / 2 - (top_count * height + (top_count - 1) * padding) / 2;
    char msg[100];
    for (int i = 0; i < top_count; i++)
    {
        VueSDL_box(data, w / 2 - 200, y - 14, 400, height, VueSDL_get_color_value(player_scores[i][0]));
        sprintf(msg, "%d: Player %s - %d points",
                i + 1,
                VueSDL_get_color_text(player_scores[i][0]),
                player_scores[i][1]
        );
        VueSDL_label(data, msg, w / 2, y, COLOR_COLOR_PRIMARY, VueSDL_FLAG_CENTER);
        y += height + padding;
    }

    VueSDL_Delay_FPS(MENU_FPS);
}

void VueSDL_clear_keys(SDLData* data)
{
    for (int i = 0; i < 4 * MAX_PLAYERS; i++)
        data->handler_key[i].pressed = false;
}

void VueSDL_handle_key(SDLData* data, SDL_Keycode key)
{
    for (int i = 0; i < 4 * MAX_PLAYERS; i++)
        if (data->handler_key[i].key == key)
        {
            data->handler_key[i].pressed = true;
            break;
        }
}


void VueSDL_loop(SDLData* data)
{
    debug_log("VueSDL_loop");
    bool running = true;

    while (running)
    {
        // handle events
        SDL_Event event;
        VueSDL_clear_keys(data);
        while (SDL_PollEvent(&event))
            if (event.type == SDL_QUIT)
                running = false;
            else if (event.type == SDL_KEYDOWN)
                VueSDL_handle_key(data, event.key.keysym.sym);

        // clear screen
        SDL_SetRenderDrawColor(data->renderer,
                               COLOR_BACKGROUND_PRIMARY.r, COLOR_BACKGROUND_PRIMARY.g, COLOR_BACKGROUND_PRIMARY.b,
                               COLOR_BACKGROUND_PRIMARY.a);
        SDL_RenderClear(data->renderer);

        // evaluate click state
        bool isClicked = false;
        if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
            isClicked = true;

        // update click state with previous state
        if (data->click_state == VUE_SDL_CLICK_DOWN && !isClicked)
            data->click_state = VUE_SDL_CLICK_UP;
        else if (data->click_state == VUE_SDL_CLICK_UP && isClicked)
            data->click_state = VUE_SDL_CLICK_DOWN;
        else if (data->click_state == VUE_SDL_CLICK_DOWN && isClicked)
            data->click_state = VUE_SDL_CLICK_ALREADY_DOWN;
        else if (data->click_state == VUE_SDL_CLICK_UP && !isClicked)
            data->click_state = VUE_SDL_CLICK_ALREADY_UP;
        else if (data->click_state == VUE_SDL_CLICK_ALREADY_DOWN && !isClicked)
            data->click_state = VUE_SDL_CLICK_UP;
        else if (data->click_state == VUE_SDL_CLICK_ALREADY_UP && isClicked)
            data->click_state = VUE_SDL_CLICK_DOWN;


        // render the current menu
        if (data->menu_state == MENU_STATE_MAIN)
            VueSDL_main_menu(data);
        else if (data->menu_state == MENU_STATE_OPTIONS)
            VueSDL_option_menu(data);
        else if (data->menu_state == MENU_STATE_PLAY)
            VueSDL_play_menu(data);
        else if (data->menu_state == MENU_STATE_GAME_OVER)
            VueSDL_game_over_menu(data);

        // render modal if available
        if (VueSDL_valid_modal(data))
            VueSDL_render_modal(data);

        SDL_RenderPresent(data->renderer);
    }
}


int VueSDL_main(Vue* self)
{
    debug_log("VueSDL_main");

    // initialize timer for game
    time_t* tim = malloc(sizeof(time_t));
    if (tim == NULL)
        return 1;
    *tim = time(NULL);

    SDLData data = {
        self,
        tim,
        NULL,
        NULL,
        MENU_STATE_MAIN,
        NULL,
        NULL,
        VUE_SDL_CLICK_UP,
        NULL
    };

    // initialize handler keys
    data.handler_key = malloc(4 * MAX_PLAYERS * sizeof(VueSDK_HandlerKey));
    if (data.handler_key == NULL)
    {
        free(tim);
        return 1;
    }
    for (int i = 0; i < 4 * MAX_PLAYERS; i++)
    {
        data.handler_key[i].key = VueSDL_PLAYER_KEYS[i / 4][i % 4];
        data.handler_key[i].pressed = false;
    }

    int io = VueSDL_init(&data);
    if (io == 0) VueSDL_loop(&data);
    VueSDL_destroy(&data);

    // free memory
    free(tim);
    free(data.handler_key);

    return io;
}
