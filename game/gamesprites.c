#include <6502.h>
#include <cx16.h>
#include <stdio.h>

#include "gamesprites.h"
#include "x16graphics.h"
#include "memmap.h"
#include "gametiles.h"
#include "sprites.h"
#include "level.h"
#include "imageload.h"

// Not sure how big the stack needs to be. Unclear how this works.
#define IRQ_HANDLER_STACK_SIZE 8
unsigned char IRQHandlerStack[IRQ_HANDLER_STACK_SIZE];

#define SPRITE_COUNT 36
#define ENEMY_SPRITE_IDX_START SPRITE_MEM_SNAKE_IDX

unsigned short spriteMemAddresses[SPRITE_COUNT];

unsigned short enemySpritesMemStart = 0;

// { Snake = 0, Bee = 1, Ghost = 2, Scorpion = 3, Wasp = 4, Fish1 = 5, BigGhost = 6, Eyeball = 7,
// Mushroom = 8, Slime = 9, Spider = 10, Rat = 11, Rockman = 12, Eel = 13, Iceman = 14, Snowball = 15,
// Dragonfly = 16, BigBear = 17, Clouds = 18, Flies = 19, Shark = 20, Bluehawk = 21, Beetle = 22,
// Jellyfish = 23, Wolf = 24 }
/*
typedef struct EnemyStats {
    unsigned char graphicsIdx;
    unsigned char animationSpeed;
    unsigned char speed;
    unsigned char health;
    unsigned char framesBetweenShots;
    unsigned short framesBetweenJumps;
    unsigned char animateIfStill;
} EnemyStats;
*/
EnemyStats enemyStats[25] = {
    // graphicsIdx              asp spd hea fbs  fbj  ais
    { SPRITE_MEM_SNAKE_IDX,     6,  8,  3,  90,  300, 0 },
    { SPRITE_MEM_BEE_IDX,       6,  13, 2,  70,  0,   1 },
    { SPRITE_MEM_GHOST_IDX,     20, 3,  4,  180, 0,   1 },
    { SPRITE_MEM_SCORPION_IDX,  6,  6,  3,  90,  240, 0 },
    { SPRITE_MEM_WASP_IDX,      6,  12, 2,  120, 0,   1 },
    { SPRITE_MEM_FISH1_IDX,     30, 4,  2,  120, 105, 1 },
    { SPRITE_MEM_BIG_GHOST_IDX, 8,  3,  7, 120, 0,   1 },
    { SPRITE_MEM_EYEBALL_IDX,   6,  6,  3,  90,  0,   1 },
    { SPRITE_MEM_MUSHROOM_IDX,  30, 4,  3,  90,  0,   1 },
    { SPRITE_MEM_SLIME_IDX,     20, 4,  3,  90,  300, 1 },
    { SPRITE_MEM_SPIDER_IDX,    6,  9,  2,  90,  120, 0 },
    { SPRITE_MEM_RAT_IDX,       6,  10, 2,  90,  180, 0 },
    { SPRITE_MEM_ROCKMAN_IDX,   10, 2,  4,  90,  0,   1 },
    { SPRITE_MEM_EEL_IDX,       6,  10, 1,  90,  90,  1 },
    { SPRITE_MEM_ICEMAN_IDX,    12, 2,  3,  90,  0,   1 },
    { SPRITE_MEM_SNOWBALL_IDX,  6,  8,  3,  90,  120, 1 },
    { SPRITE_MEM_DRAGONFLY_IDX, 6,  11, 2,  80,  180, 1 },
    { SPRITE_MEM_BIG_BEAR_IDX,  6,  10, 7, 120, 180, 1 },
    { SPRITE_MEM_CLOUDS_IDX,    6,  6,  3,  90,  0,   1 },
    { SPRITE_MEM_FLIES_IDX,     4,  10, 7, 120, 180, 1 },
    { SPRITE_MEM_SHARK_IDX,     20, 7,  4, 120, 120, 1 },
    { SPRITE_MEM_BLUEHAWK_IDX,  6,  11, 2,  80,  180, 1 },
    { SPRITE_MEM_BEETLE_IDX,    12, 4,  2,  90,  0,   1 },
    { SPRITE_MEM_JELLYFISH_IDX, 8,  3,  2,  120, 360, 1 },
    { SPRITE_MEM_WOLF_IDX,      6,  6,  3,  90,  240, 1 },
};

