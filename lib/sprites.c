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
    sprite->xL = x * MOVE_FACTOR;
    sprite->yL = y * MOVE_FACTOR;
}

void spriteMoveX(Sprite *sprite, short x) {
    sprite->lastX = sprite->x;
    sprite->x = x;
    sprite->xL = x * MOVE_FACTOR;
}

void spriteMoveY(Sprite *sprite, short y) {
    sprite->lastY = sprite->y;
    sprite->y = y;
    sprite->yL = y * MOVE_FACTOR;
}

void spriteMoveL(Sprite *sprite, short xL, short yL) {
    sprite->lastX = sprite->x;
    sprite->lastY = sprite->y;
    sprite->xL = xL;
    sprite->yL = yL;
    sprite->x = xL / MOVE_FACTOR;
    sprite->y = yL / MOVE_FACTOR;
}

void spriteMoveXL(Sprite *sprite, short xL) {
    sprite->lastX = sprite->x;
    sprite->xL = xL;
    sprite->x = xL / MOVE_FACTOR;
}

void spriteMoveYL(Sprite *sprite, short yL) {
    sprite->lastY = sprite->y;
    sprite->yL = yL;
    sprite->y = yL / MOVE_FACTOR;
}

void spriteMoveBack(Sprite *sprite) {
    sprite->x = sprite->lastX;
    sprite->y = sprite->lastY;
    sprite->xL = sprite->lastX * MOVE_FACTOR;
    sprite->yL = sprite->lastY * MOVE_FACTOR;
}

void spriteMoveBackX(Sprite *sprite) {
    sprite->x = sprite->lastX;
    sprite->xL = sprite->lastX * MOVE_FACTOR;
}

void spriteMoveBackY(Sprite *sprite) {
    sprite->y = sprite->lastY;
    sprite->yL = sprite->lastY * MOVE_FACTOR;
}