#include <stdlib.h>
#include <joystick.h>
#include <cx16.h>

#include "waitforjiffy.h"
#include "level.h"
#include "sprites.h"
#include "gametiles.h"
#include "sound.h"

#define PLAYER_NEAR_X 180
#define PLAYER_NEAR_Y 40

Entity *playerTouchingEntity(EntityList *entityList, Sprite *sprite) {
    unsigned char i,x,y;
    
    x = ((sprite->x + TILE_PIXEL_WIDTH_HALF) / TILE_PIXEL_WIDTH);
    y = ((sprite->y + TILE_PIXEL_HEIGHT_HALF) / TILE_PIXEL_HEIGHT);

    for (i=0; i<entityList->length; i++) {
        if (entityList->entities[i].entityType != 0 &&
            y == entityList->entities[i].y &&
            x == entityList->entities[i].x) {
                return &entityList->entities[i];
            }
    }

    return 0;
}

void spriteHeadTouchingTile(LevelOveralLayout *level, Sprite *sprite, TileInfo *tileCollision, unsigned char yAdjust) {
    tileCollision->x = ((sprite->x + TILE_PIXEL_WIDTH_HALF) / TILE_PIXEL_WIDTH);
    tileCollision->y = ((sprite->y + yAdjust) / TILE_PIXEL_HEIGHT);

    // NOTE: Some IDEs complain about this array cast but it compiles
    tileCollision->type = ((unsigned char(*)[40])level->movementTypes)[tileCollision->y][tileCollision->x];
}

void spriteTouchingTile(LevelOveralLayout *level, Sprite *sprite, TileInfo *tileCollision) {
    tileCollision->x = ((sprite->x + TILE_PIXEL_WIDTH_HALF) / TILE_PIXEL_WIDTH);
    tileCollision->y = (sprite->y + pixelSizes[sprite->height]) / TILE_PIXEL_HEIGHT;

    // NOTE: Some IDEs complain about this array cast but it compiles
    tileCollision->type = ((unsigned char(*)[40])level->movementTypes)[tileCollision->y][tileCollision->x];
}

unsigned char playerNear(Sprite *player, short x, short y) {
    // TODO: Eventually we will want the distance here to come from the specific enemy
    // Some enemies may be dumber and not see the player until very close and others smarter
    // this works fine as a general AI awareness routine for now.
    return (abs(player->x-x) <= PLAYER_NEAR_X && abs(player->y-y)<=PLAYER_NEAR_Y) ? 1 : 0;
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
        if (entranceList->entrances[i].entityType == EntranceEnum && entranceList->entrances[i].id == entranceId) {
            return &entranceList->entrances[i];
        }
    }

    return 0;
}

void waitForButtonPress() {
    unsigned char joy = 0;

    while(1) {
        waitforjiffy();
        
        joy = joy_read(0) | joy_read(1);

        // Exit when either button is pressed
        if (JOY_BTN_1(joy)) {
            while(JOY_BTN_1(joy)) {
                waitforjiffy();
        
                joy = joy_read(0) | joy_read(1);
            }
            break;
        } else if (JOY_BTN_2(joy)) {
            while(JOY_BTN_2(joy)) {
                waitforjiffy();
        
                joy = joy_read(0) | joy_read(1);
            }
            break;
        }
    }
}

unsigned char readButtonPress() {
    unsigned char joy = 0;
    unsigned char pressed = 0;

    joy = joy_read(0) | joy_read(1);

    // Exit when either button is pressed
    if (JOY_BTN_1(joy)) {
        while(JOY_BTN_1(joy)) {
            waitforjiffy();
        
            joy = joy_read(0) | joy_read(1);
        }
        pressed = 1;
    } else if (JOY_BTN_2(joy)) {
        while(JOY_BTN_2(joy)) {
            waitforjiffy();
        
            joy = joy_read(0) | joy_read(1);
        }
        pressed = 1;
    }

    return pressed;
}