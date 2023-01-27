#include <peekpoke.h>
#include <cx16.h>
#include <joystick.h>

// Libs
#include "x16graphics.h"
#include "sprites.h"

// Game specific
#include "gametiles.h"
#include "gamesprites.h"
#include "memmap.h"
#include "gamelevels.h"

// The "waitvsync" function is broken in r41
// People say to use this until fixed
#include "waitforjiffy.h"

// Import the levels
extern TileLayout testLevelTiles[];
extern LevelLayout testLevelSolid[];
extern TileLayout cloudTiles1[];

void videoConfig() {
    // clear the screen to start
    // This happens when we draw the screen anyway. If we need again...
    // #include <conio.h>
    // clrscr();

    // We are using 640x480 with 256clr, 16x16 Tiles
    // Each tile is 256 bytes, but we won't need THAT many so this feels ok

    // For the layers, we will use both layers with the sprites inbetween them (L0 in back).
    // Layer Maps will be in mode:
    //  Height: 00 (gives us 32 Tiles which is more than the 30 we need: 480/16=30)
    //  Width:  01 (gives us 64 Tiles which is more than the 40 we need: 640/16=40)
    // Each Map will be 4096 bytes

    // Enable both layers (leave other settings the same)
    POKE(VMEM_VIDEO, PEEK(VMEM_VIDEO) | 0b00110000);

    // Config both layers - H/W, 256clr, 11 color depth
    POKE(LAYER_0_CONFIG, 0b00011011);
    POKE(LAYER_1_CONFIG, 0b00011011);

    // Mapbase for both layers - Bank 0
    POKE(LAYER_0_MAPBASE, memoryToMapMemoryAddress(LAYER0_MAP_MEM_BANK, LAYER0_MAP_MEM));
    POKE(LAYER_1_MAPBASE, memoryToMapMemoryAddress(LAYER1_MAP_MEM_BANK, LAYER1_MAP_MEM));

    // Tilebase - Put after the Mapbases for both layers
    POKE(LAYER_0_TILEBASE, tileBaseConfig(TILE_MEM_BANK, TILE_MEM, 1, 1));
    POKE(LAYER_1_TILEBASE, tileBaseConfig(TILE_MEM_BANK, TILE_MEM, 1, 1));
}

void addLevelTiles(unsigned char length, TileLayout layout[]) {
    unsigned short layerMem;
    unsigned char i, l, t;

    vMemSetBank(LAYER1_MAP_MEM_BANK);
    for (i=0,t=2; i<length; i++) {
        layerMem = layout[i].layer == 0 ? LAYER0_MAP_MEM : LAYER1_MAP_MEM;
        vMemSetAddr(layerMem+(layout[i].y * TILES_ACROSS * 2)+(layout[i].x * 2));
        for (l=0; l<layout[i].length; l++) {
            // This special type repeats floor tiles with tiles for each end
            if (layout[i].type == 255) {
                if (l==0) {
                    vMemSetData0(1);
                } else if (l==layout[i].length-1) {
                    vMemSetData0(5);
                } else {
                    vMemSetData0(t);
                    t++;
                    if (t==5) {
                        t=2;
                    }
                }
            } else {
                vMemSetData0(layout[i].type);
            }
            vMemSetData0(0);
        }
    }
}

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
    unsigned char collision;
    unsigned char joy;
    LevelLayout tileCollision;
    Sprite player, badguy, bullet;
    
    videoConfig();
    tilesConfig();
    layerMapsAddSomeStuff();
    spriteDataLoad();
    spriteIRQConfig();

    // Create the sprites
    playerCreate(&player);
    badguyCreate(&badguy);
    bulletCreate(&bullet);
    
    // Configure the joysticks
    joy_install(cx16_std_joy);

    while (1) {
        waitforjiffy();

        // Count game loops so we can animate sprites.
        // Only animate if the guy is moving.
        player.going=0;
        player.animationCount++;

        // We are changing the guys animation every 6 game loops
        // but hold at this count as we only animate if moving.
        if (player.animationCount > 6) {
            player.animationCount=6;
        }

        joy = joy_read(0);
        // No moving up/down now
        // Player just falls
        // if (JOY_UP(joy)) {
        //     spriteMoveY(&player, player.y-player.speed);
        //     player.going=1;
        // } else if (JOY_DOWN(joy)) {
        //     player.going=1;
        //     spriteMoveY(&player, player.y+player.speed);
        // }

        // Falling
        // Inefficient: We are checking here AND after moving X
        // Might be ok and makes it easier to deal with moving him back
        spriteMoveY(&player, player.y+3);
        playerTouchingTile(&player, &tileCollision);
        if (tileCollision.type != 255) {
            // spriteMoveBackY(&player);
            spriteMoveY(&player, ((tileCollision.y * TILE_PIXEL_HEIGHT) - pixelSizes[player.height]) - 1);
        }

        if (JOY_LEFT(joy)) {
            player.going=1;
            // We also flip the animation depending on direction
            if (player.animationDirection != 1) {
                player.animationDirection=1;
                x16SpriteIdxSetHFlip(player.spriteBank, player.index, player.animationDirection);
            }
            spriteMoveX(&player, player.x-player.speed);
        } else if (JOY_RIGHT(joy)) {
            player.going=1;
            // Maybe flip animation
            if (player.animationDirection != 0) {
                player.animationDirection=0;
                x16SpriteIdxSetHFlip(player.spriteBank, player.index, player.animationDirection);
            }
            spriteMoveX(&player, player.x+player.speed);
        }

        // Change animation if moving and hit loop count
        if (player.going==1 && player.animationCount == player.animationSpeed) {
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
        
        // Quit
        if (JOY_BTN_1(joy)) {
            break;
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
            // Bullet hit the badguy
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