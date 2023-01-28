#ifndef LEVEL_H
#define LEVEL_H

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

void addLevelTiles(unsigned char length, TileLayout layout[]);

#endif
