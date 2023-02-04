#include <cx16.h>
#include <joystick.h>
#include <stdio.h>
// Libs
#include "x16graphics.h"
#include "sprites.h"

// Game specific
#include "startup.h"
#include "gametiles.h"
#include "gamesprites.h"
#include "memmap.h"
#include "level.h"
#include "welcome.h"
#include "gameoverall.h"

// The "waitvsync" function is broken in r41
// People say to use this until fixed
#include "waitforjiffy.h"

#define PLAYER_FALL_SPEED 2
#define PLAYER_JUMP_SPEED 2
#define PLAYER_JUMP_FRAMES 14
#define NO_TILE_COLLISION 255

// Import the levels
// extern GameLayout gameLayout;
// extern LevelOveralLayout level1;

// TODO: Fixed array to hold AISprites
// Need something more dynamic but this works for now
AISprite masterEnemiesList[16];
Sprite player, bullet, expSmall;
LevelOveralLayout* level;

void layerMapsAddSomeStuff(LevelOveralLayout *level) {
    unsigned short x, y;
    
    vMemSetIncMode(1);

    // Clear Layer 0 (background)
    vMemSetBank(LAYER0_MAP_MEM_BANK);
    vMemSetAddr(LAYER0_MAP_MEM);
    for (y=0; y<32; y++) {
        for (x=0; x<64; x++) {
            vMemSetData0(0);
            vMemSetData0(0);
        }
    }

    // Clear Layer 1 (foreground)
    vMemSetBank(LAYER1_MAP_MEM_BANK);
    vMemSetAddr(LAYER1_MAP_MEM);
    for (y=0; y<32; y++) {
        for (x=0; x<64; x++) {     
            vMemSetData0(0);
            vMemSetData0(0);
        }
    }

    addLevelTiles(level->tilesList->length, level->tilesList->tiles);
}

void playerTouchingExit(LevelOveralLayout *level, Sprite *sprite, Exit *exitCollision) {
    unsigned char i;
    Exit exitStanding;

    // Special signal that all is clear
    exitCollision->level = NO_TILE_COLLISION;

    exitStanding.x = ((sprite->x + TILE_PIXEL_WIDTH_HALF) / TILE_PIXEL_WIDTH);
    exitStanding.y = ((sprite->y + TILE_PIXEL_HEIGHT_HALF) / TILE_PIXEL_HEIGHT);

    for (i=0; i<level->exitList->length; i++) {
        if (exitStanding.y == level->exitList->exits[i].y &&
            exitStanding.x == level->exitList->exits[i].x) {
                *exitCollision = level->exitList->exits[i];
                return;
            }
    }
}

void spriteTouchingTile(LevelOveralLayout *level, Sprite *sprite, SolidLayout *tileCollision) {
    unsigned char i;
    SolidLayout tileStanding;
    
    // Special signal that all is clear
    tileCollision->type = NO_TILE_COLLISION;

    tileStanding.x = ((sprite->x + TILE_PIXEL_WIDTH_HALF) / TILE_PIXEL_WIDTH);
    tileStanding.y = (sprite->y + pixelSizes[sprite->height]) / TILE_PIXEL_HEIGHT;

    for (i=0; i<level->solidList->length; i++) {
        if (tileStanding.y == level->solidList->solid[i].y &&
            tileStanding.x >= level->solidList->solid[i].x &&
            tileStanding.x <= level->solidList->solid[i].x + (level->solidList->solid[i].length-1)) {
                *tileCollision = level->solidList->solid[i];
                return;
            }
    }

    return;
}

