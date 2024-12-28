//
// Created by hactazia on 22/12/2024.
//

#include "utils.h"

int compose_flags(char **argv, const int argc)
{
    int flag = 0;

    // check for flags
    for (int i = 1; i < argc; i++)
    {
        // if SDL flag is found, add it to the flags
        if (strcmp(argv[i], SDL_FLAG_PROMPT) == 0)
            flag |= SDL_FLAG;

        // if ncurses flag is found, add it to the flags
        else if (strcmp(argv[i], NCURSES_FLAG_PROMPT) == 0)
            flag |= NCURSES_FLAG;
    }
    // if it has both flags, remove the ncurses flag
    if (flag & (SDL_FLAG | NCURSES_FLAG))
        flag &= ~SDL_FLAG;

    // if it has no flags, add the ncurses flag
    if ((flag & (SDL_FLAG | NCURSES_FLAG)) == 0)
        flag |= NCURSES_FLAG;

    return flag;
}
