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
#include "level.h"
#include "welcome.h"

// The "waitvsync" function is broken in r41
// People say to use this until fixed
#include "waitforjiffy.h"

#define PLAYER_FALL_SPEED 2
#define PLAYER_JUMP_SPEED 2
#define PLAYER_JUMP_FRAMES 14

// Import the levels
extern LevelOveralLayout testLevel;

// TODO: Fixed array to hold AISprites
// Need something more dynamic but this works for now
AISprite masterEnemiesList[16];

void layerMapsAddSomeStuff() {
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

    // Set to all empty tiles in Layer 1 (foreground)
    vMemSetBank(LAYER1_MAP_MEM_BANK);
    vMemSetAddr(LAYER1_MAP_MEM);
    for (y=0; y<32; y++) {
        for (x=0; x<64; x++) {     
            vMemSetData0(0);
            vMemSetData0(0);
        }
    }

    for (x=0; x < testLevel.tilesListLength ; x++) {
        addLevelTiles(testLevel.tilesList[x].length, testLevel.tilesList[x].tiles);
    }
}

void spriteTouchingTile(Sprite *sprite, SolidLayout *collisionTile) {
    unsigned char i,j;
    SolidLayout standingTile;
    
    // Special signal that all is clear
    collisionTile->type = 255;

    standingTile.x = ((sprite->x + TILE_PIXEL_WIDTH_HALF) / TILE_PIXEL_WIDTH);
    standingTile.y = (sprite->y + pixelSizes[sprite->height]) / TILE_PIXEL_HEIGHT;

    for (i=0; i<testLevel.solidListLength; i++) {
        for (j=0; j<testLevel.solidList[i].length; j++) {
            if (standingTile.y == testLevel.solidList[i].solid[j].y &&
                standingTile.x >= testLevel.solidList[i].solid[j].x &&
                standingTile.x <= testLevel.solidList[i].solid[j].x + (testLevel.solidList[i].solid[j].length-1)) {
                    *collisionTile = testLevel.solidList[i].solid[j];
                    return;
                }
        }
    }

    return;
}

unsigned char enemiesCreate(AISprite enemies[], unsigned char nextSpriteIndex) {
    unsigned char i, j;
    unsigned char length = 0;
    SolidLayout si;
    void (*enemyCreate[])(AISprite*, EnemyLayout*, unsigned char) = {
        snakeCreate, beeCreate, ghostCreate, scorpionCreate, waspCreate
    };

    for (i=0; i<testLevel.enemiesListLength; i++) {
        for (j=0; j<testLevel.enemiesList[i].length; j++) {
            (*enemyCreate[testLevel.enemiesList[i].enemies[j].enemyType])(
                &enemies[length], &testLevel.enemiesList[i].enemies[j], nextSpriteIndex+length
            );

            // Get the current ground the AI is standing on and patrol it
            spriteTouchingTile(&enemies[length].sprite, &si);
            if (si.type !=255) {
                enemies[length].xTileStart = si.x;
                enemies[length].yTileStart = si.y;
                enemies[length].xTileEnd = si.x + si.length;
                enemies[length].yTileEnd = si.y + si.length;
            }
            length++;
        }
    }

    return length;
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

void main() {
    unsigned char collision, joy, enemyCount;
    unsigned char nextSpriteIndex = 0;
    unsigned char jumpFrames = 0;
    
    SolidLayout tileCollision;
    Sprite player, bullet;

    welcomeStart();

    tilesConfig();
    welcomeTiles();

    layerMapsAddSomeStuff();
    welcomeMaps();

    spriteDataLoad();
    spriteIRQConfig();
    welcomeSprites();
    welcomeEnd();

    // Wait to switch to game mode until everything is loaded
    // If you switch video modes first, you get crazy stuff on screen (kind cool?)
    videoConfig();

    // Create the sprites
    playerCreate(&player, testLevel.playerLayout, nextSpriteIndex++);
    enemyCount = enemiesCreate(masterEnemiesList, nextSpriteIndex);
    nextSpriteIndex+= enemyCount;
    bulletCreate(&bullet, nextSpriteIndex++);
    
    // Configure the joysticks
    joy_install(cx16_std_joy);

    while (1) {
        waitforjiffy();

        enemiesMove(masterEnemiesList, enemyCount);

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
            spriteTouchingTile(&player, &tileCollision);
            if (tileCollision.type != 255) {
                spriteMoveY(&player, ((tileCollision.y * TILE_PIXEL_HEIGHT) - pixelSizes[player.height]) - 1);
                // Player is on solid ground, can jump
                if (JOY_BTN_1(joy) || JOY_UP(joy)) {
                    jumpFrames = PLAYER_JUMP_FRAMES;
                    // The jump animation wasn't great. The normal animation is fine
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
            // No animation change
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

        // See if the player is touching any tiles
        // Just move back for now
        spriteTouchingTile(&player, &tileCollision);
        if (tileCollision.type != 255) {
            spriteMoveBackX(&player);
        }

        x16SpriteIdxSetXY(player.index, player.x, player.y);

        // Get the Collision bits and shift them down
        collision = x16SpriteCollisionBitsGet();

        // The collision bits will be the OVERLAP of the Collision Masks of the two sprites.
        // We can then look at all the sprites that have that bit in their mask

        // Player and Enemy collisions
        if (collision == 0b1001) {
            // Move the sprite back
            spriteMove(&player, 350, 235);
            // x16SpriteIdxSetXY(player.index, player.x, player.y);
        } else if (collision == 0b1010) {
            // Bullet hit the snake
            bullet.active = 0;
            bullet.zDepth = Disabled;
            x16SpriteIdxSetZDepth(bullet.index, bullet.zDepth);
        }

        // Bullet is off screen or collided with a solid tile
        if (bullet.active == 1) {
            spriteTouchingTile(&bullet, &tileCollision);
            if (tileCollision.type != 255 || bullet.x < 0 || bullet.x > 639) {
                bullet.active = 0;
                bullet.zDepth = Disabled;
                x16SpriteIdxSetZDepth(bullet.index, bullet.zDepth);
            }
        }
    }

    // Disable sprite collisions before quitting
    // or the UI hangs if sprites are still touching.
    x16SpriteCollisionsDisable();
}