unsigned char enemiesCreate(LevelOveralLayout *level, AISprite enemies[], unsigned char nextSpriteIndex) {
    unsigned char i;
    unsigned char length = 0;
    SolidLayout si;
    void (*enemyCreate[])(AISprite*, EnemyLayout*, unsigned char) = {
        snakeCreate, beeCreate, ghostCreate, scorpionCreate, waspCreate
    };

    for (i=0; i<level->enemiesList->length; i++) {
        (*enemyCreate[level->enemiesList->enemies[i].enemyType])(
            &enemies[length], &level->enemiesList->enemies[i], nextSpriteIndex+length
        );

        // Get the current ground tile the AI is standing on and patrol its entire length
        spriteTouchingTile(level, &enemies[length].sprite, &si);
        if (si.type != NO_TILE_COLLISION) {
            enemies[length].xTileStart = si.x;
            enemies[length].yTileStart = si.y;
            enemies[length].xTileEnd = si.x + si.length;
            enemies[length].yTileEnd = si.y + si.length;
        }
        length++;
    }

    return length;
}

void enemiesReset(AISprite enemies[], unsigned char length) {
    unsigned char i;
    AISprite *enemy;

    // Reset enemies
    for (i=0; i<length; i++) {
        enemy = &enemies[i];
        enemy->sprite.active = 0;
        enemy->sprite.zDepth = Disabled;
        x16SpriteIdxSetZDepth(enemy->sprite.index, Disabled);
    }
}

