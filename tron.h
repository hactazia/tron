#ifndef TRON_H
#define TRON_H

#include "controller.h"
#include "model.h"
#include "vue.h"

typedef struct Tron {
    Controller *controller;
    Model *model;
    Vue *vue;
} Tron;

/**
 * @brief Create and initialize a Tron game instance.
 * @param model Pointer to the Model instance.
 * @param vue Pointer to the Vue instance.
 * @param controller Pointer to the Controller instance.
 * @return Pointer to the newly created Tron game instance.
 */
Tron *create_tron(Model *model, Vue *vue, Controller *controller);

#endif // TRON_H
