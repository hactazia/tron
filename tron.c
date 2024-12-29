#include "tron.h"

#include <stdlib.h>

Tron *create_tron(Model *model, Vue *vue, Controller *controller) {
    Tron *tron = malloc(sizeof(Tron));
    tron->controller = controller;
    tron->model = model;
    tron->vue = vue;

    controller->game = tron;
    model->game = tron;
    vue->game = tron;

    return tron;
}
