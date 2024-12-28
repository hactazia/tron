#include <stdlib.h>
#include "tron.h"
#include "utils.h"
#include "vue_sdl.h"
#include "vue_ncurses.h"

int main(const int argc, char **argv) {
    const int flags = compose_flags(argv, argc);

    Controller controller = {NULL};
    Model model = {NULL};

    Tron * tron;
    if (flags & SDL_FLAG) {
        VueSDL vue_sdl = {
            {
                NULL,
                VueSDL_main
            }
        };
        tron = create_tron(&model, (Vue *)&vue_sdl, &controller);
    } else if (flags & NCURSES_FLAG) {
        VueNCURSES vue_ncurses = {
            {
                NULL,
                VueNCURSES_main
            }
        };
        tron = create_tron(&model, (Vue *)&vue_ncurses, &controller);
    }

    return tron->vue->main(tron->vue);
}
