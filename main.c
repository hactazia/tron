#include <stdlib.h>
#include "tron.h"
#include "utils.h"
#include "vue_sdl.h"
#include "vue_ncurses.h"

int main(const int argc, char** argv)
{
    // Clear the log file
    clear_log();
    debug_log("Creating Tron game");

    // Compose flags from command line arguments
    const int flags = compose_flags(argv, argc);
    debug_logf("Flags: %d", flags);

    // Initialize controller and model
    Controller controller = {NULL};
    Model model = {NULL};

    Tron* tron;

    // Check if SDL flag is set
    if (flags & SDL_FLAG)
    {
        // Initialize SDL view
        VueSDL vue_sdl = {
            {
                NULL,
                VueSDL_main
            }
        };
        // Create Tron game with SDL view
        tron = create_tron(&model, (Vue*)&vue_sdl, &controller);
    }
    // Check if NCURSES flag is set
    else if (flags & NCURSES_FLAG)
    {
        // Initialize NCURSES view
        VueNCURSES vue_ncurses = {
            {
                NULL,
                VueNCURSES_main
            }
        };
        // Create Tron game with NCURSES view
        tron = create_tron(&model, (Vue*)&vue_ncurses, &controller);
    }
    else
    {
        // No valid flags found, log and return error
        debug_log("No flags found");
        return 1;
    }

    // Run the main function of the view
    int io = tron->vue->main(tron->vue);

    // Free the Tron game instance
    free(tron);

    return io;
}