#include <cx16.h>
#include <joystick.h>

// Libs
#include "x16graphics.h"
#include "sprites.h"

// Game specific
#include "startup.h"
#include "gametiles.h"
#include "gamesprites.h"
#include "memmap.h"
#include "gamelevels.h"
#include "level.h"

// The "waitvsync" function is broken in r41
// People say to use this until fixed
#include "waitforjiffy.h"

#define PLAYER_FALL_SPEED 2
#define PLAYER_JUMP_SPEED 2
#define PLAYER_JUMP_FRAMES 14

// Import the levels
extern TileLayout testLevelTiles[];
extern LevelLayout testLevelSolid[];
extern TileLayout cloudTiles1[];

void layerMapsAddSomeStuff() {
    unsigned short x, y;
    
    vMemSetIncMode(1);

    // Clear Layer 0 (background) but add some clouds and other tiles
    vMemSetBank(LAYER0_MAP_MEM_BANK);
    vMemSetAddr(LAYER0_MAP_MEM);
    for (y=0; y<32; y++) {
        for (x=0; x<64; x++) {
            vMemSetData0(0);
            vMemSetData0(0);
        }
    }

    addLevelTiles(CLOUD_TILES_1_LENGTH, cloudTiles1);

    // Set to all empty tiles in Layer 1 (foreground)
    vMemSetBank(LAYER1_MAP_MEM_BANK);
    vMemSetAddr(LAYER1_MAP_MEM);
    for (y=0; y<32; y++) {
        for (x=0; x<64; x++) {     
            vMemSetData0(0);
            vMemSetData0(0);
        }
    }

    addLevelTiles(TEST_LEVEL_TILES_LENGTH, testLevelTiles);
}

void playerTouchingTile(Sprite *player, LevelLayout *collisionTile) {
    unsigned char i;
    LevelLayout leftTile;
    LevelLayout rightTile;
    
    // Special signal that all is clear
    collisionTile->type = 255;

    leftTile.x = player->x / TILE_PIXEL_WIDTH;
    leftTile.y = (player->y + pixelSizes[player->height]) / TILE_PIXEL_HEIGHT;

    rightTile.x = (player->x + pixelSizes[player->width]) / TILE_PIXEL_WIDTH;
    rightTile.y = leftTile.y;

    for (i=0; i<TEST_LEVEL_SOLID_LENGTH; i++) {
        if (leftTile.y == testLevelSolid[i].y &&
            leftTile.x >= testLevelSolid[i].x &&
            leftTile.x <= testLevelSolid[i].x + (testLevelSolid[i].length-1)) {
                *collisionTile = leftTile;
                return;
            }

        if (rightTile.y == testLevelSolid[i].y &&
            rightTile.x >= testLevelSolid[i].x &&
            rightTile.x <= testLevelSolid[i].x + (testLevelSolid[i].length-1)) {
                *collisionTile = rightTile;
                return;
            }
    }

    return;
}

