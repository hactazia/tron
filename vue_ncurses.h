#ifndef VUE_NCURSES_H
#define VUE_NCURSES_H

#include <time.h>

#include "ncurses.h"
#include "vue.h"

typedef struct VueNCURSES {
    Vue base;
} VueNCURSES;

typedef struct NCURSESData {
    WINDOW *win;
    WINDOW *scoreboard;
    WINDOW *msg;
    Vue *self;
    char *msg_text;
    time_t* start_date;
} NCURSESData;

int VueNCURSES_main(Vue *self);
void VueNCURSES_gui(WINDOW **win, WINDOW **scoreboard, Vue *self);

#endif // VUE_NCURSES_H
