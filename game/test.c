#include <cx16.h>
#include <cbm.h>
#include <stdio.h>
#include <stdlib.h>
#include <peekpoke.h>
#include "level.h"

/*
unsigned char level0MovementTypes[30][40] // Fixed size

TileLayout level0Tiles[?] { 7, 19, 1, 154, 0, 0, 0 }, // 7 bytes
TileLayoutList level0TilesList = { ?, level0Tiles };

Entrance level0Entrances[?]  { 19, 21, 0 }, // byte/byte/byte = 3 bytes
EntranceList level0EntranceList = { ?, level0Entrances };

Exit level0Exits[?] // byte/byte/byte/byte = 4 bytes
ExitList level0ExitList = { ?, level0Exits };

EnemyLayout level0Enemies[?] // 8 bytes
EnemyLayoutList level0EnemyList = { ?, level0Enemies };

LevelOveralLayout level0 = {
    0,
    &level0TilesList,
    &level0EnemyList,
    &level0EntranceList,
    &level0ExitList,
    (unsigned char *)level0MovementTypes,
};

How:
const output = new Uint8Array([
    0, 0,
    shortHi(tilesLength),
    shortLo(tilesLength),
    entrancesBytes.length/3,
    exitsBytes.length/4,
    enemiesBytes.length/8,
    ...movementTypeBytes,
    ...tilesBytes,
    ...entrancesBytes,
    ...exitsBytes,
    ...enemiesBytes
  ]);


*/
/**
 * Program to test loading .bin files
*/

LevelOveralLayout *createLevel(unsigned char levelNum) {
    unsigned short x,y;
    unsigned short tilesLength; // *7
    unsigned char entrancesLength; // *3
    unsigned char exitsLength;  // *4
    unsigned char enemiesLength; // *8
    char filename[32];
    unsigned char *movementTypes;

    TileLayout *tiles;
    TileLayoutList *tilesList; // = { 389, level1Tiles };
    Entrance *entrances;
    EntranceList *entranceList; // = { 1, level1Entrances };
    Exit *exits;
    ExitList *exitList; // = { 1, level1Exits };
    EnemyLayout *enemies;
    EnemyLayoutList *enemyList; // = { 6, level1Enemies };
    LevelOveralLayout *level;
    
    unsigned short ramstart = (unsigned short)BANK_RAM;

    POKE(0, 2);

    sprintf(filename, "levels/level%d.bin", levelNum);
    printf("Loading file %s\n", filename);

    cbm_k_setnam(filename);
    cbm_k_setlfs(0, 8, 0);
    cbm_k_load(0, (unsigned int)ramstart);

    tilesLength = *(unsigned short*)ramstart;
    entrancesLength = *(unsigned char*)(ramstart+2);
    exitsLength = *(unsigned char*)(ramstart+3);
    enemiesLength = *(unsigned char*)(ramstart+4);
    
    movementTypes = malloc(1200);
    ramstart+= 5;
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
    tilesList = malloc(4);
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

void main() {
    unsigned short x,y;
    LevelOveralLayout *level;
    
    level = createLevel(1);

    printf("Enemies: %u\n", level->enemiesList->length);
    for (x=0; x<level->enemiesList->length; x++) {
        for (y=0; y< sizeof(EnemyLayout); y++) {
            printf("%u ", ((unsigned char*)level->enemiesList->enemies)[(x*sizeof(EnemyLayout))+y]);
        }
        printf("\n");
    }

    freeLevel(level);
}

// cl65 -o testload.prg -t cx16 testload.c
// ../../emu/x16emu -prg testload.prg -run

// 10 for i=0 to 15 : print peek($a000+i) : next i