void enemiesMove(AISprite enemies[], unsigned char length) {
    unsigned char i;
    signed char tileCalc;
    AISprite *enemy;

    // Move enemies
    for (i=0; i<length; i++) {
        enemy = &enemies[i];
        spriteMoveXL(&enemy->sprite, enemy->sprite.animationDirection == 0 ? enemy->sprite.xL-enemy->sprite.speed : enemy->sprite.xL+enemy->sprite.speed);
        enemy->sprite.animationCount++;
        if (enemy->sprite.animationCount == enemy->sprite.animationSpeed) {
            enemy->sprite.animationCount=0;
            enemy->sprite.animationFrame++;
            if (enemy->sprite.animationFrame == enemy->sprite.frames) {
                enemy->sprite.animationFrame = 0;
            }
            x16SpriteIdxSetGraphicsPointer(enemy->sprite.index, enemy->sprite.clrMode, enemy->sprite.graphicsBank,
                enemy->sprite.graphicsAddress+(enemy->sprite.animationFrame * enemy->sprite.frameSize));
        }
        tileCalc = enemy->sprite.x / TILE_PIXEL_WIDTH;
        // Careful, can be -1 if on left edge (signed char)
        if (tileCalc <= (signed char)enemy->xTileStart - 1) {
            enemy->sprite.animationDirection = 1;
            x16SpriteIdxSetHFlip(enemy->sprite.index, enemy->sprite.animationDirection);
        } else if (tileCalc >= enemy->xTileEnd - 1) {
            enemy->sprite.animationDirection = 0;
            x16SpriteIdxSetHFlip(enemy->sprite.index, enemy->sprite.animationDirection);
        }
        x16SpriteIdxSetXY(enemy->sprite.index, enemy->sprite.x, enemy->sprite.y);
    }
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

Exit* runLevel(unsigned char nextSpriteIndex) {
    unsigned char collision, joy, enemyCount;
    unsigned char jumpFrames = 0;
    unsigned char releasedBtnAfterJump = 1;
    
    SolidLayout tileCollision;
    Exit exitCollision;
    
    enemyCount = enemiesCreate(level, masterEnemiesList, nextSpriteIndex);
    nextSpriteIndex+= enemyCount;

    while (1) {
        waitforjiffy();

        // See if player is touching an exit
        playerTouchingExit(level, &player, &exitCollision);
        if (exitCollision.level != NO_TILE_COLLISION) {
            // Clean up the enemies and return the exit info
            enemiesReset(masterEnemiesList, enemyCount);
            return &exitCollision;
        }

        enemiesMove(masterEnemiesList, enemyCount);

        // Count game loops so we can animate sprites.
        // Only animate if the guy is "going" somewhere.
        player.going=0;
        player.animationCount++;

        // We are changing the guys animation every 6 game loops
        // but hold at this count as we only animate if moving.
        if (player.animationCount > player.animationSpeed) {
            player.animationCount=player.animationSpeed;
        }

        joy = joy_read(0);

        // Falling
        // jumpFrames are the number of frames the player moves UP (jumps)
        // At 0, the player is always falling, even if on the ground.
        if (jumpFrames == 0) {
            // Inefficient: We are checking here AND after moving X
            // Might be ok and makes it easier to deal with moving him back
            spriteMoveY(&player, player.y+PLAYER_FALL_SPEED);
            spriteTouchingTile(level, &player, &tileCollision);

            // If the player is standing on a tile, a few things happen
            if (tileCollision.type != NO_TILE_COLLISION) {
                // Move the player to the top of the tile
                // We do this because as the player falls they may end up wedged a few pixels into a tile
                spriteMoveY(&player, ((tileCollision.y * TILE_PIXEL_HEIGHT) - pixelSizes[player.height]) - 1);
                // Player is on solid ground, see if they pressed jump button
                if (JOY_BTN_1(joy) || JOY_UP(joy)) {
                    // Only let them jump if they released the jump button since the last jump.
                    // Without this, the player just hops as you hold the button.
                    if (releasedBtnAfterJump == 1) {
                        jumpFrames = PLAYER_JUMP_FRAMES;
                        releasedBtnAfterJump = 0;
                        // The jump animation wasn't great. The normal animation is fine
                    }
                } else {
                    // Player is on ground and isn't holding the button
                    // They can jump again next game loop.
                    releasedBtnAfterJump = 1;
                }
            }
        } else {
            // Player is jumping, move them up
            jumpFrames--;
            player.going==1;
            spriteMoveY(&player, player.y-PLAYER_JUMP_SPEED);
        }

        // Check if player is moving left/right
        if (JOY_LEFT(joy)) {
            player.going=1;
            // We also flip the animation depending on direction
            if (player.animationDirection != 0) {
                player.animationDirection=0;
                x16SpriteIdxSetHFlip(player.index, player.animationDirection);
            }
            spriteMoveXL(&player, player.xL-player.speed);
        } else if (JOY_RIGHT(joy)) {
            player.going=1;
            // Maybe flip animation
            if (player.animationDirection != 1) {
                player.animationDirection=1;
                x16SpriteIdxSetHFlip(player.index, player.animationDirection);
            }
            spriteMoveXL(&player, player.xL+player.speed);
        }

        // Change animation if jumping or moving and hit loop count
        if (jumpFrames > 0) {
            // No animation change because it looked dumb
        } else if (player.going==1 && player.animationCount == player.animationSpeed) {
            player.animationCount=0;
            player.animationFrame++;
            if (player.animationFrame == player.frames) {
                player.animationFrame = 0;
            }
            x16SpriteIdxSetGraphicsPointer(player.index, player.clrMode, player.graphicsBank,
                player.graphicsAddress+(player.animationFrame * player.frameSize));
        } else if (player.animationCount == player.animationSpeed && player.animationFrame != player.animationStopFrame) {
            // If the guy is standing still, always show a certain frame
            // In future this could be a totally different animation
            // We have a yawn animation for instance for when waiting too long.
            player.animationFrame = player.animationStopFrame;
            x16SpriteIdxSetGraphicsPointer(player.index, player.clrMode, player.graphicsBank,
                player.graphicsAddress+(player.animationFrame * player.frameSize));
        }
        
        // Move the bullet
        if (bullet.active == 1) {
            spriteMoveX(&bullet, bullet.animationDirection == 0 ? bullet.x-bullet.speed : bullet.x+bullet.speed);
            x16SpriteIdxSetXY(bullet.index, bullet.x, bullet.y);
        }

        // See if player is shooting
        if (JOY_BTN_2(joy) && bullet.active==0) {
            bullet.active = 1;
            bullet.animationDirection = player.animationDirection;
            spriteMove(&bullet, player.x, player.y);
            // Show the bullet
            bullet.zDepth = BetweenL0L1;
            x16SpriteIdxSetZDepth(bullet.index, bullet.zDepth);
            x16SpriteIdxSetXY(bullet.index, bullet.x, bullet.y);
            x16SpriteIdxSetHFlip(bullet.index, bullet.animationDirection);
        }

        // See if the player is touching any tiles left/right
        // Move back if so.
        spriteTouchingTile(level, &player, &tileCollision);
        if (tileCollision.type != 255) {
            spriteMoveBackX(&player);
        }

        // Final position of the player. Update their sprite
        x16SpriteIdxSetXY(player.index, player.x, player.y);

        // Manage explosion sprites
        expSmall.animationCount++;
        if (expSmall.animationCount == expSmall.animationSpeed) {
            expSmall.animationCount=0;
            expSmall.animationFrame++;
            x16SpriteIdxSetGraphicsPointer(expSmall.index, expSmall.clrMode, expSmall.graphicsBank,
                expSmall.graphicsAddress+(expSmall.animationFrame * expSmall.frameSize));

            if (expSmall.animationFrame == expSmall.frames) {
                expSmall.zDepth = Disabled;
                x16SpriteIdxSetZDepth(expSmall.index, expSmall.zDepth);
            }
        }
            
        // Get the Collision bits and shift them down
        collision = x16SpriteCollisionBitsGet();

        // The collision bits will be the OVERLAP of the Collision Masks of the two sprites.
        // We can then look at all the sprites that have that bit in their mask

        // Player and Enemy collisions
        if (collision == 0b1001) {
            // Move the sprite back to the start
            spriteMove(&player, level->entranceList->entrances[0].x, level->entranceList->entrances[0].y);
            x16SpriteIdxSetXY(player.index, player.x, player.y);
        } else if (collision == 0b1010) {
            // Explosion
            smallExplosion(&expSmall, BetweenL0L1, bullet.x, bullet.y);
            
            // Bullet hit an enemy
            bullet.active = 0;
            bullet.zDepth = Disabled;
            x16SpriteIdxSetZDepth(bullet.index, bullet.zDepth);
        }

        // Bullet is off screen or collided with a solid tile
        if (bullet.active == 1) {
            spriteTouchingTile(level, &bullet, &tileCollision);
            if (tileCollision.type != 255 || bullet.x < 0 || bullet.x > 639) {
                if (tileCollision.type != 255) {
                    // Explosion
                    smallExplosion(&expSmall, InFrontOfL1, bullet.x, bullet.y);
                }
                bullet.active = 0;
                bullet.zDepth = Disabled;
                x16SpriteIdxSetZDepth(bullet.index, bullet.zDepth);
            }
        }
    }
}

void main() {
    unsigned char nextSpriteIndex = 0;
    Exit *exit;
    level = levelGet(1);

    // Configure the joysticks
    joy_install(cx16_std_joy);
    
    showTitleScreen();
    
    tilesConfig();
    spriteDataLoad();
    spriteIRQConfig();

    // Wait to switch to game mode until everything is loaded
    // If you switch video modes first, you get crazy stuff on screen (kind cool?)
    videoConfig();

    // Create the sprites
    playerCreate(&player, &level->entranceList->entrances[0], nextSpriteIndex++);
    bulletCreate(&bullet, nextSpriteIndex++);
    explosionSmallCreate(&expSmall, nextSpriteIndex++);

    while(1) {
        layerMapsAddSomeStuff(level);
        exit = runLevel(nextSpriteIndex);
        level = levelGet(exit->level);
        spriteMove(&player, level->entranceList->entrances[0].x * TILE_PIXEL_WIDTH, level->entranceList->entrances[0].y * TILE_PIXEL_WIDTH);
        x16SpriteIdxSetXY(player.index, player.x, player.y);
    }

    // Disable sprite collisions before quitting
    // or the UI hangs if sprites are still touching.
    x16SpriteCollisionsDisable();
}