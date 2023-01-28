#ifndef GAMESPRITES_H
#define GAMESPRITES_H

#include "sprites.h"
#include "level.h"

typedef struct AISprite {
    Sprite sprite;
    unsigned char xTileStart;
    unsigned char yTileStart;
    unsigned char xTileEnd;
    unsigned char yTileEnd;
} AISprite;

void spriteDataLoad();
void spriteIRQConfig();

void playerCreate(Sprite *p);
void snakeCreate(AISprite *snake, LevelLayout *layout);
void bulletCreate(Sprite *b);

#endif