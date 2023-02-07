#include <6502.h>
#include <peekpoke.h>
#include <cx16.h>
#include "gamesprites.h"
#include "x16graphics.h"
#include "memmap.h"
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
    imageFileLoad(2, SPRITE_MEM_BANK, SPRITE_MEM_PLAYER, "images/guyrun.bin", 1536);
    imageFileLoad(2, SPRITE_MEM_BANK, SPRITE_MEM_EXP_SMALL, "images/expsmall.bin", 1024);
    imageFileLoad(2, SPRITE_MEM_BANK, SPRITE_MEM_SNAKE, "images/snake.bin", 1024);
    imageFileLoad(2, SPRITE_MEM_BANK, SPRITE_MEM_BEE, "images/bee.bin", 1024);
    imageFileLoad(2, SPRITE_MEM_BANK, SPRITE_MEM_GHOST, "images/ghost.bin", 1024);
    imageFileLoad(2, SPRITE_MEM_BANK, SPRITE_MEM_SCORPION, "images/scorpion.bin", 1024);
    imageFileLoad(2, SPRITE_MEM_BANK, SPRITE_MEM_WASP, "images/wasp.bin", 1024);
    imageFileLoad(2, SPRITE_MEM_BANK, SPRITE_MEM_FISH1, "images/fish1.bin", 1024);
    imageFileLoad(2, SPRITE_MEM_BANK, SPRITE_MEM_LASER, "images/laser.bin", 1024);

    // Back to memory bank 1
    POKE(0, 1);
}

void spriteIRQConfig() {
    // Turn on sprites
    x16SpriteSetGlobalOn();

    // Setup the IRQ handler for sprite collisions and enable global sprite collisions
    set_irq(&x16SpriteCollisionIRQHandler, IRQHandlerStack, IRQ_HANDLER_STACK_SIZE);
    x16SpriteCollisionsEnable();
}

void standardAISpriteConfig(AISprite *sp, EnemyLayout *layout, unsigned char index)
{
    sp->sprite.index = index;
    sp->sprite.active=1;
    sp->sprite.clrMode = 1;
    
    // Possible zDepth will change but leave for now
    sp->sprite.zDepth = BetweenL0L1;
    sp->sprite.graphicsBank = SPRITE_MEM_BANK;
    sp->sprite.animationCount = 0;    
    sp->sprite.animationStopFrame = 0;
    sp->sprite.animationDirection = layout->moveDir;
    sp->sprite.animationFrame = 0;
    sp->sprite.x = layout->x * TILE_PIXEL_WIDTH;
    sp->sprite.y = layout->y * TILE_PIXEL_HEIGHT;
    sp->sprite.xL = sp->sprite.x * MOVE_FACTOR;
    sp->sprite.yL = sp->sprite.y * MOVE_FACTOR;
    sp->sprite.lastX = sp->sprite.x;
    sp->sprite.lastY = sp->sprite.y;
    sp->sprite.lastTileX = 0;
    sp->sprite.lastTileY = 0;

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
    p->index = index;
    p->active = 1;
    p->clrMode = 1;
    p->collisionMask = PLAYER_COLLISION_MASK;
    p->zDepth = BetweenL0L1;
    p->width = PX16;
    p->height = PX16;
    p->graphicsBank = SPRITE_MEM_BANK;
    p->graphicsAddress = SPRITE_MEM_PLAYER;
    p->frames = 4;
    p->frameSize = 256; // Calculated as width * height
    p->animationCount = 0;
    p->animationSpeed = 6;
    p->animationStopFrame = 0;
    p->animationDirection = 0;
    p->animationFrame = 0;
    p->x = entrance->x * TILE_PIXEL_WIDTH;
    p->y = entrance->y * TILE_PIXEL_HEIGHT;
    p->xL = p->x * MOVE_FACTOR;
    p->yL = p->y * MOVE_FACTOR;
    p->lastX = p->x;
    p->lastY = p->y;
    p->speed = 12;
    p->swimSpeed = 6;

    spriteInit(p);
    x16SpriteIdxSetXY(p->index, p->x, p->y);
}

void snakeCreate(AISprite *snake, EnemyLayout *layout, unsigned char index)
{
    snake->sprite.collisionMask = ENEMY_COLLISION_MASK;
    snake->sprite.width = PX16;
    snake->sprite.height = PX16;
    snake->sprite.graphicsAddress = SPRITE_MEM_SNAKE;
    snake->sprite.frames = 4;
    snake->sprite.frameSize = 256; // Calculated as width * height
    snake->sprite.animationSpeed = 6;
    snake->sprite.speed = 8;

    snake->health = 4;
    snake->framesBetweenShots = 90;
    standardAISpriteConfig(snake, layout, index);
}

