#ifndef LEVEL_H
#define LEVEL_H

#include "enemies.h"

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
    unsigned char vFlip;
    unsigned char hFlip;
} TileLayout;

typedef struct EnemyLayout {
    unsigned char x;
    unsigned char y;
    unsigned char length;
    EnemyType enemyType;
    EnemyMoveType moveType;
} EnemyLayout;

typedef struct SolidLayoutList {
    unsigned short length;
    SolidLayout *solid;
} SolidLayoutList;

typedef struct TileLayoutList {
    unsigned short length;
    TileLayout *tiles;
} TileLayoutList;

typedef struct EnemyLayoutList {
    unsigned short length;
    EnemyLayout *enemies;
} EnemyLayoutList;

typedef struct LevelOveralLayout {
    unsigned short solidListLength;
    SolidLayoutList *solidList;
    unsigned short tilesListLength;
    TileLayoutList *tilesList;
    unsigned short enemiesListLength;
    EnemyLayoutList *enemiesList;
} LevelOveralLayout;

void addLevelTiles(unsigned char length, TileLayout layout[]);

#endif
