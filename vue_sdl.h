#ifndef VUE_SDL_H
#define VUE_SDL_H

#include "vue.h"

typedef struct VueSDL {
    Vue base;
} VueSDL;

void VueSDL_render(Vue *self);
int VueSDL_main(Vue *self);

#endif // VUE_SDL_H
