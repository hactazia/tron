#ifndef UTILS_H
#define UTILS_H

#define SDL_FLAG_PROMPT "-sdl"
#define SDL_FLAG 1
#define NCURSES_FLAG_PROMPT "-ncurses"
#define NCURSES_FLAG 2

/**
 * @brief Compose the flags from the arguments
 * @param argv String array of arguments
 * @param argc Number of arguments
 * @return The composed flags
 */
int compose_flags(char **argv, const int argc);

/**
 * @brief Log a message to a file
 * @param content The content to log
 */
void debug_log(const char *content);

/**
 * @brief Format a string and log it to a file
 * @param format The format of the string
 * @param ... The arguments to format
 * @note This function is similar to printf
 */
void debug_logf(const char *format, ...);

/**
 * @brief Clear the log file
 */
void clear_log();

/**
 * @brief Sort a 2D array
 * @param array The array to sort
 * @param size The size of the array
 * @note use only the first column to sort
 */
void sort_array2d(int array[][2], const int size);

#endif //UTILS_H