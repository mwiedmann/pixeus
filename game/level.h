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

typedef struct Entrance {
    unsigned short x;
    unsigned short y;
    unsigned char *name;
} Entrance;

typedef struct EntranceList {
    unsigned short length;
    Entrance *entrances;
} EntranceList;

typedef struct Exit {
    unsigned short x;
    unsigned short y;
    unsigned char level;
    unsigned char *entrance;
} Exit;

typedef struct ExitList {
    unsigned short length;
    Exit *exits;
} ExitList;

typedef struct LevelOveralLayout {
    SolidLayoutList *solidList;
    TileLayoutList *tilesList;
    EnemyLayoutList *enemiesList;
    EntranceList *entranceList;
    ExitList *exitList;
} LevelOveralLayout;

typedef struct GameLayout {
    unsigned short levelsLength;
    LevelOveralLayout *levels;
} GameLayout;

void addLevelTiles(unsigned short length, TileLayout layout[]);

#endif
