#include "utils.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

int compose_flags(char **argv, const int argc)
{
    int flag = 0;

    // check for flags
    for (int i = 1; i < argc; i++)
    {
        debug_logf("Flag: %s", argv[i]);
        // if SDL flag is found, add it to the flags
        if (strcmp(argv[i], SDL_FLAG_PROMPT) == 0)
        {
            debug_logf("SDL flag found: %s", argv[i]);
            flag |= SDL_FLAG;
        }

        // if ncurses flag is found, add it to the flags
        else if (strcmp(argv[i], NCURSES_FLAG_PROMPT) == 0)
        {
            debug_logf("NCURSES flag found: %s", argv[i]);
            flag |= NCURSES_FLAG;
        }
    }

    // if it has both flags, remove the ncurses flag
    if (flag & SDL_FLAG && flag & NCURSES_FLAG)
    {
        debug_logf("Both flags found: %d", flag);
        flag &= ~SDL_FLAG;
    }

    // if it has no flags, add the ncurses flag
    if ((flag & SDL_FLAG) == 0 && (flag & NCURSES_FLAG) == 0)
    {
        debug_logf("No flags found: %d", flag);
        flag |= NCURSES_FLAG;
    }

    return flag;
}

void debug_log(const char *content)
{
    FILE *file = fopen("tron.log", "a");
    if (file == NULL)
        return;

    fprintf(file, "%s\n", content);
    fclose(file);
}

void debug_logf(const char* format, ...)
{
    FILE* file = fopen("tron.log", "a");
    if (file == NULL)
        return;

    va_list args;
    va_start(args, format);
    vfprintf(file, format, args);
    fprintf(file, "\n");
    va_end(args);

    fclose(file);
}

void clear_log()
{
    FILE* file = fopen("tron.log", "w");
    if (file == NULL)
        return;

    fclose(file);
}

void sort_array2d(int arr[][2], const int n)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j][1] < arr[j + 1][1])
            {
                const int temp = arr[j][1];
                arr[j][1] = arr[j + 1][1];
                arr[j + 1][1] = temp;
                const int temp2 = arr[j][0];
                arr[j][0] = arr[j + 1][0];
                arr[j + 1][0] = temp2;
            }
}