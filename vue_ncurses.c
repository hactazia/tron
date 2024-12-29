#include "vue_ncurses.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "utils.h"
#include "tron.h"

void VueNCURSES_draw_border(NCURSESData* data)
{
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

void VueNCURSES_init(NCURSESData* data)
{
    // a window for the game preview
    // a window for the scoreboards
    // a window for the messages

    // initialize ncurses
    initscr();

    // create the game preview window
    data->scoreboard = newwin(LINES - 3, 24, 0, 0);
    data->win = newwin(LINES - 3, COLS - 24, 0, 24);
    data->msg = newwin(3, COLS, LINES - 3, 0);

    // create the minimal player count required
    while (Controller_get_player_count(data->self->game->controller) < MIN_PLAYER)
        Controller_new_player(data->self->game->controller);
}

int VueNCURSES_estimate(const int a, const int w1, const int w2)
{
    if (w1 == w2)
        return a;
    if (w1 == 0)
        return 0;
    return w2 * a / w1;
}

int VueNCURSES_number_length(int number)
{
    if (number == 0)
        return 1;
    int length = 0;
    while (number != 0)
    {
        number /= 10;
        length++;
    }
    return length;
}

void VueNCURSES_draw_window(NCURSESData* data)
{
    // modify the message window
    box(data->msg, 0, 0);
    char clear[getmaxx(data->msg) - 2];
    memset(clear, ' ', getmaxx(data->msg) - 2);
    clear[getmaxx(data->msg) - 2] = '\0';
    mvwprintw(data->msg, 1, 1, clear);
    mvwprintw(data->msg, 1, (getmaxx(data->msg) - strlen(data->msg_text)) / 2, data->msg_text);
    wrefresh(data->msg);

    // modify the scoreboard window
    box(data->scoreboard, 0, 0);
    const char* scoreboard_text = "Scoreboard";
    mvwprintw(data->scoreboard, 1, (getmaxx(data->scoreboard) - strlen(scoreboard_text)) / 2, scoreboard_text);

    // sort players by score
    const int player_count = Controller_get_player_count(data->self->game->controller);
    int scoreboard[player_count][2];
    for (int i = 0; i < player_count; i++)
    {
        const Player* player = Controller_get_player(data->self->game->controller, i);
        scoreboard[i][0] = i;
        scoreboard[i][1] = player->score;
    }
    sort_array2d(scoreboard, player_count);

    // show top players
    for (int i = 0; i < player_count; i++)
    {
        Player* player = Controller_get_player(data->self->game->controller, scoreboard[i][0]);
        mvwprintw(
            data->scoreboard,
            i + 3,
            2,
            "Player %d %s",
            scoreboard[i][0],
            player->state == PLAYER_STATE_ALIVE ? "    " : "Dead"
        );
        mvwprintw(
            data->scoreboard,
            i + 3,
            getmaxx(data->scoreboard) - VueNCURSES_number_length(scoreboard[i][1]) - 2,
            "%d",
            scoreboard[i][1]
        );
    }
    wrefresh(data->scoreboard);

    // modify the game preview window
    box(data->win, 0, 0);
    const int width = Controller_get_width(data->self->game->controller);
    const int height = Controller_get_height(data->self->game->controller);
    const int width_win = getmaxx(data->win) - 2;
    const int height_win = getmaxy(data->win) - 2;

    // render the walls
    const int wall_count = Controller_get_wall_count(data->self->game->controller);
    for (int i = 0; i < wall_count; i++)
    {
        const Wall* wall = Controller_get_wall(data->self->game->controller, i);
        const int x = VueNCURSES_estimate(wall->x, width, width_win);
        const int y = VueNCURSES_estimate(wall->y, height, height_win);

        switch (wall->direction)
        {
        case DIRECTION_UP:
            mvwvline(data->win, y+1, x+1, '|', wall->length);
            break;
        case DIRECTION_LEFT:
            mvwhline(data->win, y+1, x+1, '-', wall->length);
            break;
        case DIRECTION_DOWN:
            mvwvline(data->win, y - wall->length+1, x+1, '|', wall->length);
            break;
        case DIRECTION_RIGHT:
            mvwhline(data->win, y+1, x - wall->length+1, '-', wall->length);
            break;
        default: ;
        }
    }

    // render the players
    for (int i = 0; i < player_count; i++)
    {
        const Player* player = Controller_get_player(data->self->game->controller, i);
        const int x = VueNCURSES_estimate(player->x, width, width_win);
        const int y = VueNCURSES_estimate(player->y, height, height_win);
        int distance;
        Wall wall;
        Controller_get_player_wall(data->self->game->controller, i, &wall, &distance);
        if (distance > 0)
        {
            switch (player->direction)
            {
            case DIRECTION_UP:
                mvwvline(data->win, y+2, x+1, '|', distance);
                break;
            case DIRECTION_LEFT:
                mvwhline(data->win, y+1, x+2, '-', distance);
                break;
            case DIRECTION_DOWN:
                mvwvline(data->win, y - distance+1, x+1, '|', distance);
                break;
            case DIRECTION_RIGHT:
                mvwhline(data->win, y+1, x - distance+1, '-', distance);
                break;
            default: ;
            }
        }

        // render the player
        if (Controller_out_of_bounds(data->self->game->controller, player->x, player->y))
            mvwprintw(data->win, y + 1, x + 1, "X");
        else if (player->state == PLAYER_STATE_DEAD)
            mvwprintw(data->win, y + 1, x + 1, "D");
        else mvwprintw(data->win, y + 1, x + 1, "%d", i);
    }

    wrefresh(data->win);
}

void VueNCURSES_loop(NCURSESData* data)
{
    bool running = true;
    nodelay(stdscr, TRUE); // getch non-blocking

    // saved keys (for multi-key combinations)
    int ch[SAVED_KEYS];
    for (int i = 0; i < SAVED_KEYS; i++)
        ch[i] = ERR;

    while (running)
    {
        bool playing = GAME_STATE_PLAYING == Controller_get_state(data->self->game->controller);

        // read all new keys
        do
        {
            for (int i = SAVED_KEYS - 1; i > 0; i--)
                ch[i] = ch[i - 1];
            ch[0] = getch();

            if (ch[0] == '=' && !playing)
            {
                // Exit event
                running = false;
                data->msg_text = "Goodbye!";
            }
            else if (ch[0] == '=' && playing)
            {
                // Cancel game event
                Controller_cancel(data->self->game->controller);
                data->msg_text = "Game over!";
            }
            else if (ch[0] == '-' && !playing && Controller_get_player_count(data->self->game->controller) > MIN_PLAYER)
            {
                // Remove player event
                Controller_remove_player(data->self->game->controller, 0);
                data->msg_text = "Player removed!";
                wclear(data->scoreboard);
                box(data->scoreboard, 0, 0);
                wrefresh(data->scoreboard);
            }
            else if (ch[0] == '+' && !playing && Controller_get_player_count(data->self->game->controller) <
                MAX_PLAYERS)
            {
                // Add player event
                Controller_new_player(data->self->game->controller);
                data->msg_text = "Player added!";
            }
            else if (ch[0] == '*' && !playing)
            {
                // Start event
                Controller_play(data->self->game->controller, getmaxx(data->win) - 2, getmaxy(data->win) - 2);
                debug_logf("Game state: %d", Controller_get_state(data->self->game->controller));
                if (GAME_STATE_PLAYING == Controller_get_state(data->self->game->controller))
                {
                    data->msg_text = "Game started!";
                    debug_log("Game started!");
                    *data->start_date = time(NULL);
                }
                else
                {
                    data->msg_text = "Game not started!";
                    debug_log("Game not started!");
                }
            }
            else if (ch[0] == 410)
            {
                // Resize event
                wresize(data->scoreboard, LINES - 3, 24);
                wresize(data->win, LINES - 3, COLS - 24);
                wresize(data->msg, 3, COLS);
                VueNCURSES_draw_border(data);
                data->msg_text = "Resized!";
            }

            // movement keys
            // arrow for player 0
            else if (ch[0] == 65 && ch[1] == 91 && ch[2] == 27 && playing) // Move player 0 UP
                Controller_move_player(data->self->game->controller, 0, DIRECTION_UP);
            else if (ch[0] == 66 && ch[1] == 91 && ch[2] == 27 && playing) // Move player 0 DOWN
                Controller_move_player(data->self->game->controller, 0, DIRECTION_DOWN);
            else if (ch[0] == 68 && ch[1] == 91 && ch[2] == 27 && playing) // Move player 0 LEFT
                Controller_move_player(data->self->game->controller, 0, DIRECTION_LEFT);
            else if (ch[0] == 67 && ch[1] == 91 && ch[2] == 27 && playing) // Move player 0 RIGHT
                Controller_move_player(data->self->game->controller, 0, DIRECTION_RIGHT);
                // zqsd for player 1
            else if (ch[0] == 'z' && playing) // Move player 1 UP
                Controller_move_player(data->self->game->controller, 1, DIRECTION_UP);
            else if (ch[0] == 's' && playing) // Move player 1 DOWN
                Controller_move_player(data->self->game->controller, 1, DIRECTION_DOWN);
            else if (ch[0] == 'q' && playing) // Move player 1 LEFT
                Controller_move_player(data->self->game->controller, 1, DIRECTION_LEFT);
            else if (ch[0] == 'd' && playing) // Move player 1 RIGHT
                Controller_move_player(data->self->game->controller, 1, DIRECTION_RIGHT);
                // ijkl for player 2
            else if (ch[0] == 'i' && playing) // Move player 2 UP
                Controller_move_player(data->self->game->controller, 2, DIRECTION_UP);
            else if (ch[0] == 'k' && playing) // Move player 2 DOWN
                Controller_move_player(data->self->game->controller, 2, DIRECTION_DOWN);
            else if (ch[0] == 'j' && playing) // Move player 2 LEFT
                Controller_move_player(data->self->game->controller, 2, DIRECTION_LEFT);
            else if (ch[0] == 'l' && playing) // Move player 2 RIGHT
                Controller_move_player(data->self->game->controller, 2, DIRECTION_RIGHT);
                // tfgh for player 3
            else if (ch[0] == 't' && playing) // Move player 3 UP
                Controller_move_player(data->self->game->controller, 3, DIRECTION_UP);
            else if (ch[0] == 'g' && playing) // Move player 3 DOWN
                Controller_move_player(data->self->game->controller, 3, DIRECTION_DOWN);
            else if (ch[0] == 'f' && playing) // Move player 3 LEFT
                Controller_move_player(data->self->game->controller, 3, DIRECTION_LEFT);
            else if (ch[0] == 'h' && playing) // Move player 3 RIGHT
                Controller_move_player(data->self->game->controller, 3, DIRECTION_RIGHT);
                // 5123 for player 4
            else if (ch[0] == '5' && playing) // Move player 4 UP
                Controller_move_player(data->self->game->controller, 4, DIRECTION_UP);
            else if (ch[0] == '2' && playing) // Move player 4 DOWN
                Controller_move_player(data->self->game->controller, 4, DIRECTION_DOWN);
            else if (ch[0] == '1' && playing) // Move player 4 LEFT
                Controller_move_player(data->self->game->controller, 4, DIRECTION_LEFT);
            else if (ch[0] == '3' && playing) // Move player 4 RIGHT
                Controller_move_player(data->self->game->controller, 4, DIRECTION_RIGHT);
                // -789 for player 5
            else if (ch[0] == '-' && playing) // Move player 5 UP
                Controller_move_player(data->self->game->controller, 5, DIRECTION_UP);
            else if (ch[0] == '7' && playing) // Move player 5 DOWN
                Controller_move_player(data->self->game->controller, 5, DIRECTION_DOWN);
            else if (ch[0] == '8' && playing) // Move player 5 LEFT
                Controller_move_player(data->self->game->controller, 5, DIRECTION_LEFT);
            else if (ch[0] == '9' && playing) // Move player 5 RIGHT
                Controller_move_player(data->self->game->controller, 5, DIRECTION_RIGHT);
        }
        while (ch[0] != ERR);

        GameState state = Controller_get_state(data->self->game->controller);

        // calculate the time since the game started
        const time_t crt = *data->start_date;
        const long timer = time(NULL) - crt;

        // comportment based on the game state
        if (state == GAME_STATE_PLAYING && timer >= DELAY)
        {
            // when the game is playing, process the game
            if (timer == DELAY)
            {
                // message when the game starts
                data->msg_text = "Game started!";
                wrefresh(data->msg);
            }

            // update the game state
            Controller_update(data->self->game->controller);
            state = Controller_get_state(data->self->game->controller);

            if (state == GAME_STATE_GAME_OVER)
            {
                // if the game is over, show the winner
                data->msg_text = "Game over!";
                wrefresh(data->msg);
                char msg[100] = "\0";

                for (int i = 0; i < Controller_get_player_count(data->self->game->controller); i++)
                {
                    Player* player = Controller_get_player(data->self->game->controller, i);
                    if (player->state != PLAYER_STATE_ALIVE) continue;
                    sprintf(msg, "Winner is player %d with %d points!", i, player->score);
                    data->msg_text = msg;
                }

                if (strlen(msg) == 0)
                    data->msg_text = "No winner!";
                wrefresh(data->msg);
            }
        }
        else if (state == GAME_STATE_PLAYING && timer == 0)
        {
            // clear the game preview and the scoreboard when the game starts
            wclear(data->win);
            box(data->win, 0, 0);
            wclear(data->scoreboard);
            box(data->scoreboard, 0, 0);
            wrefresh(data->win);
            wrefresh(data->scoreboard);
        }
        else if (state == GAME_STATE_PLAYING)
        {
            // when the game is playing and the timeout is not over, show the countdown
            char msg[100];
            sprintf(msg, "Starting in %ld seconds...", DELAY - timer);
            data->msg_text = msg;
            wrefresh(data->msg);
        }

        // render
        VueNCURSES_draw_window(data);

        usleep(1000000 / GAME_FPS);
    }
}

void VueNCURSES_destroy(NCURSESData* data)
{
    // destroy the windows
    delwin(data->win);
    delwin(data->scoreboard);
    delwin(data->msg);
    free(data->start_date);
    // end ncurses
    endwin();
}

int VueNCURSES_main(Vue* self)
{
    debug_log("VueNCURSES_main");

    // initialize the time since the game started
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

    // free memory
    free(tim);

    return 0;
}
