#ifndef ENEMYMGR_H
#define ENEMYMGR_H

#include "gamesprites.h"

/**
 * Find the 1st enemy that overlaps with this sprite
*/
AISprite *findEnemyCollision(Sprite *s);

/**
 * Create the enemies defined on this level
*/
unsigned char enemiesCreate(LevelOveralLayout *level, unsigned char nextSpriteIndex);

/**
 * Disabled and reset the current enemy and laser sprites
*/
void enemiesReset(unsigned char length);

/**
 * Shoot an enemy laser starting at the location and direction
*/
void enemyShot(short x, short y, unsigned char direction);

/**
 * Reset the laser that is closest to this position.
 * Use this after a laser collision to find and reset the laser.
*/
void resetClosestLaser(short x, short y);

/**
 * Move all the enemies
*/
void enemiesMove(Sprite *player, unsigned char length);

/**
 * Move all the enemy lasers
*/
void enemyLasersMove(LevelOveralLayout *level);

/**
 * Create all the enemy laser sprites. Only call once during game init.
*/
unsigned char enemyLasersCreate(unsigned char nextSpriteIndex);

#endif