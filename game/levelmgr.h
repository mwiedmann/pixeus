#ifndef LEVELMGR_H
#define LEVELMGR_H

#include "level.h"

/**
 * Read a binary level file and load into our structs
*/
LevelOveralLayout *levelGet(unsigned char levelNum);

/**
 * Free all the memory associated with the level structs
*/
void freeLevel(LevelOveralLayout *level);

/**
 * Blanks out the layer maps.
 * Good to do this before loading new tiles otherwise the sceen goes wild!
*/
void layerMapsClear();

/**
 * Draw the tiles for the current level into the layer maps
 * We use both layer 0 and 1
*/
void layerMapsLevelInit(LevelOveralLayout *level);

#endif