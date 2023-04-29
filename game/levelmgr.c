#include <cx16.h>
#include <cbm.h>
#include <stdio.h>
#include <stdlib.h>
#include <peekpoke.h>
#include "x16graphics.h"
#include "memmap.h"
#include "level.h"
#include "entitymgr.h"
#include "gametiles.h"

LevelOveralLayout *levelGet(unsigned char levelNum) {
    unsigned short x;
    unsigned char tilesetId;
    unsigned short tilesLength;
    unsigned char entitiesLength;
    unsigned char enemiesLength;
    unsigned char leftLevel;
    unsigned char rightLevel;
    unsigned char downLevel;
    unsigned char upLevel;
    unsigned char *cachedEntityTypes;

    char filename[32];
    unsigned char *movementTypes;
    
    TileLayout *tiles;
    TileLayoutList *tileList;
    Entity *entities;
    EntityList *entityList;
    EnemyLayout *enemies;
    EnemyLayoutList *enemyList;
    LevelOveralLayout *level;
    
    unsigned short ramstart = (unsigned short)BANK_RAM;
    sprintf(filename, "level%u.bin", levelNum);

    // Load the level file into Banked RAM
    POKE(0, LEVEL_BANK);
    cbm_k_setnam(filename);
    cbm_k_setlfs(0, 8, 0);
    cbm_k_load(0, (unsigned short)ramstart);

    // The level file has a structure that starts with
    // the lengths of everything.
    tilesetId = *(unsigned char*)(ramstart);
    leftLevel = *(unsigned char*)(ramstart+1);
    rightLevel = *(unsigned char*)(ramstart+2);
    downLevel = *(unsigned char*)(ramstart+3);
    upLevel = *(unsigned char*)(ramstart+4);
    tilesLength = *(unsigned short*)(ramstart+5);
    entitiesLength = *(unsigned char*)(ramstart+7);
    enemiesLength = *(unsigned char*)(ramstart+8);
    
    // The tilesetId, level Ids, and lengths take up 9 bytes
    ramstart+= 9;

    // Movement Types is always a fixed 40x30 size so we don't need a length
    // Point movementTypes to the data in HI RAM and advance the pointer 1200
    movementTypes = (unsigned char*)ramstart;
    ramstart+= 1200;

    // TILES
    // Point tiles to the data in HI RAM
    tiles = (TileLayout *)ramstart;

    // Allocate the structure for the list/length
    tileList = malloc(5);
    tileList->tilesetId = tilesetId;
    tileList->length = tilesLength;
    tileList->tiles = tiles;

    ramstart+= tilesLength * sizeof(TileLayout);

    // Entities
    // Copy the entity data from HIRAM byte-by-byte
    entities = malloc(entitiesLength * sizeof(Entity));
    for (x=0; x<entitiesLength * sizeof(Entity); x++) {
        ((unsigned char*)entities)[x] = *(unsigned char*)(ramstart+x);
    }
    entityList = malloc(4);
    entityList->length = entitiesLength;
    entityList->entities = entities;

    // See if we have already cached the entity list (entityType for each entity)
    // We cache this because entities like energy and gold don't respawn
    cachedEntityTypes = cachedEntityListGet(levelNum);
    if (cachedEntityTypes != 0) {
        for (x=0; x < entitiesLength; x++) {
            entities[x].entityType = cachedEntityTypes[x];
        }
    }

    ramstart+= entitiesLength * sizeof(Entity);

    // Enemies
    // Point enemies to the data in HI RAM
    enemies = (EnemyLayout *)ramstart;
    
    enemyList = malloc(4);
    enemyList->length = enemiesLength;
    enemyList->enemies = enemies;
    
    ramstart+= enemiesLength * sizeof(EnemyLayout);

    // Overall
    level = malloc(sizeof(LevelOveralLayout));
    level->levelNum = levelNum;
    level->leftLevel = leftLevel;
    level->rightLevel = rightLevel;
    level->downLevel = downLevel;
    level->upLevel = upLevel;
    level->tileList = tileList;
    level->enemyList = enemyList;
    level->entityList = entityList;
    level->movementTypes = movementTypes;

    // Cache any new level data
    cacheLevelData(level);

    return level;
}

void freeLevel(LevelOveralLayout *level) {
    free(level->tileList);
    free(level->enemyList);
    free(level->entityList->entities);
    free(level->entityList);
    free(level);
}

void layerMapsClear() {
    unsigned short x, y;
    
    vMemSetIncMode(1);

    // Clear Layer 0 (background)
    vMemSetBank(LAYER0_MAP_MEM_BANK);
    vMemSetAddr(LAYER0_MAP_MEM);
    for (y=0; y<TILES_DOWN; y++) {
        for (x=0; x<TILES_ACROSS; x++) {
            vMemSetData0(0);
            vMemSetData0(0);
        }
    }

    // Clear Layer 1 (foreground)
    vMemSetBank(LAYER1_MAP_MEM_BANK);
    vMemSetAddr(LAYER1_MAP_MEM);
    for (y=0; y<TILES_DOWN; y++) {
        for (x=0; x<TILES_ACROSS; x++) {     
            vMemSetData0(0);
            vMemSetData0(0);
        }
    }
}

void layerMapsLevelInit(LevelOveralLayout *level) {
    vMemSetIncMode(1);
    addLevelTiles(level->tileList->length, level->tileList->tiles);
}

