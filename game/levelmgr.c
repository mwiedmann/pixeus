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
    sprintf(filename, "levels/level%u.bin", levelNum);

    // Load the level file into Banked RAM
    POKE(0, LEVEL_BANK);
    cbm_k_setnam(filename);
    cbm_k_setlfs(0, 8, 0);
    cbm_k_load(0, (unsigned int)ramstart);

    // The level file has a structure that starts with
    // the lengths of everything.
    tilesetId = *(unsigned char*)(ramstart);
    tilesLength = *(unsigned short*)(ramstart+1);
    entitiesLength = *(unsigned char*)(ramstart+3);
    enemiesLength = *(unsigned char*)(ramstart+4);
    
    // The lengths take up 5 bytes
    ramstart+= 5;

    // Movement Types is always a fixed 40x30 size so we don't need a length
    // Point movementTypes to the data in HI RAM and advance the pointer 1200
    movementTypes = ramstart;
    ramstart+= 1200;

    // TILES
    // Point tiles to the data in HI RAM
    tiles = ramstart;

    // Allocate the structure for the list/length
    tileList = malloc(5);
    tileList->tilesetId = tilesetId;
    tileList->length = tilesLength;
    tileList->tiles = tiles;

    ramstart+= tilesLength * sizeof(TileLayout);

    // Entities
    // See if we have already cached the entity list
    // We cache this because entities like energy and gold don't respawn
    // We don't use HI RAM here because we cache it
    // Its not much memory but we could refacto this into other HI RAM (later)
    entityList = cachedEntityListGet(levelNum);
    if (entityList == 0) {
        entities = malloc(entitiesLength * sizeof(Entity));
        for (x=0; x<entitiesLength * sizeof(Entity); x++) {
            ((unsigned char*)entities)[x] = *(unsigned char*)(ramstart+x);
        }
        entityList = malloc(4);
        entityList->length = entitiesLength;
        entityList->entities = entities;
    }
    ramstart+= entitiesLength * sizeof(Entity);

    // Enemies
    // Point enemies to the data in HI RAM
    enemies = ramstart;
    
    enemyList = malloc(4);
    enemyList->length = enemiesLength;
    enemyList->enemies = enemies;
    
    ramstart+= enemiesLength * sizeof(EnemyLayout);

    // Overall
    level = malloc(sizeof(LevelOveralLayout));
    level->levelNum = levelNum;
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

