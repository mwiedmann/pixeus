#ifndef GAMELEVELS_H
#define GAMELEVELS_H

#include "x16graphics.h"

typedef struct LevelLayout {
    unsigned char x;
    unsigned char y;
    unsigned char length;
    unsigned char type;
} LevelLayout;

#endif