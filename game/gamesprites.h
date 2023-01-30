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

void playerCreate(Sprite *p, unsigned char index);
void snakeCreate(AISprite *snake, EnemyLayout *layout, unsigned char index);
void beeCreate(AISprite *bee, EnemyLayout *layout, unsigned char index);
void ghostCreate(AISprite *ghost, EnemyLayout *layout, unsigned char index);
void bulletCreate(Sprite *b, unsigned char index);

#endif