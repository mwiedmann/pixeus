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

void spriteMove(Sprite *sprite, short x, short y) {
    sprite->lastX = sprite->x;
    sprite->lastY = sprite->y;
    sprite->x = x;
    sprite->y = y;
}

void spriteMoveX(Sprite *sprite, short x) {
    sprite->lastX = sprite->x;
    sprite->x = x;
}

void spriteMoveY(Sprite *sprite, short y) {
    sprite->lastY = sprite->y;
    sprite->y = y;
}

void spriteMoveBack(Sprite *sprite) {
    sprite->x = sprite->lastX;
    sprite->y = sprite->lastY;
}

void spriteMoveBackX(Sprite *sprite) {
    sprite->x = sprite->lastX;
}

void spriteMoveBackY(Sprite *sprite) {
    sprite->y = sprite->lastY;
}