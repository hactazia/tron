#ifndef VUE_H
#define VUE_H

typedef struct Tron Tron; // Forward declaration

typedef struct Vue {
    Tron *game;
    int (*main)(struct Vue *self);
} Vue;

#endif // VUE_H
