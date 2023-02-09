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
    unsigned char health;
    unsigned short framesUntilNextShot;
    unsigned short framesBetweenShots;
    unsigned short framesUntilFacePlayer;
    unsigned short framesBetweenFacePlayer;
    unsigned short yLaserAdjust;
} AISprite;

void spriteDataLoad();
void spriteIRQConfig();

void playerCreate(Sprite *p, Entrance *entrance, unsigned char index);
void snakeCreate(AISprite *snake, EnemyLayout *layout, unsigned char index);
void beeCreate(AISprite *bee, EnemyLayout *layout, unsigned char index);
void ghostCreate(AISprite *ghost, EnemyLayout *layout, unsigned char index);
void scorpionCreate(AISprite *scorpion, EnemyLayout *layout, unsigned char index);
void waspCreate(AISprite *wasp, EnemyLayout *layout, unsigned char index);
void fish1Create(AISprite *fish, EnemyLayout *layout, unsigned char index);
void bigGhostCreate(AISprite *ghost, EnemyLayout *layout, unsigned char index);
void bulletCreate(Sprite *b, unsigned char index);
void laserCreate(Sprite *b, unsigned char index);
void shipCreate(Sprite *b, unsigned char index);
void explosionSmallCreate(Sprite *b, unsigned char index);

#endif