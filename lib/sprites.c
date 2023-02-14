#include "x16graphics.h"
#include "sprites.h"

unsigned char doOverlap(short x1, short y1, unsigned char width1, unsigned char height1,
    short x2, short y2, unsigned char width2, unsigned char height2)
{  
    // If one rectangle is on left side of other
    if (x1 > x2+width2 || x2 > x1+width1)
        return 0;
 
    // If one rectangle is above other
    if (y1 > y2+height2 || y2 > y1+height1)
        return 0;
 
    return 1;
}

void spriteInit(Sprite *sprite) {
    // This sets the current frame to something different for each sprite
    sprite->animationFrame = sprite->index % sprite->frames;
    sprite->animationCount = (sprite->index * 4) % sprite->animationSpeed;

    x16SpriteInit(
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

void spriteMoveToTile(Sprite *sprite, short x, short y, unsigned char tileWidth, unsigned char tileHeight) {
    sprite->x = x * tileWidth;
    sprite->y = y * tileHeight;
    sprite->lastX = sprite->x;
    sprite->lastY = sprite->y;
    sprite->xL = sprite->x * MOVE_FACTOR;
    sprite->yL = sprite->y * MOVE_FACTOR;
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
    short newX = xL / MOVE_FACTOR;
    sprite->lastX = sprite->x;
    sprite->xL = xL;
    sprite->x = newX;
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