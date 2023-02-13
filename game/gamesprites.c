#include <6502.h>
#include <cx16.h>
#include "gamesprites.h"
#include "x16graphics.h"
#include "memmap.h"
#include "gametiles.h"
#include "sprites.h"
#include "level.h"
#include "imageload.h"

#define ENEMY_COLLISION_MASK 0b1011 // [Enemy] | 0 | Player Bullets | Player
#define ENEMY_LASER_COLLISION_MASK 0b0101 // 0 | [Enemy Laser] | 0 | Player
#define PLAYER_COLLISION_MASK 0b1101 // Enemy | Enemy Laser | 0 | [Player]
#define PLAYER_BULLET_COLLISION_MASK 0b1010 // Enemy | 0 | [Player Bullet] | 0

// Not sure how big the stack needs to be. Unclear how this works.
#define IRQ_HANDLER_STACK_SIZE 8
unsigned char IRQHandlerStack[IRQ_HANDLER_STACK_SIZE];

void spriteDataLoad() {
    imageFileLoad(IMAGE_LOAD_BANK, SPRITE_MEM_BANK, SPRITE_MEM_PLAYER, "images/guyrun.bin");
    imageFileLoad(IMAGE_LOAD_BANK, SPRITE_MEM_BANK, SPRITE_MEM_PLAYER_SCUBA, "images/guyscuba.bin");
    imageFileLoad(IMAGE_LOAD_BANK, SPRITE_MEM_BANK, SPRITE_MEM_EXP_SMALL, "images/expsmall.bin");
    imageFileLoad(IMAGE_LOAD_BANK, SPRITE_MEM_BANK, SPRITE_MEM_SNAKE, "images/snake.bin");
    imageFileLoad(IMAGE_LOAD_BANK, SPRITE_MEM_BANK, SPRITE_MEM_BEE, "images/bee.bin");
    imageFileLoad(IMAGE_LOAD_BANK, SPRITE_MEM_BANK, SPRITE_MEM_GHOST, "images/ghost.bin");
    imageFileLoad(IMAGE_LOAD_BANK, SPRITE_MEM_BANK, SPRITE_MEM_SCORPION, "images/scorpion.bin");
    imageFileLoad(IMAGE_LOAD_BANK, SPRITE_MEM_BANK, SPRITE_MEM_WASP, "images/wasp.bin");
    imageFileLoad(IMAGE_LOAD_BANK, SPRITE_MEM_BANK, SPRITE_MEM_FISH1, "images/fish1.bin");
    imageFileLoad(IMAGE_LOAD_BANK, SPRITE_MEM_BANK, SPRITE_MEM_BIG_GHOST, "images/bigghost.bin");
    imageFileLoad(IMAGE_LOAD_BANK, SPRITE_MEM_BANK, SPRITE_MEM_LASER, "images/laser.bin");
    imageFileLoad(IMAGE_LOAD_BANK, SPRITE_MEM_BANK, SPRITE_MEM_SHIP, "images/ship.bin");
    imageFileLoad(IMAGE_LOAD_BANK, SPRITE_MEM_BANK, SPRITE_MEM_ENERGY, "images/energy.bin");
    imageFileLoad(IMAGE_LOAD_BANK, SPRITE_MEM_BANK, SPRITE_MEM_GOLD, "images/gold.bin");
    imageFileLoad(IMAGE_LOAD_BANK, SPRITE_MEM_BANK, SPRITE_MEM_SCUBA, "images/scuba.bin");
    imageFileLoad(IMAGE_LOAD_BANK, SPRITE_MEM_BANK, SPRITE_MEM_WEAPON, "images/weapon.bin");
    imageFileLoad(IMAGE_LOAD_BANK, SPRITE_MEM_BANK, SPRITE_MEM_BOOTS, "images/boots.bin");
    imageFileLoad(IMAGE_LOAD_BANK, SPRITE_MEM_BANK, SPRITE_MEM_EXTRA_LIFE, "images/extralife.bin");
}

void spriteIRQConfig() {
    // Turn on sprites
    x16SpriteSetGlobalOn();

    // Setup the IRQ handler for sprite collisions and enable global sprite collisions
    set_irq(&x16SpriteCollisionIRQHandler, IRQHandlerStack, IRQ_HANDLER_STACK_SIZE);
    x16SpriteCollisionsEnable();
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
    sp->collisionMask = ENEMY_COLLISION_MASK;
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
    sp->framesBetweenFacePlayer = 120;
    sp->framesUntilFacePlayer = 0;

    spriteInit(&sp->sprite);
    x16SpriteIdxSetXY(sp->sprite.index, sp->sprite.x, sp->sprite.y);
    x16SpriteIdxSetHFlip(sp->sprite.index, sp->sprite.animationDirection);
}

void playerCreate(Sprite *p, Entrance *entrance, unsigned char index) {
    spriteDefaults(p);
    
    p->index = index;
    p->active = 1;
    p->clrMode = 1;
    p->collisionMask = PLAYER_COLLISION_MASK;
    p->zDepth = Disabled;
    p->graphicsAddress = SPRITE_MEM_PLAYER;
    p->animationSpeed = 6;
    p->x = entrance->x * TILE_PIXEL_WIDTH;
    p->y = entrance->y * TILE_PIXEL_HEIGHT;
    p->xL = p->x * MOVE_FACTOR;
    p->yL = p->y * MOVE_FACTOR;
    p->lastX = p->x;
    p->lastY = p->y;

    spriteInit(p);
    x16SpriteIdxSetXY(p->index, p->x, p->y);
}

