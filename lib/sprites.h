#ifndef SPRITES_H
#define SPRITES_H

#include "x16graphics.h"

typedef struct Sprite {
    unsigned char index;
    unsigned char spriteBank;
    unsigned char clrMode;
    unsigned char collisionMask;
    enum ZDepth zDepth;
    enum SpriteSize width;
    enum SpriteSize height;
    unsigned char graphicsBank;
    unsigned short graphicsAddress;
    unsigned char frames;
    unsigned short frameSize;
    unsigned char animationCount;
    unsigned char animationSpeed;
    unsigned char animationStopFrame;
    unsigned char animationDirection;
    unsigned char animationFrame;
    short x;
    short y;
    unsigned char active;
    unsigned char going;
    unsigned char speed;
} Sprite;

void spriteInit(Sprite *sprite);

#endif