#ifndef LEVELMGR_H
#define LEVELMGR_H

#include "level.h"

LevelOveralLayout *levelGet(unsigned char levelNum);
void freeLevel(LevelOveralLayout *level);

#endif