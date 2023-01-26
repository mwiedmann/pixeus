#include "x16graphics.h"
#include "sprites.h"

void spriteInit(Sprite *sprite) {
    x16SpriteInit(
        sprite->spriteBank, 
        sprite->index,
        sprite->clrMode,
        sprite->graphicsBank,
        sprite->graphicsAddress,
        sprite->collisionMask,
        sprite->zDepth,
        sprite->width,
        sprite->height);
}