void snakeCreate(AISprite *snake, EnemyLayout *layout, unsigned char index)
{
    spriteDefaults(&snake->sprite);

    snake->sprite.graphicsAddress = SPRITE_MEM_SNAKE;
    snake->sprite.animationSpeed = 6;
    snake->sprite.speed = 8;

    snake->health = 4;
    snake->framesBetweenShots = 90;
    standardAISpriteConfig(snake, layout, index);
}

void beeCreate(AISprite *bee, EnemyLayout *layout, unsigned char index)
{
    spriteDefaults(&bee->sprite);

    bee->sprite.graphicsAddress = SPRITE_MEM_BEE;
    bee->sprite.animationSpeed = 6;
    bee->sprite.speed = 13;

    bee->health = 2;
    bee->framesBetweenShots = 70;
    standardAISpriteConfig(bee, layout, index);
}

void ghostCreate(AISprite *ghost, EnemyLayout *layout, unsigned char index)
{
    spriteDefaults(&ghost->sprite);
    
    ghost->sprite.graphicsAddress = SPRITE_MEM_GHOST;
    ghost->sprite.animationSpeed = 20;
    ghost->sprite.speed = 3;

    ghost->health = 10;
    ghost->framesBetweenShots = 180;
    standardAISpriteConfig(ghost, layout, index);
}

void scorpionCreate(AISprite *scorpion, EnemyLayout *layout, unsigned char index)
{
    spriteDefaults(&scorpion->sprite);
    
    scorpion->sprite.graphicsAddress = SPRITE_MEM_SCORPION;
    scorpion->sprite.animationSpeed = 6;
    scorpion->sprite.speed = 6;

    scorpion->health = 6;
    scorpion->framesBetweenShots = 90;
    standardAISpriteConfig(scorpion, layout, index);
}

void waspCreate(AISprite *wasp, EnemyLayout *layout, unsigned char index)
{
    spriteDefaults(&wasp->sprite);
    
    wasp->sprite.graphicsAddress = SPRITE_MEM_WASP;
    wasp->sprite.animationSpeed = 6;
    wasp->sprite.speed = 12;

    wasp->health = 3;
    wasp->framesBetweenShots = 120;
    standardAISpriteConfig(wasp, layout, index);
}

void fish1Create(AISprite *fish, EnemyLayout *layout, unsigned char index)
{
    spriteDefaults(&fish->sprite);

    fish->sprite.graphicsAddress = SPRITE_MEM_FISH1;
    fish->sprite.animationSpeed = 30;
    fish->sprite.speed = 3;

    fish->health = 3;
    fish->framesBetweenShots = 120;
    standardAISpriteConfig(fish, layout, index);
}

void bigGhostCreate(AISprite *ghost, EnemyLayout *layout, unsigned char index)
{
    spriteDefaults(&ghost->sprite);
    
    ghost->sprite.width = PX32;
    ghost->sprite.height = PX32;
    ghost->sprite.graphicsAddress = SPRITE_MEM_BIG_GHOST;
    ghost->sprite.frameSize = 1024; // Calculated as width * height
    ghost->sprite.animationSpeed = 30;
    ghost->sprite.speed = 3;

    ghost->health = 20;
    ghost->framesBetweenShots = 120;

    // The ghost is tall, shoot lasers out of his bottom half
    ghost->yLaserAdjust = TILE_PIXEL_HEIGHT;
    standardAISpriteConfig(ghost, layout, index);
}

void explosionSmallCreate(Sprite *b, unsigned char index) {
    spriteDefaults(b);

    b->index = index;
    b->active = 1;
    b->clrMode = 1;
    b->collisionMask = 0;
    b->zDepth = Disabled;
    b->graphicsAddress = SPRITE_MEM_EXP_SMALL;
    b->animationSpeed = 6;

    spriteInit(b);
}

void bulletCreate(Sprite *b, unsigned char index) {
    spriteDefaults(b);

    b->index = index;
    b->active = 1;
    b->clrMode = 1;
    b->collisionMask = PLAYER_BULLET_COLLISION_MASK;
    b->zDepth = Disabled;
    b->graphicsAddress = SPRITE_MEM_PLAYER_BULLET;
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
    b->collisionMask = ENEMY_LASER_COLLISION_MASK;
    b->zDepth = Disabled;
    b->graphicsAddress = SPRITE_MEM_LASER;
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
    b->graphicsAddress = SPRITE_MEM_ENERGY;
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
    b->graphicsAddress = SPRITE_MEM_GOLD;
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
    b->graphicsAddress = SPRITE_MEM_SCUBA;
    
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
    b->graphicsAddress = SPRITE_MEM_BOOTS;

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
    b->graphicsAddress = SPRITE_MEM_WEAPON;

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
    b->graphicsAddress = SPRITE_MEM_EXTRA_LIFE;

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
    b->graphicsAddress = SPRITE_MEM_SHIP;
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
