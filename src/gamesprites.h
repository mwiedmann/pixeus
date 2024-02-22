#ifndef GAMESPRITES_H
#define GAMESPRITES_H

#include "sprites.h"
#include "level.h"

/*
COLLISION MASKS:
   ENEMY: 1 0 1 0
EN.LASER: 0 1 0 1
PL.LASER: 0 0 1 0
  PLAYER: 1 1 0 0

The collision bits will be the OVERLAP of the Collision Masks of the two sprites.
We can then look at all the sprites that have that bit in their mask

NOTE: If there are multiple collisions on the same frame, multiple bits will be set
This is fine, we just AND "&" with our COLLISION_RESULTS to see what has collided

COLLISION RESULTS:
   ENEMY-PLAYER: 1 0 0 0
EN.LASER-PLAYER: 0 1 0 0
 ENEMY-PL.LASER: 0 0 1 0

FALSE SELF COLLISIONS:
EN.LASER: 0 1 0 1
*/

// Collision Masks
#define COLLISION_MASK_ENEMY 0b1010
#define COLLISION_MASK_ENEMY_LASER 0b0101
#define COLLISION_MASK_PLAYER_BULLET 0b0010
#define COLLISION_MASK_PLAYER 0b1100

// Collision Results
#define COLLISION_RESULT_ENEMY_PLAYER 0b1000
#define COLLISION_RESULT_ENEMY_LASER_PLAYER 0b0100
#define COLLISION_RESULT_ENEMY_PLAYER_LASER 0b0010

// False Self Collisions
#define COLLISION_FALSE_SELF_ENEMY_LASER 0b0101

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
    unsigned short framesBetweenJumps;
    unsigned short framesUntilNextJump;
    unsigned char jumpFrames;
    signed char jumpDir;
    signed char jumpSpeed;
} AISprite;

typedef struct EnemyStats {
    unsigned char graphicsIdx;
    unsigned char animationSpeed;
    unsigned char speed;
    unsigned char health;
    unsigned char framesBetweenShots;
    unsigned short framesBetweenJumps;
    unsigned char animateIfStill;
} EnemyStats;

void spriteDataLoad();
void spriteEnemyDataLoad(EnemyLayout *enemies, unsigned char length) ;
void spriteIRQConfig();
void spriteAnimationAdvance(Sprite *sp);
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