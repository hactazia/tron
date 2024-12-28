#ifndef UTILS_H
#define UTILS_H

#define SDL_FLAG_PROMPT "-sdl"
#define SDL_FLAG 1
#define NCURSES_FLAG_PROMPT "-ncurses"
#define NCURSES_FLAG 2
#include <string.h>

/**
 * @brief Compose the flags from the arguments
 * @param argv String array of arguments
 * @param argc Number of arguments
 * @return The composed flags
 */
int compose_flags(char **argv, const int argc);

#endif //UTILS_H
