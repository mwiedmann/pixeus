#include <stdlib.h>
#include <cx16.h>

#include "x16graphics.h"
#include "gamesprites.h"
#include "gametiles.h"
#include "level.h"
#include "levelutils.h"
#include "memmap.h"

Sprite entitySprites[16];

// Increase this size if we add more levels
#define CACHED_ENTITY_LIST_LENGTH 71

/**
 * How many frames an enemy will stay dead. Starts when player leaves the level.
 * 60 frames/sec, so 1 min = 3600
*/
#define ENEMY_FRAMES_STAY_DEAD 18000 // 18000=5 mins

/**
 * List of the entityTypes for entities in each level.
 * When they are collected the type is changed to EmptyType.
 * We need to hold this for the entire game so entities don't respawn
*/
unsigned char *cachedEntityLists[CACHED_ENTITY_LIST_LENGTH];

/**
 * This tracks how many frames have past since the enemy died.
 * They respawn after ENEMY_FRAMES_STAY_DEAD frames
*/
unsigned long* cachedEnemyDiedFrame[CACHED_ENTITY_LIST_LENGTH];

void initCachedLevelData(unsigned char clearFirst) {
    unsigned char i;

    for (i=0; i<CACHED_ENTITY_LIST_LENGTH; i++) {
        if (clearFirst && cachedEntityLists[i] != 0) {
            free(cachedEntityLists[i]);
            free(cachedEnemyDiedFrame[i]);
        }
        cachedEntityLists[i] = 0;
        cachedEnemyDiedFrame[i] = 0;
    }
}

void cacheLevelData(LevelOveralLayout *level) {
    unsigned char i;

    if (cachedEntityLists[level->levelNum] == 0) {
        // Track the entityType for each entity
        // It will change to EmptyType when the entity is collected
        cachedEntityLists[level->levelNum] = malloc(level->entityList->length);
        for (i=0; i<level->entityList->length; i++) {
            cachedEntityLists[level->levelNum][i] = level->entityList->entities[i].entityType;
        }

        // Memory for enemy death frame counts (4 bytes per)
        cachedEnemyDiedFrame[level->levelNum] = malloc(level->enemyList->length * 4);
        for (i=0; i<level->enemyList->length; i++) {
            cachedEnemyDiedFrame[level->levelNum][i] = 0;
        }
    }
}

unsigned char enemyAlive(unsigned long frameCount, unsigned char level, unsigned char enemyIdx) {
    unsigned long f;

    f = cachedEnemyDiedFrame[level][enemyIdx];

    if (f == 0 || frameCount - f >= ENEMY_FRAMES_STAY_DEAD) {
        return 1;
    }

    return 0;
}

void enemyFrameSet(unsigned long frameCount, unsigned char level, unsigned char enemyIdx) {
    cachedEnemyDiedFrame[level][enemyIdx] = frameCount;
}

void entityTypeSet(unsigned char entityType, unsigned char level, unsigned char entityIdx) {
    cachedEntityLists[level][entityIdx] = entityType;
}

unsigned char *cachedEntityListGet(unsigned char levelNum) {
    return cachedEntityLists[levelNum];
}

#pragma code-name (push, "BANKRAM01")

unsigned char entitiesCreate(LevelOveralLayout *level, unsigned char nextSpriteIndex) {
    unsigned char i, index;

    // Some of these are 0 because we don't create anything for them. They are skipped.
    void (*entityCreate[])(Sprite*, Entity*, unsigned char) = {
        0, 0, 0, energyCreate, goldCreate, scubaCreate, weaponCreate, bootsCreate, extraLifeCreate
    };

    for (i=0,index=0; i<level->entityList->length; i++) {
        if (*entityCreate[level->entityList->entities[i].entityType] != 0) {
            (*entityCreate[level->entityList->entities[i].entityType])(
                &entitySprites[index++], &level->entityList->entities[i], nextSpriteIndex++
            );
        }
    }
    
    return index;
}

#pragma code-name (pop)

void entitiesReset(unsigned char length) {
    unsigned char i;
    Sprite *entity;

    RAM_BANK = CODE_BANK;
    // Reset entities
    for (i=0; i<length; i++) {
        entity = &entitySprites[i];
        spriteReset(entity);
    }
    RAM_BANK = LEVEL_BANK;
}

void entitiesAnimate(unsigned char length) {
    unsigned char i;

    // Animate entities
    for (i=0; i<length; i++) {
        spriteAnimationAdvance(&entitySprites[i]);
    }
}

void hideEntity(unsigned char length, Entity *entity) {
    unsigned char i;
    Sprite *sprite;

    entity->entityType = EmptyEnum;

    // Find the sprite and hide it
    for (i=0; i<length; i++) {    
        sprite = &entitySprites[i];
        // TODO: We need an ID to associate between the Entity and Sprite
        if (entity->x == ((sprite->x + TILE_PIXEL_WIDTH_HALF) / TILE_PIXEL_WIDTH) &&
            entity->y == ((sprite->y + TILE_PIXEL_HEIGHT_HALF) / TILE_PIXEL_HEIGHT)) {
            RAM_BANK = CODE_BANK;
            spriteReset(sprite);
            RAM_BANK = LEVEL_BANK;
            return;
        }
    }
}