#ifndef TEXTMGR_H
#define TEXTMGR_H

#include "sprites.h"

unsigned char showTitleScreen();
void showIntroScene(Sprite *ship);
void gameOverScreen();
void victoryScreen(Sprite *ship, unsigned short gold);

#endif