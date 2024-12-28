#include <stdio.h>
#include "vue_ncurses.h"
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "ncurses.h"
#include "tron.h"

void VueNCURSES_draw_border(NCURSESData *data) {
    // render the game preview
    wclear(data->win);
    box(data->win, 0, 0);
    wrefresh(data->win);

    // render the scoreboard
    wclear(data->scoreboard);
    box(data->scoreboard, 0, 0);
    wrefresh(data->scoreboard);

    // render the message
    wclear(data->msg);
    box(data->msg, 0, 0);
    wrefresh(data->msg);
}

void VueNCURSES_init(NCURSESData *data) {
    // make a tron GUI with ncurses
    // add a window for the game preview
    // add a window for the scoreboards

    // initialize ncurses
    initscr();

    // create the game preview window
    data->scoreboard = newwin(LINES-3, 24, 0, 0);
    data->win = newwin(LINES-3, COLS - 24, 0, 24);
    data->msg = newwin(3, COLS, LINES-3, 0);

    VueNCURSES_draw_border(data);
}

int VueNCURSES_estimate(const int a, const int w1, const int w2) {
    return (int) ((float) a / (float) w1 * (float) w2);
}

int number_length(int number) {
    int length = 0;
    while (number != 0) {
        number /= 10;
        length++;
    }
    return length;
}

void sort(int arr[][2], const int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j][1] < arr[j + 1][1]) {
                const int temp = arr[j][1];
                arr[j][1] = arr[j + 1][1];
                arr[j + 1][1] = temp;
            }
}

void VueNCURSES_draw_window(NCURSESData *data) {

    // modify the message window
    box(data->msg, 0, 0);
    char clear[getmaxx(data->msg)-2];
    memset(clear, ' ', getmaxx(data->msg)-2);
    clear[getmaxx(data->msg)-2] = '\0';
    mvwprintw(data->msg, 1, 1, clear);
    mvwprintw(data->msg, 1, (getmaxx(data->msg) - strlen(data->msg_text)) / 2, data->msg_text);
    wrefresh(data->msg);

    // modify the scoreboard window
    box(data->scoreboard, 0, 0);
    const char* scoreboard_text = "Scoreboard";
    mvwprintw(data->scoreboard, 1, (getmaxx(data->scoreboard) - strlen(scoreboard_text)) / 2, scoreboard_text);

    const int player_count = Controller_get_player_count(data->self->game->controller);
    int scoreboard[player_count][2];
    for (int i = 0; i < player_count; i++) {
        const Player *player = Controller_get_player(data->self->game->controller, i);
        scoreboard[i][0] = i;
        scoreboard[i][1] = player->score;
    }
    sort(scoreboard, player_count);

    for (int i = 0; i < player_count; i++) {
        mvwprintw(data->scoreboard, i + 3, 2, "Player %d", scoreboard[i][0]);
        mvwprintw(data->scoreboard, i + 3, getmaxx(data->scoreboard) - number_length(scoreboard[i][1]) - 3, "%d", scoreboard[i][1]);
    }
    wrefresh(data->scoreboard);

    // modify the game preview window
    box(data->win, 0, 0);
    const int width = Controller_get_width(data->self->game->controller);
    const int height = Controller_get_height(data->self->game->controller);
    const int width_win = getmaxx(data->win);
    const int height_win = getmaxy(data->win);


    const int wall_count = Controller_get_wall_count(data->self->game->controller);
    for (int i = 0; i < wall_count; i++) {
        const Wall *wall = Controller_get_wall(data->self->game->controller, i);
        const int x = VueNCURSES_estimate(wall->x, width, width_win);
        const int y = VueNCURSES_estimate(wall->y, height, height_win);

        switch (wall->direction) {
            case DIRECTION_UP:
            case DIRECTION_DOWN:
                mvwvline(data->win, y, x, '|', wall->length);
                break;
            case DIRECTION_LEFT:
            case DIRECTION_RIGHT:
                mvwhline(data->win, y, x, '-', wall->length);
                break;
            default: ;
        }
    }


    for (int i = 0; i < player_count; i++) {
        const Player *player = Controller_get_player(data->self->game->controller, i);
        const int x = VueNCURSES_estimate(player->x, width, width_win);
        const int y = VueNCURSES_estimate(player->y, height, height_win);
        int distance;
        Wall wall;
        Controller_get_player_wall(data->self->game->controller, i, &wall, &distance);
        if (distance > 0) {
            const int x0 = VueNCURSES_estimate(wall.x, width, width_win);
            const int y0 = VueNCURSES_estimate(wall.y, height, height_win);
            switch (player->direction) {
                case DIRECTION_UP:
                    mvwvline(data->win, y, x, '|', distance);
                break;
                case DIRECTION_LEFT:
                    mvwhline(data->win, y, x, '-', distance);
                break;
                case DIRECTION_DOWN:
                    mvwvline(data->win, y0, x0, '|', distance);
                    break;
                case DIRECTION_RIGHT:
                    mvwhline(data->win, y0, x0, '-', distance);
                    break;
                default: ;
            }
        }

        mvwprintw(data->win, y, x, "P");
    }


    wrefresh(data->win);
}
#define DELAY 6

void VueNCURSES_loop(NCURSESData *data) {
    bool running = true;
    nodelay(stdscr, TRUE); // Make getch non-blocking

    while (running) {
        const int ch = getch();


        switch (ch) {
            case '=': // quit
                running = false;
                data->msg_text = "Goodbye!";
            break;
            case '$':
                Controller_new_player(data->self->game->controller);
                data->msg_text = "Player added!";
                break;
            case '*':
                Controller_play(data->self->game->controller, getmaxx(data->win), getmaxy(data->win));
                if (GAME_STATE_PLAYING == Controller_get_state(data->self->game->controller)) {
                    data->msg_text = "Game started!";
                    *data->start_date = time(NULL);
                }else data->msg_text = "Game not started!";
                break;

            case 410: // resize event
                wresize(data->scoreboard, LINES-3, 24);
                wresize(data->win, LINES-3, COLS - 24);
                wresize(data->msg, 3, COLS);
                VueNCURSES_draw_border(data);
                data->msg_text = "Resized!";
                break;
            default:
                break;
        }

        const GameState state = Controller_get_state(data->self->game->controller);

        const time_t crt = *data->start_date;
        const int timer = (int)(time(NULL) - crt);
        if (state == GAME_STATE_PLAYING && timer == DELAY) {
            data->msg_text = "Started!";
            wrefresh(data->msg);
        }else if (state == GAME_STATE_PLAYING && timer > DELAY){
            Controller_update(data->self->game->controller);

        }else if (state == GAME_STATE_PLAYING) {
            char msg[100];
            sprintf(msg, "Starting in %i seconds...", DELAY - timer);
            data->msg_text = msg;
            wrefresh(data->msg);
        }

        VueNCURSES_draw_window(data);

        usleep((int)(1.0f / 5.0f * 1000000.0f));
    }
}

void VueNCURSES_destroy(NCURSESData *data) {
    // destroy the windows
    delwin(data->win);
    delwin(data->scoreboard);
    delwin(data->msg);
    free(data->start_date);
    // end ncurses
    endwin();
}

int VueNCURSES_main(Vue *self) {
    time_t* tim = malloc(sizeof(time_t));
    if (tim == NULL)
        return 1;
    *tim = time(NULL);

    NCURSESData data = {
        NULL,
        NULL,
        NULL,
        self,
        "Press * to start the game",
        tim
    };

    VueNCURSES_init(&data);
    VueNCURSES_loop(&data);
    VueNCURSES_destroy(&data);

    return 0;
}

