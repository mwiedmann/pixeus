#ifndef LEVELUTILS_H
#define LEVELUTILS_H

#include "level.h"
#include "sprites.h"

// This is a collection of functions that don't have a proper home (yet)
// I did want to always make a new .c/.h file just for 1 or 2 functions
// As I collect more functions in here its worth going through and seeing
// if we have enough related functions to make a new file.

Exit *playerTouchingExit(LevelOveralLayout *level, Sprite *sprite);
void spriteTouchingTile(LevelOveralLayout *level, Sprite *sprite, TileInfo *tileCollision);
unsigned char playerNear(Sprite *player, short x, short y);
unsigned char facePlayer(Sprite *player, short x);
void smallExplosion(Sprite *expSmall, ZDepth zDepth, short x, short y);
Entrance *findEntranceForExit(EntranceList *entranceList, unsigned char entranceId);

#endif