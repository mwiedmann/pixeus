#ifndef LEVEL_H
#define LEVEL_H

#include "layoutdefs.h"

typedef struct TileInfo {
    unsigned char x;
    unsigned char y;
    unsigned char type;
} TileInfo;

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
    unsigned char moveDir;
    unsigned char patrolA;
    unsigned char patrolB;
    unsigned char patrolDir;
    EnemyType enemyType;
    EnemyMoveType moveType;
} EnemyLayout;

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
    unsigned char id;
} Entrance;

typedef struct EntranceList {
    unsigned short length;
    Entrance *entrances;
} EntranceList;

typedef struct Exit {
    unsigned short x;
    unsigned short y;
    unsigned char level;
    unsigned char entranceId;
} Exit;

typedef struct ExitList {
    unsigned short length;
    Exit *exits;
} ExitList;

typedef struct LevelOveralLayout {
    unsigned char levelNum;
    TileLayoutList *tilesList;
    EnemyLayoutList *enemiesList;
    EntranceList *entranceList;
    ExitList *exitList;
    unsigned char *movementTypes;
} LevelOveralLayout;

void addLevelTiles(unsigned short length, TileLayout layout[]);

#endif
