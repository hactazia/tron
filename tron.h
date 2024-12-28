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

Tron *create_tron(Model *model, Vue *vue, Controller *controller);

#endif // TRON_H
