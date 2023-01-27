#ifndef GAMELEVELS_H
#define GAMELEVELS_H

#include "x16graphics.h"

#define TEST_LEVEL_SOLID_LENGTH 5
#define TEST_LEVEL_TILES_LENGTH 23
#define CLOUD_TILES_1_LENGTH 9

typedef struct LevelLayout {
    unsigned char x;
    unsigned char y;
    unsigned char length;
    unsigned char type;
} LevelLayout;

typedef struct TileLayout {
    unsigned char x;
    unsigned char y;
    unsigned char length;
    unsigned char type;
    unsigned char layer;
} TileLayout;

#endif