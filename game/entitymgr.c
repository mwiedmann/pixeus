#include "x16graphics.h"
#include "gamesprites.h"
#include "gametiles.h"
#include "level.h"
#include "levelutils.h"

Sprite entitySprites[16];

// Increase this size if we have more than 32 levels
#define CACHED_ENTITY_LIST_LENGTH 32

EntityList *cachedEntityLists[CACHED_ENTITY_LIST_LENGTH];

void initCachedLevelData() {
    unsigned char i;

    for (i=0; i<CACHED_ENTITY_LIST_LENGTH; i++) {
        cachedEntityLists[i] = 0;
    }
}
void cacheLevelData(LevelOveralLayout *level) {
    if (cachedEntityLists[level->levelNum] == 0) {
        cachedEntityLists[level->levelNum] = level->entityList;
    }
}

EntityList *cachedEntityListGet(unsigned char levelNum) {
    return cachedEntityLists[levelNum];
}

unsigned char entitiesCreate(LevelOveralLayout *level, unsigned char nextSpriteIndex) {
    unsigned char i, index;

    for (i=0,index=0; i<level->entityList->length; i++) {
        if (level->entityList->entities[i].entityType == EnergyEnum) {
            energyCreate(&entitySprites[index++], &level->entityList->entities[i], nextSpriteIndex++);
        } else if (level->entityList->entities[i].entityType == GoldEnum) {
            goldCreate(&entitySprites[index++], &level->entityList->entities[i], nextSpriteIndex++);
        }
    }

    return index;
}

void entitiesReset(unsigned char length) {
    unsigned char i;
    Sprite *entity;

    // Reset entities
    for (i=0; i<length; i++) {
        // TODO: This seems to be a standard sprite reset, create common function
        entity = &entitySprites[i];
        spriteReset(entity);
    }
}

void entitiesAnimate(unsigned char length) {
    unsigned char i;
    Sprite *entity;

    // Animate entities
    for (i=0; i<length; i++) {
        // TODO: This seems to be a standard sprite animation, create common function
        entity = &entitySprites[i];
        entity->animationCount++;
        if (entity->animationCount == entity->animationSpeed) {
            entity->animationCount=0;
            entity->animationFrame++;
            if (entity->animationFrame == entity->frames) {
                entity->animationFrame = 0;
            }
            x16SpriteIdxSetGraphicsPointer(entity->index, entity->clrMode, entity->graphicsBank,
                entity->graphicsAddress+(entity->animationFrame * entity->frameSize));
        }
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
        if (entity->x == sprite->x / TILE_PIXEL_WIDTH && entity->y == sprite->y / TILE_PIXEL_HEIGHT) {
            spriteReset(sprite);
            return;
        }
    }
}