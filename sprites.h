#ifndef SPRITES
#define SPRITES

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
    short x;
    short y;
} Sprite;

void spriteInit(Sprite *sprite);

#endif