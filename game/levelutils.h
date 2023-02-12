#ifndef LEVELUTILS_H
#define LEVELUTILS_H

#include "level.h"
#include "sprites.h"

// This is a collection of functions that don't have a proper home (yet)
// I did want to always make a new .c/.h file just for 1 or 2 functions
// As I collect more functions in here its worth going through and seeing
// if we have enough related functions to make a new file.

/**
 * Return any entity the player is touching
*/
Entity *playerTouchingEntity(EntityList *entityList, Sprite *sprite);

/**
 * Get the TileInfo for the current tile a sprite is touching
*/
void spriteTouchingTile(LevelOveralLayout *level, Sprite *sprite, TileInfo *tileCollision);

/**
 * Return if the player is "near" to a sprite
*/
unsigned char playerNear(Sprite *player, short x, short y);

/**
 * Get the animation direction for a sprite to face towards the player
*/
unsigned char facePlayer(Sprite *player, short x);

/**
 * Show a small explosion
*/
void smallExplosion(Sprite *expSmall, ZDepth zDepth, short x, short y);

/**
 * Find the Entrance by entranceId
*/
Entrance *findEntranceForExit(EntranceList *entranceList, unsigned char entranceId);

/**
 * Wait for a button press and release
*/
void waitForButtonPress();

/**
 * Return if button pressed and released
*/
unsigned char readButtonPress();

#endif