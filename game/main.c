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

// The "waitvsync" function is broken in r41
// People say to use this until fixed
#include "waitforjiffy.h"

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

void layerMapsAddSomeStuff() {
    unsigned short x;
    unsigned short y;

    vMemSetIncMode(1);
    
    // Set some tiles in the map for Layer 0 (background)
    // All tiles filled in with tile 1
    vMemSetBank(LAYER0_MAP_MEM_BANK);
    vMemSetAddr(LAYER0_MAP_MEM);
    for (y=0; y<32; y++) {
        for (x=0; x<64; x++) {
            vMemSetData0(1);
            vMemSetData0(0);
        }
    }

    // Set some tiles in the map for Layer 1 (foreground)
    vMemSetBank(LAYER1_MAP_MEM_BANK);
    vMemSetAddr(LAYER1_MAP_MEM);
    for (y=0; y<32; y++) {
        for (x=0; x<64; x++) {
            if (x!=0 && y !=0 & x!=63 && y!=31 && x % 3 == 0 && y % 3 == 0) {
                vMemSetData0(0);
                vMemSetData0(0);
            } else {
                vMemSetData0(2);
                vMemSetData0(0);
            }
        }
    }    
}

void main() {
    unsigned char collision;
    unsigned char joy;

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
        if (JOY_UP(joy)) {
            player.y-=player.speed;
            player.going=1;
        } else if (JOY_DOWN(joy)) {
            player.going=1;
            player.y+=player.speed;
        }

        if (JOY_LEFT(joy)) {
            player.going=1;
            // We also flip the animation depending on direction
            if (player.animationDirection != 1) {
                player.animationDirection=1;
                x16SpriteIdxSetHFlip(player.spriteBank, player.index, player.animationDirection);
            }
            player.x-=player.speed;
        } else if (JOY_RIGHT(joy)) {
            player.going=1;
            // Maybe flip animation
            if (player.animationDirection != 0) {
                player.animationDirection=0;
                x16SpriteIdxSetHFlip(player.spriteBank, player.index, player.animationDirection);
            }
            player.x+=player.speed;
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

        x16SpriteIdxSetXY(player.spriteBank, player.index, player.x, player.y);

        // Get the Collision bits and shift them down
        collision = x16SpriteCollisionBitsGet();

        // The collision bits will be the OVERLAP of the Collision Masks of the two sprites.
        // We can then look at all the sprites that have that bit in their mask

        // Player and Enemy collisions
        if (collision == 0b1001) {
            // Move the sprite back
            // This will cause repeated collisions
            // Want to make sure that works
            player.x = 380;
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