void main() {
    unsigned char collision, joy, e;
    unsigned char nextSpriteIndex = 0;
    unsigned char jumpFrames = 0;
    signed char tileCalc;
    
    LevelLayout tileCollision;
    Sprite player, bullet;
    AISprite snakes[3];
    AISprite *enemy;

    videoConfig();
    tilesConfig();
    layerMapsAddSomeStuff();
    spriteDataLoad();
    spriteIRQConfig();

    // Create the sprites
    playerCreate(&player, nextSpriteIndex++);
    snakeCreate(&snakes[0], &testLevelSolid[2], nextSpriteIndex++);
    snakeCreate(&snakes[1], &testLevelSolid[3], nextSpriteIndex++);
    snakeCreate(&snakes[2], &testLevelSolid[4], nextSpriteIndex++);
    
    bulletCreate(&bullet, nextSpriteIndex++);
    
    // Configure the joysticks
    joy_install(cx16_std_joy);

    while (1) {
        waitforjiffy();

        // Move enemies
        for (e=0; e < 3; e++) {
            enemy = &snakes[e];
            spriteMoveXL(&enemy->sprite, enemy->sprite.animationDirection == 0 ? enemy->sprite.xL-enemy->sprite.speed : enemy->sprite.xL+enemy->sprite.speed);
            enemy->sprite.animationCount++;
            if (enemy->sprite.animationCount == enemy->sprite.animationSpeed) {
                enemy->sprite.animationCount=0;
                enemy->sprite.animationFrame++;
                if (enemy->sprite.animationFrame == enemy->sprite.frames) {
                    enemy->sprite.animationFrame = 0;
                }
                x16SpriteIdxSetGraphicsPointer(enemy->sprite.spriteBank, enemy->sprite.index, enemy->sprite.clrMode, enemy->sprite.graphicsBank,
                    enemy->sprite.graphicsAddress+(enemy->sprite.animationFrame * enemy->sprite.frameSize));
            }
            tileCalc = enemy->sprite.x / TILE_PIXEL_WIDTH;
            // Careful, can be -1 if on left edge (signed char)
            if (tileCalc <= (signed char)enemy->xTileStart - 1) {
                enemy->sprite.animationDirection = 1;
                x16SpriteIdxSetHFlip(enemy->sprite.spriteBank, enemy->sprite.index, enemy->sprite.animationDirection);
            } else if (tileCalc >= enemy->xTileEnd - 1) {
                enemy->sprite.animationDirection = 0;
                x16SpriteIdxSetHFlip(enemy->sprite.spriteBank, enemy->sprite.index, enemy->sprite.animationDirection);
            }
            x16SpriteIdxSetXY(enemy->sprite.spriteBank, enemy->sprite.index, enemy->sprite.x, enemy->sprite.y);
        }        

        // Count game loops so we can animate sprites.
        // Only animate if the guy is moving.
        player.going=0;
        player.animationCount++;

        // We are changing the guys animation every 6 game loops
        // but hold at this count as we only animate if moving.
        if (player.animationCount > player.animationSpeed) {
            player.animationCount=player.animationSpeed;
        }

        joy = joy_read(0);

        // Falling
        if (jumpFrames == 0) {
            // Inefficient: We are checking here AND after moving X
            // Might be ok and makes it easier to deal with moving him back
            spriteMoveY(&player, player.y+PLAYER_FALL_SPEED);
            playerTouchingTile(&player, &tileCollision);
            if (tileCollision.type != 255) {
                spriteMoveY(&player, ((tileCollision.y * TILE_PIXEL_HEIGHT) - pixelSizes[player.height]) - 1);
                // Player is on solid ground, can jump
                if (JOY_BTN_1(joy) || JOY_UP(joy)) {
                    jumpFrames = PLAYER_JUMP_FRAMES;
                    // The jump animation wasn't great. The normal animation is fine
                    // x16SpriteIdxSetGraphicsPointer(player.spriteBank, player.index, player.clrMode, player.graphicsBank,
                    //     player.graphicsAddress+(4 * player.frameSize)); // TODO: Hardcoded jump frame
                }
            }
        } else {
            jumpFrames--;
            player.going==1;
            spriteMoveY(&player, player.y-PLAYER_JUMP_SPEED);
        }

        // TODO: Player sprite images are backwards (right facing) from enemies
        // This means the animationDirection is reversed. Flip the image in the source.
        if (JOY_LEFT(joy)) {
            player.going=1;
            // We also flip the animation depending on direction
            if (player.animationDirection != 1) {
                player.animationDirection=1;
                x16SpriteIdxSetHFlip(player.spriteBank, player.index, player.animationDirection);
            }
            spriteMoveXL(&player, player.xL-player.speed);
        } else if (JOY_RIGHT(joy)) {
            player.going=1;
            // Maybe flip animation
            if (player.animationDirection != 0) {
                player.animationDirection=0;
                x16SpriteIdxSetHFlip(player.spriteBank, player.index, player.animationDirection);
            }
            spriteMoveXL(&player, player.xL+player.speed);
        }

        // Change animation if jumping or moving and hit loop count
        if (jumpFrames > 0) {
            // No animation change
        } else if (player.going==1 && player.animationCount == player.animationSpeed) {
            player.animationCount=0;
            player.animationFrame++;
            if (player.animationFrame == player.frames) {
                player.animationFrame = 0;
            }
            x16SpriteIdxSetGraphicsPointer(player.spriteBank, player.index, player.clrMode, player.graphicsBank,
                player.graphicsAddress+(player.animationFrame * player.frameSize));
        } else if (player.animationCount == player.animationSpeed && player.animationFrame != player.animationStopFrame) {
            // If the guy is standing still, always show a certain frame
            // In future this could be a totally different animation
            // We have a yawn animation for instance for when waiting too long.
            player.animationFrame = player.animationStopFrame;
            x16SpriteIdxSetGraphicsPointer(player.spriteBank, player.index, player.clrMode, player.graphicsBank,
                player.graphicsAddress+(player.animationFrame * player.frameSize));
        }
        
        if (bullet.active == 1) {
            bullet.x-= 4;
            x16SpriteIdxSetXY(bullet.spriteBank, bullet.index, bullet.x, bullet.y);
        }

        if (JOY_BTN_2(joy) && bullet.active==0) {
            bullet.active = 1;
            bullet.x = player.x;
            bullet.y = player.y;
            bullet.zDepth = BetweenL0L1;
            x16SpriteIdxSetZDepth(bullet.spriteBank, bullet.index, bullet.zDepth);
            x16SpriteIdxSetXY(bullet.spriteBank, bullet.index, bullet.x, bullet.y);
        }

        // See if the player is touching any tiles
        // Just move back for now
        playerTouchingTile(&player, &tileCollision);
        if (tileCollision.type != 255) {
            spriteMoveBackX(&player);
        }

        x16SpriteIdxSetXY(player.spriteBank, player.index, player.x, player.y);

        // Get the Collision bits and shift them down
        collision = x16SpriteCollisionBitsGet();

        // The collision bits will be the OVERLAP of the Collision Masks of the two sprites.
        // We can then look at all the sprites that have that bit in their mask

        // Player and Enemy collisions
        if (collision == 0b1001) {
            // Move the sprite back
            player.x = 350;
            player.y = 235;
            x16SpriteIdxSetXY(player.spriteBank, player.index, player.x, player.y);
        } else if (collision == 0b1010) {
            // Bullet hit the snake
            bullet.active = 0;
            bullet.zDepth = Disabled;
            x16SpriteIdxSetZDepth(bullet.spriteBank, bullet.index, bullet.zDepth);
        }

        // Bullet is off screen
        if (bullet.x < 0) {
            bullet.active = 0;
            bullet.zDepth = Disabled;
            x16SpriteIdxSetZDepth(bullet.spriteBank, bullet.index, bullet.zDepth);
        }
    }

    // Disable sprite collisions before quitting
    // or the UI hangs if sprites are still touching.
    x16SpriteCollisionsDisable();
}