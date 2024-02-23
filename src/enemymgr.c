#include <stdlib.h>
#include <cx16.h>

#include "x16graphics.h"
#include "gamesprites.h"
#include "gametiles.h"
#include "level.h"
#include "levelutils.h"
#include "layoutdefs.h"
#include "entitymgr.h"
#include "memmap.h"

#define ENEMY_LASER_DIST 200
#define ENEMY_JUMP_SPEED_NORMAL 13
#define ENEMY_WATER_JUMP_SPEED_NORMAL 4
#define ENEMY_JUMP_FRAMES 20
#define ENEMY_WATER_JUMP_FRAMES 90
#define ENEMY_LASER_COUNT 8
#define ENEMY_SPRITE_COUNT 8

AISprite masterEnemiesList[ENEMY_SPRITE_COUNT];
Sprite enemyLasers[ENEMY_LASER_COUNT];

AISprite *findEnemyCollision(Sprite *s) {
    unsigned char i;
    AISprite *enemy;

    for (i=0; i<ENEMY_SPRITE_COUNT; i++) {
        enemy = &masterEnemiesList[i];
        if (doOverlap(
            s->x, s->y, pixelSizes[s->width], pixelSizes[s->height],
            enemy->sprite.x, enemy->sprite.y, pixelSizes[enemy->sprite.width], pixelSizes[enemy->sprite.height])) {
                return enemy;
            }
    }

    return 0;
}

Sprite *findEnemyLaserCollision(Sprite *s) {
    unsigned char i;
    Sprite *laser;

    for (i=0; i<ENEMY_LASER_COUNT; i++) {
        laser = &enemyLasers[i];
        if (laser->active && doOverlap(
            s->x, s->y, pixelSizes[s->width], pixelSizes[s->height],
            laser->x, laser->y, pixelSizes[laser->width], pixelSizes[laser->height])) {
                return laser;
            }
    }

    return 0;
}

void resetLaser(Sprite *laser) {
    laser->active = 0;
    laser->zDepth = Disabled;
    spriteMove(laser, 0, 0);
    x16SpriteIdxSetZDepth(laser->index, laser->zDepth);
    x16SpriteIdxSetXY(laser->index, laser->x, laser->y);
}

unsigned char enemiesCreate(LevelOveralLayout *level, unsigned char nextSpriteIndex, unsigned long mainFrameCount) {
    unsigned char i;

    // Load the sprite data
    spriteEnemyDataLoad(level->enemyList->enemies, level->enemyList->length);

    for (i=0; i<level->enemyList->length; i++) {
        // After dying, an enemy will respawn after some time.
        if (enemyAlive(mainFrameCount, level->levelNum, i)) {
            enemyCreate(
                level->enemyList->enemies[i].enemyType,
                &masterEnemiesList[i],
                &level->enemyList->enemies[i],
                nextSpriteIndex+i);
        } else {
            masterEnemiesList[i].health = 1;
            masterEnemiesList[i].sprite.active = 0;
        }
    }

    return level->enemyList->length;
}

