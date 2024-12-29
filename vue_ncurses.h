#ifndef VUE_NCURSES_H
#define VUE_NCURSES_H

#include <time.h>
#include <ncurses.h>

#include "vue.h"

#define SAVED_KEYS 5

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

/**
 * @brief Main function for the NCURSES Vue
 * @param self The Vue
 */
int VueNCURSES_main(Vue *self);

/**
 * @brief Draw the border for the NCURSES view
 * @param data The NCURSES data
 */
void VueNCURSES_draw_border(NCURSESData* data);

/**
 * @brief Initialize the NCURSES view
 * @param data The NCURSES data
 */
void VueNCURSES_init(NCURSESData* data);

/**
 * @brief Estimate the value based on the width
 * @param a The value to estimate
 * @param w1 The original width
 * @param w2 The new width
 * @return The estimated value
 */
int VueNCURSES_estimate(const int a, const int w1, const int w2);

/**
 * @brief Get the length of a number
 * @param number The number
 * @return The length of the number
 */
int VueNCURSES_number_length(int number);

/**
 * @brief Draw the window for the NCURSES view
 * @param data The NCURSES data
 */
void VueNCURSES_draw_window(NCURSESData* data);

/**
 * @brief Main loop for the NCURSES view
 * @param data The NCURSES data
 */
void VueNCURSES_loop(NCURSESData* data);

/**
 * @brief Destroy the NCURSES view
 * @param data The NCURSES data
 */
void VueNCURSES_destroy(NCURSESData* data);

#endif // VUE_NCURSES_H
