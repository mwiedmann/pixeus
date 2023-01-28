#include <6502.h>
#include "gamesprites.h"
#include "x16graphics.h"
#include "memmap.h"
#include "sprites.h"
#include "level.h"

// External Sprites
extern unsigned char guyrunImage[];
extern unsigned char snakeImage[];

// Not sure how big the stack needs to be. Unclear how this works.
#define IRQ_HANDLER_STACK_SIZE 8
unsigned char IRQHandlerStack[IRQ_HANDLER_STACK_SIZE];

/**
 * Load image data from an array. Assumes vMem Bank and Address already set.
*/
void loadImage(unsigned char frames, unsigned char width, unsigned char height, unsigned char imageData[]) {
    unsigned char f, h, i;
    unsigned short start;

    for (f=0; f<frames; f++) {
        for (h=0; h<height; h++) {
            start = (f * width) + (h * frames * width);
            for (i=0; i<width; i++) {
                vMemSetData0(imageData[start+i]);
            }
        }
    }
}

void spriteDataLoad() {
    // You have to set the address you want to write to in VMEM using 9F20/21
    // 9F22 controls how much the VMEM address increments after each read/write
    // Then you can peek or poke using 0x9F23
    // The address is auto incremented and you can peek/poke again
    vMemSetBank(SPRITE_MEM_BANK);
    vMemSetAddr(SPRITE_MEM);
    vMemSetIncMode(1);

    loadImage(4, 16, 16, guyrunImage);
    loadImage(4, 16, 16, snakeImage);
}

void spriteIRQConfig() {
    // Turn on sprites
    x16SpriteSetGlobalOn();

    // Setup the IRQ handler for sprite collisions and enable global sprite collisions
    set_irq(&x16SpriteCollisionIRQHandler, IRQHandlerStack, IRQ_HANDLER_STACK_SIZE);
    x16SpriteCollisionsEnable();
}

void playerCreate(Sprite *p) {
    p->index = 1;
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

void snakeCreate(AISprite *snake, LevelLayout *layout)
{
    snake->sprite.index = 0;
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
    snake->sprite.y = (layout->y - 1) * TILE_PIXEL_HEIGHT;
    snake->sprite.xL = layout->x * MOVE_FACTOR;
    snake->sprite.yL = layout->y * MOVE_FACTOR;
    snake->sprite.lastX = snake->sprite.x;
    snake->sprite.lastY = snake->sprite.y;
    snake->sprite.speed = 5;

    snake->xTileStart = layout->x;
    snake->yTileStart = layout->y - 1;
    snake->xTileEnd = layout->x + layout->length;
    snake->yTileEnd = layout->y - 1;

    spriteInit(&snake->sprite);
    x16SpriteIdxSetXY(snake->sprite.spriteBank, snake->sprite.index, snake->sprite.x, snake->sprite.y);
    x16SpriteIdxSetHFlip(snake->sprite.spriteBank, snake->sprite.index, snake->sprite.animationDirection);
}

void bulletCreate(Sprite *b) {
    b->index = 2;
    b->spriteBank = 1;
    b->clrMode = 1;
    b->collisionMask = 0b1010;
    b->zDepth = Disabled;
    b->width = PX16;
    b->height = PX8;
    b->graphicsBank = 0;
    b->graphicsAddress = SPRITE_MEM;
    b->frames = 1;
    b->x = 320;
    b->y = 240;

    spriteInit(b);
}
