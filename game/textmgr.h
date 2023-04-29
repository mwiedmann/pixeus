#ifndef TEXTMGR_H
#define TEXTMGR_H

#include "sprites.h"

unsigned short loadingMessage();
void showTitleScreen();
unsigned char showIntroScene(Sprite *ship);
void gameOverScreen(unsigned short gold, unsigned char energy);
void victoryScreen(Sprite *ship, unsigned short gold, unsigned char energy);
unsigned char showSelectionScreen();

#endif