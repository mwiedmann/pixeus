#include <6502.h>
#include <peekpoke.h>
#include <cx16.h>
#include "gamesprites.h"
#include "x16graphics.h"
#include "memmap.h"
#include "sprites.h"
#include "level.h"
#include "imageload.h"

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
    sp->sprite.clrMode = 1;
    
    // Possible zDepth will change but leave for now
    sp->sprite.zDepth = BetweenL0L1;
    sp->sprite.graphicsBank = SPRITE_MEM_BANK;
    sp->sprite.animationCount = 0;    
    sp->sprite.animationStopFrame = 0;
    sp->sprite.animationDirection = 1;
    sp->sprite.animationFrame = 0;
    sp->sprite.x = layout->x * TILE_PIXEL_WIDTH;
    sp->sprite.y = layout->y * TILE_PIXEL_HEIGHT;
    sp->sprite.xL = sp->sprite.x * MOVE_FACTOR;
    sp->sprite.yL = sp->sprite.y * MOVE_FACTOR;
    sp->sprite.lastX = sp->sprite.x;
    sp->sprite.lastY = sp->sprite.y;
    
    sp->xTileStart = layout->x;
    sp->yTileStart = layout->y;
    sp->xTileEnd = layout->x + layout->length;
    sp->yTileEnd = layout->y;

    spriteInit(&sp->sprite);
    x16SpriteIdxSetXY(sp->sprite.index, sp->sprite.x, sp->sprite.y);
    x16SpriteIdxSetHFlip(sp->sprite.index, sp->sprite.animationDirection);
}

void playerCreate(Sprite *p, Entrance *entrance, unsigned char index) {
    p->index = index;
    p->clrMode = 1;
    p->collisionMask = 0b1101;
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
    p->x = entrance->x;
    p->y = entrance->y;
    p->xL = p->x * MOVE_FACTOR;
    p->yL = p->y * MOVE_FACTOR;
    p->lastX = p->x;
    p->lastY = p->y;
    p->speed = 12;

    spriteInit(p);
    x16SpriteIdxSetXY(p->index, p->x, p->y);
}

void snakeCreate(AISprite *snake, EnemyLayout *layout, unsigned char index)
{
    snake->sprite.collisionMask = 0b1011;
    snake->sprite.width = PX16;
    snake->sprite.height = PX16;
    snake->sprite.graphicsAddress = SPRITE_MEM_SNAKE;
    snake->sprite.frames = 4;
    snake->sprite.frameSize = 256; // Calculated as width * height
    snake->sprite.animationSpeed = 6;
    snake->sprite.speed = 6;

    standardAISpriteConfig(snake, layout, index);
}

void beeCreate(AISprite *bee, EnemyLayout *layout, unsigned char index)
{
    bee->sprite.collisionMask = 0b1011;
    bee->sprite.width = PX16;
    bee->sprite.height = PX16;
    bee->sprite.graphicsAddress = SPRITE_MEM_BEE;
    bee->sprite.frames = 4;
    bee->sprite.frameSize = 256; // Calculated as width * height
    bee->sprite.animationSpeed = 6;
    bee->sprite.speed = 13;

    standardAISpriteConfig(bee, layout, index);
}

void ghostCreate(AISprite *ghost, EnemyLayout *layout, unsigned char index)
{
    ghost->sprite.collisionMask = 0b1011;
    ghost->sprite.width = PX16;
    ghost->sprite.height = PX16;
    ghost->sprite.graphicsAddress = SPRITE_MEM_GHOST;
    ghost->sprite.frames = 4;
    ghost->sprite.frameSize = 256; // Calculated as width * height
    ghost->sprite.animationSpeed = 20;
    ghost->sprite.speed = 3;

    standardAISpriteConfig(ghost, layout, index);
}

void scorpionCreate(AISprite *scorpion, EnemyLayout *layout, unsigned char index)
{
    scorpion->sprite.collisionMask = 0b1011;
    scorpion->sprite.width = PX16;
    scorpion->sprite.height = PX16;
    scorpion->sprite.graphicsAddress = SPRITE_MEM_SCORPION;
    scorpion->sprite.frames = 4;
    scorpion->sprite.frameSize = 256; // Calculated as width * height
    scorpion->sprite.animationSpeed = 6;
    scorpion->sprite.speed = 8;

    standardAISpriteConfig(scorpion, layout, index);
}

void waspCreate(AISprite *wasp, EnemyLayout *layout, unsigned char index)
{
    wasp->sprite.collisionMask = 0b1011;
    wasp->sprite.width = PX16;
    wasp->sprite.height = PX16;
    wasp->sprite.graphicsAddress = SPRITE_MEM_WASP;
    wasp->sprite.frames = 4;
    wasp->sprite.frameSize = 256; // Calculated as width * height
    wasp->sprite.animationSpeed = 6;
    wasp->sprite.speed = 12;

    standardAISpriteConfig(wasp, layout, index);
}

void explosionSmallCreate(Sprite *b, unsigned char index) {
    b->index = index;
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
    b->clrMode = 1;
    b->collisionMask = 0b1010;
    b->zDepth = Disabled;
    b->width = PX16;
    b->height = PX16;
    b->graphicsBank = SPRITE_MEM_BANK;
    b->graphicsAddress = SPRITE_MEM_PLAYER_BULLET;
    b->frames = 1;
    b->x = 320;
    b->y = 240;
    b->speed = 4;

    spriteInit(b);
}