/**
 * These are non-enemy sprites that we just load once.
 * Other sprites (enemies) are loaded on demand.
*/
void spriteDataLoad() {
    unsigned char i;
    unsigned char filename[32];
    unsigned short allBytes = SPRITE_MEM;

    for (i=0; i<ENEMY_SPRITE_IDX_START; i++) {
        sprintf(filename, "images/sp%u.bin", i);
        spriteMemAddresses[i] = allBytes;
        allBytes+= imageFileLoad(IMAGE_LOAD_BANK, SPRITE_MEM_BANK, allBytes, filename);
    }

    // Enemy sprites are loaded on demand and can start at this mem addr.
    enemySpritesMemStart = allBytes;
}

void spriteEnemyDataLoad(EnemyLayout *enemies, unsigned char length) {
    unsigned char i, gIdx;
    unsigned char filename[32];
    unsigned short allBytes = enemySpritesMemStart;

    // Clear any existing sprite memory addresses
    for (i=ENEMY_SPRITE_IDX_START; i<SPRITE_COUNT; i++) {
        spriteMemAddresses[i] = 0;
    }

    for (i=0; i<length; i++) {
        gIdx = enemyStats[enemies[i].enemyType].graphicsIdx;
        if (spriteMemAddresses[gIdx] == 0) {
            sprintf(filename, "images/sp%u.bin", gIdx);
            spriteMemAddresses[gIdx] = allBytes;
            allBytes+= imageFileLoad(IMAGE_LOAD_BANK, SPRITE_MEM_BANK, allBytes, filename);
        }
    }
}

void spriteIRQConfig() {
    // Turn on sprites
    x16SpriteSetGlobalOn();

    // Setup the IRQ handler for sprite collisions and enable global sprite collisions
    set_irq(&x16SpriteCollisionIRQHandler, IRQHandlerStack, IRQ_HANDLER_STACK_SIZE);
    x16SpriteCollisionsEnable();
}

void spriteAnimationAdvance(Sprite *sp) {
    sp->animationCount++;
    if (sp->animationCount >= sp->animationSpeed) {
        sp->animationCount=0;
        sp->animationFrame++;
        if (sp->animationFrame >= sp->frames) {
            sp->animationFrame = 0;
        }
        x16SpriteIdxSetGraphicsPointer(sp->index, sp->clrMode, sp->graphicsBank,
            sp->graphicsAddress+(sp->animationFrame * sp->frameSize));
    }
}

void spriteAnimationAddressSet(Sprite *sp, unsigned char idx) {
    if (sp->graphicsAddress != spriteMemAddresses[idx]) {
        sp->graphicsAddress = spriteMemAddresses[idx];
        x16SpriteIdxSetGraphicsPointer(sp->index, sp->clrMode, sp->graphicsBank,
            sp->graphicsAddress);
    }
}

void spriteReset(Sprite *sp) {
    spriteMove(sp, 0, 0);
    sp->active = 0;
    sp->zDepth = Disabled;
    x16SpriteIdxSetZDepth(sp->index, Disabled);
    x16SpriteIdxSetXY(sp->index, sp->x, sp->y);
}

void spriteDefaults(Sprite *sp) {
    sp->graphicsBank = SPRITE_MEM_BANK;
    sp->width = PX16;
    sp->height = PX16;
    sp->frames = 4;
    sp->frameSize = 256;
    sp->collisionMask = COLLISION_MASK_ENEMY;
    sp->zDepth = BetweenL0L1;
}

void standardAISpriteConfig(AISprite *sp, EnemyLayout *layout, unsigned char index)
{
    sp->sprite.index = index;
    sp->sprite.active=1;
    sp->sprite.clrMode = 1;
    
    // Possible zDepth will change but leave for now
    sp->sprite.animationDirection = layout->moveDir;
    sp->sprite.x = layout->x * TILE_PIXEL_WIDTH;
    sp->sprite.y = layout->y * TILE_PIXEL_HEIGHT;
    sp->sprite.xL = sp->sprite.x * MOVE_FACTOR;
    sp->sprite.yL = sp->sprite.y * MOVE_FACTOR;
    sp->sprite.lastX = sp->sprite.x;
    sp->sprite.lastY = sp->sprite.y;

    // Default for now
    if (sp->sprite.swimSpeed == 0) {
        sp->sprite.swimSpeed = sp->sprite.speed;
    }

    // TODO: patrolDir can move up/down too
    sp->xTileStart = layout->x - layout->patrolA;
    sp->yTileStart = layout->y;
    sp->xTileEnd = layout->x + layout->patrolB;
    sp->yTileEnd = layout->y;
    sp->framesUntilNextShot = sp->framesBetweenShots;
    sp->framesBetweenFacePlayer = 60;
    sp->framesUntilFacePlayer = 0;
    sp->framesUntilNextJump = sp->framesBetweenJumps;
    sp->jumpDir = 1;
    sp->jumpSpeed = 0;

    spriteInit(&sp->sprite);
    x16SpriteIdxSetXY(sp->sprite.index, sp->sprite.x, sp->sprite.y);
    x16SpriteIdxSetHFlip(sp->sprite.index, sp->sprite.animationDirection);
}

