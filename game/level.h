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

typedef struct TileLayoutList {
    unsigned char tilesetId;
    unsigned short length;
    TileLayout *tiles;
} TileLayoutList;

typedef struct EnemyLayout {
    unsigned char x;
    unsigned char y;
    unsigned char moveDir;
    unsigned char patrolA;
    unsigned char patrolB;
    unsigned char patrolDir;
    unsigned char enemyType;
    unsigned char moveType;
} EnemyLayout;

typedef struct EnemyLayoutList {
    unsigned char length;
    EnemyLayout *enemies;
} EnemyLayoutList;

typedef enum { EntranceEnum = 0, ExitEnum = 1, EnergyEnum = 3, GoldEnum = 4 } EntityType;

typedef struct Entity {
    unsigned char entityType;
    unsigned char x;
    unsigned char y;
    unsigned char arg0;
    unsigned char arg1;
} Entity;

typedef struct EntityList {
    unsigned char length;
    Entity *entities;
} EntityList;

// Entity can be cast to any of the specialized structs here for clarity

typedef struct Entrance {
    unsigned char entityType;
    unsigned char x;
    unsigned char y;
    unsigned char id;
    unsigned char unused1;
} Entrance;

typedef struct EntranceList {
    unsigned char length;
    Entrance *entrances;
} EntranceList;

typedef struct Exit {
    unsigned char entityType;
    unsigned char x;
    unsigned char y;
    unsigned char level;
    unsigned char entranceId;
} Exit;

typedef struct ExitList {
    unsigned char length;
    Exit *exits;
} ExitList;

typedef struct Energy {
    unsigned char entityType;
    unsigned char x;
    unsigned char y;
    unsigned char amount;
    unsigned char unused1;
} Energy;

typedef struct EnergyList {
    unsigned char length;
    Energy *energies;
} EnergyList;

typedef struct Gold {
    unsigned char entityType;
    unsigned char x;
    unsigned char y;
    unsigned char amount;
    unsigned char unused1;
} Gold;

typedef struct GoldList {
    unsigned char length;
    Gold *goldpiles;
} GoldList;

typedef struct LevelOveralLayout {
    unsigned char levelNum;
    TileLayoutList *tileList;
    EnemyLayoutList *enemyList;
    EntityList *entityList;
    unsigned char *movementTypes;
} LevelOveralLayout;

void addLevelTiles(unsigned short length, TileLayout layout[]);

#endif
