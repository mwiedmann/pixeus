#ifndef ENEMYMGR_H
#define ENEMYMGR_H

#include "gamesprites.h"

AISprite *findEnemyCollision(Sprite *s);
unsigned char enemiesCreate(LevelOveralLayout *level, unsigned char nextSpriteIndex);
void enemiesReset(unsigned char length);
void enemyShot(short x, short y, unsigned char direction);
void resetClosestLaser(short x, short y);
void enemiesMove(Sprite *player, unsigned char length);
void enemyLasersMove(LevelOveralLayout *level);
unsigned char enemyLasersCreate(unsigned char nextSpriteIndex);

#endif