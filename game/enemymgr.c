#include <stdlib.h>

#include "x16graphics.h"
#include "gamesprites.h"
#include "gametiles.h"
#include "level.h"
#include "levelutils.h"
#include "layoutdefs.h"

#define ENEMY_LASER_DIST 200

AISprite masterEnemiesList[16];
Sprite enemyLasers[16];

AISprite *findEnemyCollision(Sprite *s) {
    unsigned char i;
    AISprite *enemy;

    for (i=0; i<16; i++) {
        enemy = &masterEnemiesList[i];
        if (doOverlap(
            s->x, s->y, pixelSizes[s->width], pixelSizes[s->height],
            enemy->sprite.x, enemy->sprite.y, pixelSizes[enemy->sprite.width], pixelSizes[enemy->sprite.height])) {
                return enemy;
            }
    }

    return 0;
}

unsigned char enemiesCreate(LevelOveralLayout *level, unsigned char nextSpriteIndex) {
    unsigned char i;

    for (i=0; i<level->enemyList->length; i++) {
        enemyCreate(
            level->enemyList->enemies[i].enemyType,
            &masterEnemiesList[i],
            &level->enemyList->enemies[i],
            nextSpriteIndex+i);
    }

    return level->enemyList->length;
}

void enemiesReset(unsigned char length) {
    unsigned char i;
    AISprite *enemy;
    Sprite *laser;

    // Reset enemies
    for (i=0; i<length; i++) {
        enemy = &masterEnemiesList[i];
        spriteMove(&enemy->sprite, 0, 0);
        enemy->sprite.active = 0;
        enemy->sprite.zDepth = Disabled;
        x16SpriteIdxSetZDepth(enemy->sprite.index, Disabled);
        x16SpriteIdxSetXY(enemy->sprite.index, enemy->sprite.x, enemy->sprite.y);
    }

    // Reset lasers
    for (i=0; i<16; i++) {
        laser = &enemyLasers[i];
        spriteMove(laser, 0, 0);
        laser->active = 0;
        laser->zDepth = Disabled;
        x16SpriteIdxSetZDepth(laser->index, Disabled);
        x16SpriteIdxSetXY(laser->index, laser->x, laser->y);
    }
}

void enemyShot(short x, short y, unsigned char direction) {
    unsigned char i;
    Sprite *laser;

    for (i=0; i<16; i++) {
        laser = &enemyLasers[i];

        if (laser->active == 0) {
            laser->active = 1;
            laser->animationDirection = direction;
            laser->zDepth = BetweenL0L1;
            laser->startX = x;
            spriteMove(laser, x, y);
            x16SpriteIdxSetXY(laser->index, laser->x, laser->y);
            x16SpriteIdxSetHFlip(laser->index, laser->animationDirection);
            x16SpriteIdxSetZDepth(laser->index, laser->zDepth);
            return;
        }
    }
}

void resetClosestLaser(short x, short y) {
    unsigned char i;
    Sprite *laser;

    for (i=0; i<16; i++) {
        laser = &enemyLasers[i];

        if (laser->active == 1 && abs(laser->x-x) <= TILE_PIXEL_WIDTH && abs(laser->y-y) <= TILE_PIXEL_HEIGHT) {
            break;
        } else {
            laser = 0;
        }
    }

    if (laser) {
        laser->active = 0;
        laser->zDepth = Disabled;
        spriteMove(laser, 0, 0);
        x16SpriteIdxSetZDepth(laser->index, laser->zDepth);
        x16SpriteIdxSetXY(laser->index, laser->x, laser->y);
    }
}

void enemiesMove(Sprite *player, unsigned char length) {
    unsigned char i,r;
    signed char tileCalc;
    AISprite *enemy;

    // Move enemies
    for (i=0; i<length; i++) {
        enemy = &masterEnemiesList[i];
        if (enemy->sprite.active == 1) {
            spriteMoveXL(&enemy->sprite, enemy->sprite.animationDirection == 0 ? enemy->sprite.xL-enemy->sprite.speed : enemy->sprite.xL+enemy->sprite.speed);
            spriteAnimationAdvance(&enemy->sprite);
            tileCalc = enemy->sprite.x / TILE_PIXEL_WIDTH;
            // Careful, can be -1 if on left edge (signed char)
            if (tileCalc <= (signed char)enemy->xTileStart - 1) {
                enemy->sprite.animationDirection = 1;
                x16SpriteIdxSetHFlip(enemy->sprite.index, enemy->sprite.animationDirection);
            } else if (tileCalc >= enemy->xTileEnd) {
                enemy->sprite.animationDirection = 0;
                x16SpriteIdxSetHFlip(enemy->sprite.index, enemy->sprite.animationDirection);
            }
            x16SpriteIdxSetXY(enemy->sprite.index, enemy->sprite.x, enemy->sprite.y);

            if (playerNear(player, enemy->sprite.x, enemy->sprite.y)) {
                // Player is near, face him
                if (enemy->framesUntilFacePlayer == 0) {
                    // Can't face player again until this hits 0
                    enemy->framesUntilFacePlayer = enemy->framesBetweenFacePlayer;

                    // Flip the enemy towards the player
                    enemy->sprite.animationDirection = facePlayer(player, enemy->sprite.x);
                    x16SpriteIdxSetHFlip(enemy->sprite.index, enemy->sprite.animationDirection);
                }
                // Shoot
                if (enemy->framesUntilNextShot == 0) {
                    r=rand();
                    enemy->framesUntilNextShot = r > 127 ? enemy->framesBetweenShots : enemy->framesBetweenShots/2;
                    // HACK: Move y 1px up to avoid ground collision
                    enemyShot(enemy->sprite.x, (enemy->sprite.y + enemy->yLaserAdjust)-1, enemy->sprite.animationDirection);
                } else {
                    enemy->framesUntilNextShot--;
                }

                // Enemy can face the player again when this hits 0
                enemy->framesUntilFacePlayer--;
            } else {
                // Can face the player immediately next time in range
                enemy->framesUntilFacePlayer = 0;
            }
        }
    }
}

void enemyLasersMove(LevelOveralLayout *level) {
    unsigned char i;
    TileInfo tileCollision;
    Sprite *laser;

    for (i=0; i<16; i++) {
        laser = &enemyLasers[i];
        if (laser->active == 1) {
            spriteMoveX(laser, laser->animationDirection == 0 ? laser->x-laser->speed : laser->x+laser->speed);
            x16SpriteIdxSetXY(laser->index, laser->x, laser->y);
            
            spriteTouchingTile(level, laser, &tileCollision);
            if (tileCollision.type == Ground || tileCollision.type == Ice || laser->x < 0 || laser->x > 639 || 
                abs(laser->x - laser->startX) >= ENEMY_LASER_DIST) {
                // TODO: Explosion for enemy lasers?
                laser->active = 0;
                laser->zDepth = Disabled;
                spriteMove(laser, 0, 0);
                x16SpriteIdxSetZDepth(laser->index, laser->zDepth);
                x16SpriteIdxSetXY(laser->index, laser->x, laser->y);
            }
        }
    }
}

unsigned char enemyLasersCreate(unsigned char nextSpriteIndex) {
    unsigned char i;

    for (i=0; i<16; i++) {
        laserCreate(&enemyLasers[i], nextSpriteIndex++);
    }

    return nextSpriteIndex;
}