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

typedef struct EnemyStats {
    unsigned char graphicsIdx;
    unsigned char animationSpeed;
    unsigned char speed;
    unsigned char health;
    unsigned char framesBetweenShots;
} EnemyStats;

void spriteDataLoad();
void spriteIRQConfig();
void spriteReset(Sprite *sp);
void spriteAnimationAddressSet(Sprite *sp, unsigned char idx);
void playerCreate(Sprite *p, Entrance *entrance, unsigned char index);
void enemyCreate(EnemyType type, AISprite *enemy, EnemyLayout *layout, unsigned char index);
void bulletCreate(Sprite *b, unsigned char index);
void laserCreate(Sprite *b, unsigned char index);
void energyCreate(Sprite *b, Entity *entityInfo, unsigned char index);
void goldCreate(Sprite *b, Entity *entityInfo, unsigned char index);
void scubaCreate(Sprite *b, Entity *entityInfo, unsigned char index);
void bootsCreate(Sprite *b, Entity *entityInfo, unsigned char index);
void weaponCreate(Sprite *b, Entity *entityInfo, unsigned char index);
void extraLifeCreate(Sprite *b, Entity *entityInfo, unsigned char index);
void shipCreate(Sprite *b, unsigned char index);
void explosionSmallCreate(Sprite *b, unsigned char index);

#endif