/**
 * Any dead enemies will have the framecount cached so we know when to respawn them
*/
void levelCacheUpdate(LevelOveralLayout *level, unsigned long mainFrameCount) {
    unsigned char i;

    for (i=0; i<level->enemyList->length; i++) {
        if (masterEnemiesList[i].health == 0) {
           enemyFrameSet(mainFrameCount, level->levelNum, i);
        }
    }

    for (i=0; i<level->entityList->length; i++) {
        entityTypeSet(level->entityList->entities[i].entityType, level->levelNum, i);
    }
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
    for (i=0; i<ENEMY_LASER_COUNT; i++) {
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

    for (i=0; i<ENEMY_LASER_COUNT; i++) {
        laser = &enemyLasers[i];

        if (!laser->active) {
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

void enemiesMove(LevelOveralLayout *level, Sprite *player, unsigned char length) {
    unsigned char i,r;
    signed char tileCalc;
    AISprite *enemy;
    TileInfo tileCollision;

    // Move enemies
    for (i=0; i<length; i++) {
        enemy = &masterEnemiesList[i];
        if (enemy->sprite.active) {
            // See if the enemy jumps at all
            if (enemy->framesBetweenJumps > 0) {
                // See if enemy needs to jump
                if (enemy->framesUntilNextJump == 0) {
                    spriteTouchingTile(level, &enemy->sprite, &tileCollision);
                    if (tileCollision.type == Water) {
                        enemy->jumpFrames = ENEMY_WATER_JUMP_FRAMES;
                        // In water, enemies can "jump" up or swim down
                        r = rand();
                        enemy->jumpDir = r > 127 ? 1 : -1;
                        enemy->jumpSpeed = ENEMY_WATER_JUMP_SPEED_NORMAL * enemy->jumpDir;
                    } else {
                        enemy->jumpFrames = ENEMY_JUMP_FRAMES;
                        enemy->jumpDir = 1;
                        enemy->jumpSpeed = ENEMY_JUMP_SPEED_NORMAL;
                    }
                    enemy->framesUntilNextJump = 9999;
                }

                // See if enemy is currently jumping
                if (enemy->jumpFrames > 0) {
                    spriteMoveYL(&enemy->sprite, enemy->sprite.yL - enemy->jumpSpeed);
                    enemy->jumpFrames--;
                } else if ((enemy->jumpDir == 1 && enemy->sprite.y < enemy->yTileStart * TILE_PIXEL_HEIGHT) ||
                           (enemy->jumpDir == -1 && enemy->sprite.y > enemy->yTileStart * TILE_PIXEL_HEIGHT)) {
                    // Falling back down
                    spriteMoveYL(&enemy->sprite, enemy->sprite.yL + enemy->jumpSpeed);

                    // If back on the ground, reset the jump counters
                    if ((enemy->jumpDir == 1 && enemy->sprite.y >= enemy->yTileStart * TILE_PIXEL_HEIGHT) ||
                        (enemy->jumpDir == -1 && enemy->sprite.y <= enemy->yTileStart * TILE_PIXEL_HEIGHT)) {
                        enemy->framesUntilNextJump = enemy->framesBetweenJumps;
                        spriteMoveY(&enemy->sprite, enemy->yTileStart * TILE_PIXEL_HEIGHT);
                    }
                }

                enemy->framesUntilNextJump--;
            }

            // If patrolling an area, move
            if (enemy->xTileStart != enemy->xTileEnd) {
                spriteAnimationAdvance(&enemy->sprite);
                spriteMoveXL(&enemy->sprite, enemy->sprite.animationDirection == 0 ? enemy->sprite.xL-enemy->sprite.speed : enemy->sprite.xL+enemy->sprite.speed);    
                tileCalc = enemy->sprite.x / TILE_PIXEL_WIDTH;
                // Careful, can be -1 if on left edge (signed char)
                if (tileCalc <= (signed char)enemy->xTileStart - 1) {
                    enemy->sprite.animationDirection = 1;
                    x16SpriteIdxSetHFlip(enemy->sprite.index, enemy->sprite.animationDirection);
                } else if (tileCalc >= enemy->xTileEnd) {
                    enemy->sprite.animationDirection = 0;
                    x16SpriteIdxSetHFlip(enemy->sprite.index, enemy->sprite.animationDirection);
                }
            } else {
                // Enemy is standing still
                // See if we should still animate
                if (enemy->sprite.animateIfStill) {
                    spriteAnimationAdvance(&enemy->sprite);
                }
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

    for (i=0; i<ENEMY_LASER_COUNT; i++) {
        laser = &enemyLasers[i];
        if (laser->active) {
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

#pragma code-name (push, "BANKRAM01")

unsigned char enemyLasersCreate(unsigned char nextSpriteIndex) {
    unsigned char i;

    for (i=0; i<ENEMY_LASER_COUNT; i++) {
        laserCreate(&enemyLasers[i], nextSpriteIndex++);
    }

    return nextSpriteIndex;
}

#pragma code-name (pop)