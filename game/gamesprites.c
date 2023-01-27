#include <6502.h>
#include "x16graphics.h"
#include "memmap.h"
#include "sprites.h"

// External Sprites
extern unsigned char guyrunImage[];

// Not sure how big the stack needs to be. Unclear how this works.
#define IRQ_HANDLER_STACK_SIZE 8
unsigned char IRQHandlerStack[IRQ_HANDLER_STACK_SIZE];

void spriteDataLoad() {
    unsigned short i;
    unsigned char frames, f;
    unsigned char width;
    unsigned char height, h;
    unsigned short start;

    // You have to set the address you want to write to in VMEM using 9F20/21
    // 9F22 controls how much the VMEM address increments after each read/write
    // Then you can peek or poke using 0x9F23
    // The address is auto incremented and you can peek/poke again
    vMemSetBank0();
    vMemSetAddr(SPRITE_MEM);
    vMemSetIncMode(1);
    
    // Poke a block of solid data into vmem to use as sprite data
    // Big enough for 32x32 sprites
    for (i=0; i<32*32; i++) {
        vMemSetData0(199);
    }

    // This has multiple frames so we have to jump around
    // Need to know the width, height, and number of frames
    frames = 8;
    height = 32;
    width = 32;
    for (f=0; f<8; f++) {
        for (h=0; h<height; h++) {
            start = (f * width) + (h * frames * width);
            for (i=0; i<width; i++) {
                vMemSetData0(guyrunImage[start+i]);
            }
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

void playerCreate(Sprite *p) {
    p->index = 1;
    p->spriteBank = 1;
    p->clrMode = 1;
    p->collisionMask = 0b1101;
    p->zDepth = BetweenL0L1;
    p->width = PX32;
    p->height = PX32;
    p->graphicsBank = 0;
    p->graphicsAddress = SPRITE_MEM+1024;
    p->frames = 8;
    p->frameSize = 1024; // Calculated as width * height
    p->animationCount = 0;
    p->animationSpeed = 6;
    p->animationStopFrame = 4;
    p->animationDirection = 0;
    p->animationFrame = 0;
    p->x = 380;
    p->y = 235;
    p->speed = 2;

    spriteInit(p);
    x16SpriteIdxSetXY(p->spriteBank, p->index, p->x, p->y);
}

void badguyCreate(Sprite *bg) {
    bg->index = 0;
    bg->spriteBank = 1;
    bg->clrMode = 1;
    bg->collisionMask = 0b1011;
    bg->zDepth = BetweenL0L1;
    bg->width = PX32;
    bg->height = PX32;
    bg->graphicsBank = 0;
    bg->graphicsAddress = SPRITE_MEM;
    bg->frames = 1;
    bg->x = 320;
    bg->y = 240;

    spriteInit(bg);
    x16SpriteIdxSetXY(bg->spriteBank, bg->index, bg->x, bg->y);
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
