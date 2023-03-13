#ifndef ENTITYMGR_H
#define ENTITYMGR_H

#include "level.h"

void initCachedLevelData(unsigned char clearFirst);
void cacheLevelData(LevelOveralLayout *level);
EntityList *cachedEntityListGet(unsigned char levelNum);
void cacheLevelData(LevelOveralLayout *level);
unsigned char enemyAlive(unsigned long frameCount, unsigned char level, unsigned char enemyIdx);
void enemyFrameSet(unsigned long frameCount, unsigned char level, unsigned char enemyIdx);

unsigned char entitiesCreate(LevelOveralLayout *level, unsigned char nextSpriteIndex);
void entitiesReset(unsigned char length);
void entitiesAnimate(unsigned char length);
void hideEntity(unsigned char length, Entity *entity);

#endif