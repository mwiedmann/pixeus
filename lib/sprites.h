#ifndef SPRITES_H
#define SPRITES_H

#include "x16graphics.h"

#define  MOVE_FACTOR 10

typedef struct Sprite {
    unsigned char index;
    unsigned char clrMode;
    unsigned char collisionMask;
    ZDepth zDepth;
    SpriteSize width;
    SpriteSize height;
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
    short lastX;
    short lastY;
    short xL;
    short yL;
    unsigned char active;
    unsigned char going;
    unsigned char speed;
} Sprite;

unsigned char doOverlap(short x1, short y1, unsigned char width1, unsigned char height1,
    short x2, short y2, unsigned char width2, unsigned char height2);
void spriteInit(Sprite *sprite);
void spriteMoveToTile(Sprite *sprite, short x, short y, unsigned char tileWidth, unsigned char tileHeight);
void spriteMove(Sprite *sprite, short x, short y);
void spriteMoveX(Sprite *sprite, short x);
void spriteMoveY(Sprite *sprite, short y);
void spriteMoveBack(Sprite *sprite);
void spriteMoveBackX(Sprite *sprite);
void spriteMoveBackY(Sprite *sprite);
void spriteMoveL(Sprite *sprite, short xL, short yL);
void spriteMoveXL(Sprite *sprite, short xL);
void spriteMoveYL(Sprite *sprite, short yL);

#endif