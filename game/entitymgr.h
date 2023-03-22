#ifndef ENTITYMGR_H
#define ENTITYMGR_H

#include "level.h"

/**
 * Clear (and possibly free the mem) for our cached level data.
 * Called for each new game. Only need to "free" if there was a previous game.
 *  
*/
void initCachedLevelData(unsigned char clearFirst);

/**
 * Cache some level data for things that don't respawn (gold, energy)
 * and counters like how many frames before enemy respawn.
*/
void cacheLevelData(LevelOveralLayout *level);

/**
 * Should this enemy respawn/be alive on this level. Based on frame counts if it died before.
*/
unsigned char enemyAlive(unsigned long frameCount, unsigned char level, unsigned char enemyIdx);

/**
 * Set the frame count when an enemy died. Used to track when it should respawn.
*/
void enemyFrameSet(unsigned long frameCount, unsigned char level, unsigned char enemyIdx);

/**
 * Get the cached entity list for this level. Can return NULL/0 if not cached.
*/
EntityList *cachedEntityListGet(unsigned char levelNum);

/**
 * Create the sprites for the entities (gold, energy, items) on this level
*/
unsigned char entitiesCreate(LevelOveralLayout *level, unsigned char nextSpriteIndex);

/**
 * Reset (make inactive, disable, etc) the entities
*/
void entitiesReset(unsigned char length);

/**
 * Animate all of the entities
*/
void entitiesAnimate(unsigned char length);

/**
 * Find and hide an entity. Called after it is collected by the player.
*/
void hideEntity(unsigned char length, Entity *entity);

#endif