void playerCreate(Sprite *p, Entrance *entrance, unsigned char index) {
    spriteDefaults(p);
    
    p->index = index;
    p->active = 1;
    p->clrMode = 1;
    p->collisionMask = COLLISION_MASK_PLAYER;
    p->zDepth = Disabled;
    p->graphicsAddress = spriteMemAddresses[SPRITE_MEM_PLAYER_IDX];
    p->animationSpeed = 6;
    if (entrance != 0) {
        p->x = entrance->x * TILE_PIXEL_WIDTH;
        p->y = entrance->y * TILE_PIXEL_HEIGHT;
    } else {
        p->x = 320;
        p->y = 240;
    }
    p->xL = p->x * MOVE_FACTOR;
    p->yL = p->y * MOVE_FACTOR;
    p->lastX = p->x;
    p->lastY = p->y;
    p->animationDirection = 0;

    spriteInit(p);
    x16SpriteIdxSetXY(p->index, p->x, p->y);
}

void largeSpriteAdjust(AISprite *largeSprite)
{
    largeSprite->sprite.width = PX32;
    largeSprite->sprite.height = PX32;
    largeSprite->sprite.frameSize = 1024; // Calculated as width * height
    
    // The largeSprite is tall, shoot lasers out of its bottom half
    largeSprite->yLaserAdjust = TILE_PIXEL_HEIGHT;
}

void largeFlatSpriteAdjust(AISprite *largeSprite)
{
    largeSprite->sprite.width = PX32;
    largeSprite->sprite.height = PX16;
    largeSprite->sprite.frameSize = 512; // Calculated as width * height
}

void enemyCreate(EnemyType type, AISprite *enemy, EnemyLayout *layout, unsigned char index) {
    spriteDefaults(&enemy->sprite);

    enemy->sprite.graphicsAddress = spriteMemAddresses[enemyStats[type].graphicsIdx];
    enemy->sprite.animationSpeed = enemyStats[type].animationSpeed;
    enemy->sprite.speed = enemyStats[type].speed;
    enemy->health = enemyStats[type].health;
    enemy->framesBetweenShots = enemyStats[type].framesBetweenShots;
    enemy->framesBetweenJumps = enemyStats[type].framesBetweenJumps;
    enemy->sprite.animateIfStill = enemyStats[type].animateIfStill;
    enemy->yLaserAdjust = 0;
    
    // Some special settings for large sprites
    if (type == BigGhost || type == BigBear || type == Flies) {
        largeSpriteAdjust(enemy);
    } else if (type == Shark) {
        largeFlatSpriteAdjust(enemy);
    }

    standardAISpriteConfig(enemy, layout, index);
}

void explosionSmallCreate(Sprite *b, unsigned char index) {
    spriteDefaults(b);

    b->index = index;
    b->active = 1;
    b->clrMode = 1;
    b->collisionMask = 0;
    b->zDepth = Disabled;
    b->graphicsAddress = spriteMemAddresses[SPRITE_MEM_EXP_SMALL_IDX];
    b->animationSpeed = 6;

    spriteInit(b);
}

void bulletCreate(Sprite *b, unsigned char index) {
    spriteDefaults(b);

    b->index = index;
    b->active = 1;
    b->clrMode = 1;
    b->collisionMask = COLLISION_MASK_PLAYER_BULLET;
    b->zDepth = Disabled;
    b->graphicsAddress = spriteMemAddresses[SPRITE_MEM_PLAYER_IDX] + SPRITE_MEM_PLAYER_BULLET_OFFSET;
    b->frames = 1;
    b->x = 320;
    b->y = 240;

    spriteInit(b);
}

void laserCreate(Sprite *b, unsigned char index) {
    spriteDefaults(b);

    b->index = index;
    b->active = 0;
    b->clrMode = 1;
    b->collisionMask = COLLISION_MASK_ENEMY_LASER;
    b->zDepth = Disabled;
    b->graphicsAddress = spriteMemAddresses[SPRITE_MEM_LASER_IDX];
    b->frames = 1;
    b->x = 320;
    b->y = 240;
    b->speed = 2;
    b->swimSpeed = 3;

    spriteInit(b);
}