void beeCreate(AISprite *bee, EnemyLayout *layout, unsigned char index)
{
    bee->sprite.collisionMask = ENEMY_COLLISION_MASK;
    bee->sprite.width = PX16;
    bee->sprite.height = PX16;
    bee->sprite.graphicsAddress = SPRITE_MEM_BEE;
    bee->sprite.frames = 4;
    bee->sprite.frameSize = 256; // Calculated as width * height
    bee->sprite.animationSpeed = 6;
    bee->sprite.speed = 13;

    bee->health = 2;
    bee->framesBetweenShots = 70;
    standardAISpriteConfig(bee, layout, index);
}

void ghostCreate(AISprite *ghost, EnemyLayout *layout, unsigned char index)
{
    ghost->sprite.collisionMask = ENEMY_COLLISION_MASK;
    ghost->sprite.width = PX16;
    ghost->sprite.height = PX16;
    ghost->sprite.graphicsAddress = SPRITE_MEM_GHOST;
    ghost->sprite.frames = 4;
    ghost->sprite.frameSize = 256; // Calculated as width * height
    ghost->sprite.animationSpeed = 20;
    ghost->sprite.speed = 3;

    ghost->health = 10;
    ghost->framesBetweenShots = 180;
    standardAISpriteConfig(ghost, layout, index);
}

void scorpionCreate(AISprite *scorpion, EnemyLayout *layout, unsigned char index)
{
    scorpion->sprite.collisionMask = ENEMY_COLLISION_MASK;
    scorpion->sprite.width = PX16;
    scorpion->sprite.height = PX16;
    scorpion->sprite.graphicsAddress = SPRITE_MEM_SCORPION;
    scorpion->sprite.frames = 4;
    scorpion->sprite.frameSize = 256; // Calculated as width * height
    scorpion->sprite.animationSpeed = 6;
    scorpion->sprite.speed = 6;

    scorpion->health = 6;
    scorpion->framesBetweenShots = 90;
    standardAISpriteConfig(scorpion, layout, index);
}

void waspCreate(AISprite *wasp, EnemyLayout *layout, unsigned char index)
{
    wasp->sprite.collisionMask = ENEMY_COLLISION_MASK;
    wasp->sprite.width = PX16;
    wasp->sprite.height = PX16;
    wasp->sprite.graphicsAddress = SPRITE_MEM_WASP;
    wasp->sprite.frames = 4;
    wasp->sprite.frameSize = 256; // Calculated as width * height
    wasp->sprite.animationSpeed = 6;
    wasp->sprite.speed = 12;

    wasp->health = 3;
    wasp->framesBetweenShots = 120;
    standardAISpriteConfig(wasp, layout, index);
}

void fish1Create(AISprite *fish, EnemyLayout *layout, unsigned char index)
{
    fish->sprite.collisionMask = ENEMY_COLLISION_MASK;
    fish->sprite.width = PX16;
    fish->sprite.height = PX16;
    fish->sprite.graphicsAddress = SPRITE_MEM_FISH1;
    fish->sprite.frames = 4;
    fish->sprite.frameSize = 256; // Calculated as width * height
    fish->sprite.animationSpeed = 30;
    fish->sprite.speed = 3;

    fish->health = 3;
    fish->framesBetweenShots = 120;
    standardAISpriteConfig(fish, layout, index);
}

void explosionSmallCreate(Sprite *b, unsigned char index) {
    b->index = index;
    b->active = 1;
    b->clrMode = 1;
    b->collisionMask = 0;
    b->zDepth = Disabled;
    b->width = PX16;
    b->height = PX16;
    b->graphicsBank = SPRITE_MEM_BANK;
    b->graphicsAddress = SPRITE_MEM_EXP_SMALL;
    b->frames = 4;
    b->frameSize = 256; // Calculated as width * height
    b->animationSpeed = 6;

    spriteInit(b);
}

void bulletCreate(Sprite *b, unsigned char index) {
    b->index = index;
    b->active = 1;
    b->clrMode = 1;
    b->collisionMask = PLAYER_BULLET_COLLISION_MASK;
    b->zDepth = Disabled;
    b->width = PX16;
    b->height = PX16;
    b->graphicsBank = SPRITE_MEM_BANK;
    b->graphicsAddress = SPRITE_MEM_PLAYER_BULLET;
    b->frames = 1;
    b->x = 320;
    b->y = 240;
    b->speed = 4;
    b->swimSpeed = 3;

    spriteInit(b);
}

void laserCreate(Sprite *b, unsigned char index) {
    b->index = index;
    b->active = 0;
    b->clrMode = 1;
    b->collisionMask = ENEMY_LASER_COLLISION_MASK;
    b->zDepth = Disabled;
    b->width = PX16;
    b->height = PX16;
    b->graphicsBank = SPRITE_MEM_BANK;
    b->graphicsAddress = SPRITE_MEM_LASER;
    b->frames = 1;
    b->x = 320;
    b->y = 240;
    b->speed = 4;
    b->swimSpeed = 3;

    spriteInit(b);
}
