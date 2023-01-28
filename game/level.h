#ifndef LEVEL_H
#define LEVEL_H

typedef struct SolidLayout {
    unsigned char x;
    unsigned char y;
    unsigned char length;
    unsigned char type;
} SolidLayout;

typedef struct TileLayout {
    unsigned char x;
    unsigned char y;
    unsigned char length;
    unsigned char type;
    unsigned char layer;
} TileLayout;

typedef struct SolidLayoutList {
    unsigned short length;
    SolidLayout *solid;
} SolidLayoutList;

typedef struct TileLayoutList {
    unsigned short length;
    TileLayout *tiles;
} TileLayoutList;

typedef struct LevelOveralLayout {
    unsigned short solidLength;
    SolidLayoutList *solid;
    unsigned short tilesLength;
    TileLayoutList *tiles;
} LevelOveralLayout;

void addLevelTiles(unsigned char length, TileLayout layout[]);

#endif
