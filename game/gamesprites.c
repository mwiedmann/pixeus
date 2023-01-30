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
    imageFileLoad(2, SPRITE_MEM_BANK, SPRITE_MEM_PLAYER, "images/guyrun.bin", 6, 1, 16, 16);
    imageFileLoad(2, SPRITE_MEM_BANK, SPRITE_MEM_SNAKE, "images/snake.bin", 4, 1, 16, 16);
    imageFileLoad(2, SPRITE_MEM_BANK, SPRITE_MEM_BEE, "images/bee.bin", 4, 1, 16, 16);

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

void playerCreate(Sprite *p, unsigned char index) {
    p->index = index;
    p->spriteBank = 1;
    p->clrMode = 1;
    p->collisionMask = 0b1101;
    p->zDepth = BetweenL0L1;
    p->width = PX16;
    p->height = PX16;
    p->graphicsBank = 0;
    p->graphicsAddress = SPRITE_MEM_PLAYER;
    p->frames = 4;
    p->frameSize = 256; // Calculated as width * height
    p->animationCount = 0;
    p->animationSpeed = 6;
    p->animationStopFrame = 0;
    p->animationDirection = 0;
    p->animationFrame = 0;
    p->x = 150;
    p->y = 50;
    p->xL = p->x * MOVE_FACTOR;
    p->yL = p->y * MOVE_FACTOR;
    p->lastX = p->x;
    p->lastY = p->y;
    p->speed = 12;

    spriteInit(p);
    x16SpriteIdxSetXY(p->spriteBank, p->index, p->x, p->y);
}

void snakeCreate(AISprite *snake, EnemyLayout *layout, unsigned char index)
{
    snake->sprite.index = index;
    snake->sprite.spriteBank = 1;
    snake->sprite.clrMode = 1;
    snake->sprite.collisionMask = 0b1011;
    snake->sprite.zDepth = BetweenL0L1;
    snake->sprite.width = PX16;
    snake->sprite.height = PX16;
    snake->sprite.graphicsBank = 0;
    snake->sprite.graphicsAddress = SPRITE_MEM_SNAKE;
    snake->sprite.frames = 4;
    snake->sprite.frameSize = 256; // Calculated as width * height
    snake->sprite.animationCount = 0;
    snake->sprite.animationSpeed = 6;
    snake->sprite.animationStopFrame = 0;
    snake->sprite.animationDirection = 1;
    snake->sprite.animationFrame = 0;
    snake->sprite.x = layout->x * TILE_PIXEL_WIDTH;
    snake->sprite.y = layout->y * TILE_PIXEL_HEIGHT;
    snake->sprite.xL = snake->sprite.x * MOVE_FACTOR;
    snake->sprite.yL = snake->sprite.y * MOVE_FACTOR;
    snake->sprite.lastX = snake->sprite.x;
    snake->sprite.lastY = snake->sprite.y;
    snake->sprite.speed = 6;

    snake->xTileStart = layout->x;
    snake->yTileStart = layout->y;
    snake->xTileEnd = layout->x + layout->length;
    snake->yTileEnd = layout->y;

    spriteInit(&snake->sprite);
    x16SpriteIdxSetXY(snake->sprite.spriteBank, snake->sprite.index, snake->sprite.x, snake->sprite.y);
    x16SpriteIdxSetHFlip(snake->sprite.spriteBank, snake->sprite.index, snake->sprite.animationDirection);
}

void beeCreate(AISprite *bee, EnemyLayout *layout, unsigned char index)
{
    bee->sprite.index = index;
    bee->sprite.spriteBank = 1;
    bee->sprite.clrMode = 1;
    bee->sprite.collisionMask = 0b1011;
    bee->sprite.zDepth = BetweenL0L1;
    bee->sprite.width = PX16;
    bee->sprite.height = PX16;
    bee->sprite.graphicsBank = 0;
    bee->sprite.graphicsAddress = SPRITE_MEM_BEE;
    bee->sprite.frames = 4;
    bee->sprite.frameSize = 256; // Calculated as width * height
    bee->sprite.animationCount = 0;
    bee->sprite.animationSpeed = 6;
    bee->sprite.animationStopFrame = 0;
    bee->sprite.animationDirection = 1;
    bee->sprite.animationFrame = 0;
    bee->sprite.x = layout->x * TILE_PIXEL_WIDTH;
    bee->sprite.y = layout->y * TILE_PIXEL_HEIGHT;
    bee->sprite.xL = bee->sprite.x * MOVE_FACTOR;
    bee->sprite.yL = bee->sprite.y * MOVE_FACTOR;
    bee->sprite.lastX = bee->sprite.x;
    bee->sprite.lastY = bee->sprite.y;
    bee->sprite.speed = 13;

    bee->xTileStart = layout->x;
    bee->yTileStart = layout->y;
    bee->xTileEnd = layout->x + layout->length;
    bee->yTileEnd = layout->y;

    spriteInit(&bee->sprite);
    x16SpriteIdxSetXY(bee->sprite.spriteBank, bee->sprite.index, bee->sprite.x, bee->sprite.y);
    x16SpriteIdxSetHFlip(bee->sprite.spriteBank, bee->sprite.index, bee->sprite.animationDirection);
}

void bulletCreate(Sprite *b, unsigned char index) {
    b->index = index;
    b->spriteBank = 1;
    b->clrMode = 1;
    b->collisionMask = 0b1010;
    b->zDepth = Disabled;
    b->width = PX16;
    b->height = PX16;
    b->graphicsBank = 0;
    b->graphicsAddress = SPRITE_MEM_PLAYER_BULLET;
    b->frames = 1;
    b->x = 320;
    b->y = 240;
    b->speed = 4;

    spriteInit(b);
}
