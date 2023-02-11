#ifndef ENTITYMGR_H
#define ENTITYMGR_H

#include "level.h"

void initCachedLevelData();
void cacheLevelData(LevelOveralLayout *level);
EntityList *cachedEntityListGet(unsigned char levelNum);

unsigned char entitiesCreate(LevelOveralLayout *level, unsigned char nextSpriteIndex);
void entitiesReset(unsigned char length);
void entitiesAnimate(unsigned char length);
void hideEntity(unsigned char length, Entity *entity);

#endif