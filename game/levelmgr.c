#include <cx16.h>
#include <cbm.h>
#include <stdio.h>
#include <stdlib.h>
#include <peekpoke.h>
#include "x16graphics.h"
#include "memmap.h"
#include "level.h"

LevelOveralLayout *levelGet(unsigned char levelNum) {
    unsigned short x,y;
    unsigned char tilesetId;
    unsigned short tilesLength;
    unsigned char entrancesLength;
    unsigned char exitsLength;
    unsigned char enemiesLength;
    char filename[32];
    unsigned char *movementTypes;

    TileLayout *tiles;
    TileLayoutList *tilesList;
    Entrance *entrances;
    EntranceList *entranceList;
    Exit *exits;
    ExitList *exitList;
    EnemyLayout *enemies;
    EnemyLayoutList *enemyList;
    LevelOveralLayout *level;
    
    unsigned short ramstart = (unsigned short)BANK_RAM;
    sprintf(filename, "levels/level%d.bin", levelNum);

    // Load the level file into Banked RAM
    POKE(0, 2);
    cbm_k_setnam(filename);
    cbm_k_setlfs(0, 8, 0);
    cbm_k_load(0, (unsigned int)ramstart);

    // The level file has a structure that starts with
    // the lengths of everything.
    tilesetId = *(unsigned char*)(ramstart);
    tilesLength = *(unsigned short*)(ramstart+1);
    entrancesLength = *(unsigned char*)(ramstart+3);
    exitsLength = *(unsigned char*)(ramstart+4);
    enemiesLength = *(unsigned char*)(ramstart+5);
    
    // Movement Types is always a fixed 40x30 size so we don't need a length
    movementTypes = malloc(1200);
    ramstart+= 6;
    for (y=0; y<30; y++) {
        for (x=0; x<40; x++) {
            ((unsigned char[30][40])movementTypes)[y][x] = *(unsigned char*)(ramstart);
            ramstart++;
        }
    }

    // TILES
    tiles = malloc(tilesLength * sizeof(TileLayout));
    for (x=0; x<tilesLength * sizeof(TileLayout); x++) {
        ((unsigned char*)tiles)[x] = *(unsigned char*)(ramstart+x);
    }
    tilesList = malloc(5);
    tilesList->tilesetId = tilesetId;
    tilesList->length = tilesLength;
    tilesList->tiles = tiles;

    ramstart+= tilesLength * sizeof(TileLayout);

    // Entrances
    entrances = malloc(entrancesLength * sizeof(Entrance));
    for (x=0; x<entrancesLength * sizeof(Entrance); x++) {
        ((unsigned char*)entrances)[x] = *(unsigned char*)(ramstart+x);
    }
    entranceList = malloc(4);
    entranceList->length = entrancesLength;
    entranceList->entrances = entrances;

    ramstart+= entrancesLength * sizeof(Entrance);

    // Exits
    exits = malloc(exitsLength * sizeof(Exit));
    for (x=0; x<exitsLength * sizeof(Exit); x++) {
        ((unsigned char*)exits)[x] = *(unsigned char*)(ramstart+x);
    }
    exitList = malloc(4);
    exitList->length = exitsLength;
    exitList->exits = exits;
    
    ramstart+= exitsLength * sizeof(Exit);

    // Enemies
    enemies = malloc(enemiesLength * sizeof(EnemyLayout));
    for (x=0; x<enemiesLength * sizeof(EnemyLayout); x++) {
        ((unsigned char*)enemies)[x] = *(unsigned char*)(ramstart+x);
    }
    enemyList = malloc(4);
    enemyList->length = enemiesLength;
    enemyList->enemies = enemies;
    
    ramstart+= enemiesLength * sizeof(EnemyLayout);

    // Overall
    level = malloc(sizeof(LevelOveralLayout));
    level->levelNum = levelNum;
    level->tilesList = tilesList;
    level->enemiesList = enemyList;
    level->entranceList = entranceList;
    level->exitList = exitList;
    level->movementTypes = movementTypes;

    return level;
}

void freeLevel(LevelOveralLayout *level) {
    free(level->movementTypes);
    free(level->tilesList->tiles);
    free(level->tilesList);
    free(level->enemiesList->enemies);
    free(level->enemiesList);
    free(level->entranceList->entrances);
    free(level->entranceList);
    free(level->exitList->exits);
    free(level->exitList);
    free(level);
}

void layerMapsLevelInit(LevelOveralLayout *level) {
    unsigned short x, y;
    
    vMemSetIncMode(1);

    // Clear Layer 0 (background)
    vMemSetBank(LAYER0_MAP_MEM_BANK);
    vMemSetAddr(LAYER0_MAP_MEM);
    for (y=0; y<32; y++) {
        for (x=0; x<64; x++) {
            vMemSetData0(0);
            vMemSetData0(0);
        }
    }

    // Clear Layer 1 (foreground)
    vMemSetBank(LAYER1_MAP_MEM_BANK);
    vMemSetAddr(LAYER1_MAP_MEM);
    for (y=0; y<32; y++) {
        for (x=0; x<64; x++) {     
            vMemSetData0(0);
            vMemSetData0(0);
        }
    }

    addLevelTiles(level->tilesList->length, level->tilesList->tiles);
}