void energyCreate(Sprite *b, Entity *entityInfo, unsigned char index) {
    spriteDefaults(b);

    b->index = index;
    b->active = 0;
    b->clrMode = 1;
    b->collisionMask = 0;
    b->graphicsAddress = spriteMemAddresses[SPRITE_MEM_ENERGY_IDX];
    b->animationSpeed = 10;

    b->x = entityInfo->x * TILE_PIXEL_WIDTH;
    b->y = entityInfo->y * TILE_PIXEL_HEIGHT;

    spriteInit(b);
    x16SpriteIdxSetXY(b->index, b->x, b->y);
}

void goldCreate(Sprite *b, Entity *entityInfo, unsigned char index) {
    spriteDefaults(b);

    b->index = index;
    b->active = 0;
    b->clrMode = 1;
    b->collisionMask = 0;
    b->graphicsAddress = spriteMemAddresses[SPRITE_MEM_GOLD_IDX];
    b->animationSpeed = 10;

    b->x = entityInfo->x * TILE_PIXEL_WIDTH;
    b->y = entityInfo->y * TILE_PIXEL_HEIGHT;

    spriteInit(b);
    x16SpriteIdxSetXY(b->index, b->x, b->y);
}

void scubaCreate(Sprite *b, Entity *entityInfo, unsigned char index) {
    spriteDefaults(b);

    b->index = index;
    b->active = 0;
    b->clrMode = 1;
    b->collisionMask = 0;
    b->graphicsAddress = spriteMemAddresses[SPRITE_MEM_SCUBA_IDX];
    b->frames = 1;

    b->x = entityInfo->x * TILE_PIXEL_WIDTH;
    b->y = entityInfo->y * TILE_PIXEL_HEIGHT;

    spriteInit(b);
    x16SpriteIdxSetXY(b->index, b->x, b->y);
}

void bootsCreate(Sprite *b, Entity *entityInfo, unsigned char index) {
    spriteDefaults(b);
    
    b->index = index;
    b->active = 0;
    b->clrMode = 1;
    b->collisionMask = 0;
    b->graphicsAddress = spriteMemAddresses[SPRITE_MEM_BOOTS_IDX];
    b->frames = 1;

    b->x = entityInfo->x * TILE_PIXEL_WIDTH;
    b->y = entityInfo->y * TILE_PIXEL_HEIGHT;

    spriteInit(b);
    x16SpriteIdxSetXY(b->index, b->x, b->y);
}

void weaponCreate(Sprite *b, Entity *entityInfo, unsigned char index) {
    spriteDefaults(b);

    b->index = index;
    b->active = 0;
    b->clrMode = 1;
    b->collisionMask = 0;
    b->graphicsAddress = spriteMemAddresses[SPRITE_MEM_WEAPON_IDX];
    b->frames = 1;

    b->x = entityInfo->x * TILE_PIXEL_WIDTH;
    b->y = entityInfo->y * TILE_PIXEL_HEIGHT;

    spriteInit(b);
    x16SpriteIdxSetXY(b->index, b->x, b->y);
}

void extraLifeCreate(Sprite *b, Entity *entityInfo, unsigned char index) {
    spriteDefaults(b);

    b->index = index;
    b->active = 0;
    b->clrMode = 1;
    b->collisionMask = 0;
    b->graphicsAddress = spriteMemAddresses[SPRITE_MEM_EXTRA_LIFE_IDX];
    b->frames = 1;
    
    b->x = entityInfo->x * TILE_PIXEL_WIDTH;
    b->y = entityInfo->y * TILE_PIXEL_HEIGHT;

    spriteInit(b);
    x16SpriteIdxSetXY(b->index, b->x, b->y);
}

void shipCreate(Sprite *b, unsigned char index) {
    b->index = index;
    b->active = 1;
    b->clrMode = 1;
    b->collisionMask = 0;
    b->zDepth = Disabled;
    b->width = PX64;
    b->height = PX64;
    b->graphicsBank = SPRITE_MEM_BANK;
    b->graphicsAddress = spriteMemAddresses[SPRITE_MEM_SHIP_IDX];
    b->frames = 3;
    b->frameSize = 4096;
    b->x = 298;
    b->y = -64;
    b->animationSpeed=2;
    b->animationStopFrame=3;
    b->animationCount=0;
    b->animationFrame=0;
    b->speed = 8;

    spriteInit(b);
}
