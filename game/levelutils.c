#include <stdlib.h>

#include "level.h"
#include "memmap.h"
#include "sprites.h"

Exit *playerTouchingExit(LevelOveralLayout *level, Sprite *sprite) {
    unsigned char i,x,y;
    
    x = ((sprite->x + TILE_PIXEL_WIDTH_HALF) / TILE_PIXEL_WIDTH);
    y = ((sprite->y + TILE_PIXEL_HEIGHT_HALF) / TILE_PIXEL_HEIGHT);

    for (i=0; i<level->exitList->length; i++) {
        if (y == level->exitList->exits[i].y &&
            x == level->exitList->exits[i].x) {
                return &level->exitList->exits[i];
            }
    }

    return 0;
}

void spriteTouchingTile(LevelOveralLayout *level, Sprite *sprite, TileInfo *tileCollision) {
    tileCollision->x = ((sprite->x + TILE_PIXEL_WIDTH_HALF) / TILE_PIXEL_WIDTH);
    tileCollision->y = (sprite->y + pixelSizes[sprite->height]) / TILE_PIXEL_HEIGHT;

    tileCollision->type = ((unsigned char[30][40])level->movementTypes)[tileCollision->y][tileCollision->x];
}

unsigned char playerNear(Sprite *player, short x, short y) {
    return (abs(player->x-x) <= 170 && abs(player->y-y)<=40) ? 1 : 0;
}

unsigned char facePlayer(Sprite *player, short x) {
    return x > player->x ? 0 : 1;
}

void smallExplosion(Sprite *expSmall, ZDepth zDepth, short x, short y) {
    // Explosion
    spriteMove(expSmall, x, y);
    x16SpriteIdxSetGraphicsPointer(expSmall->index, expSmall->clrMode, expSmall->graphicsBank, expSmall->graphicsAddress);
    x16SpriteIdxSetXY(expSmall->index, expSmall->x, expSmall->y);
    expSmall->zDepth = zDepth;
    x16SpriteIdxSetZDepth(expSmall->index, expSmall->zDepth);
    expSmall->animationCount = 0;
    expSmall->animationFrame = 0;
}

Entrance *findEntranceForExit(EntranceList *entranceList, unsigned char entranceId) {
    unsigned char i;

    for (i=0; i<entranceList->length; i++) {
        if (entranceList->entrances[i].id == entranceId) {
            return &entranceList->entrances[i];
        }
    }

    return 0;
}
