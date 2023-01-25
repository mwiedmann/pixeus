#include <peekpoke.h>
#include <6502.h>
#include <cx16.h>
#include <joystick.h>
#include <conio.h>
#include "x16graphics.h"

// The "waitvsync" function is broken in r41
// People say to use this until fixed
#include "waitforjiffy.h"

// Not sure how big the stack needs to be. Unclear how this works.
#define IRQ_HANDLER_STACK_SIZE 8
unsigned char IRQHandlerStack[IRQ_HANDLER_STACK_SIZE];

#define LAYER0_MAP_MEM 0
#define LAYER1_MAP_MEM 4096
#define TILE_MEM 8192
#define SPRITE_MEM 0x4000

void videoConfig() {
    // clear the screen to start
    clrscr();

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
    POKE(LAYER_0_MAPBASE, memoryToMapMemoryAddress(0, LAYER0_MAP_MEM));
    POKE(LAYER_1_MAPBASE, memoryToMapMemoryAddress(0, LAYER1_MAP_MEM));

    // Tilebase - Put after the Mapbases for both layers
    POKE(LAYER_0_TILEBASE, tileBaseConfig(0, TILE_MEM, 1, 1));
    POKE(LAYER_1_TILEBASE, tileBaseConfig(0, TILE_MEM, 1, 1));
}

void tilesConfig() {
    unsigned short x;
    unsigned short y;

    // Put some stuff into LAYER 1 - Bank 1, TILE: 0xF000, MAP: 0xB000
    vMemSetBank0();
    vMemSetAddr(TILE_MEM);
    vMemSetIncMode(1);

    // Make a square tile
    for (y=0; y < 16; y++) {
        for (x=0; x < 16; x++) {
            //if (x==0 || x==15 || y==0 || y==15) {
                vMemSetData0(59);
            // } else {
            //     vMemSetData0(0);
            // }
        }
    }

    // Make a dashed tile
    for (y=0; y < 16; y++) {
        for (x=0; x < 16; x++) {
            if (y % 2 == 0 && x % 2 == 0) {
                vMemSetData0(115);
            } else {
                vMemSetData0(0);
            }
        }
    }

    // Empty tile
    for (y=0; y < 16; y++) {
        for (x=0; x < 16; x++) {
            vMemSetData0(0);
        }
    }
}

void layerMapsAddSomeStuff() {
    unsigned short x;
    unsigned short y;

    // Set some tiles in the map for Layer 0 (background)
    // All tiles filled in with tile 1
    vMemSetBank0();
    vMemSetAddr(LAYER0_MAP_MEM);
    vMemSetIncMode(1);
    for (y=0; y<32; y++) {
        for (x=0; x<64; x++) {
            vMemSetData0(1);
            vMemSetData0(0);
        }
    }


    // Set some tiles in the map for Layer 1 (foreground)
    // Add a grid of tiles using tile 0 and tile 2 (empty)
    vMemSetAddr(LAYER1_MAP_MEM);
    for (y=0; y<32; y++) {
        for (x=0; x<64; x++) {
            // vMemSetData0(2);
            // vMemSetData0(0);
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

void spritesCreate() {
    unsigned short i;

    // You have to set the address you want to write to in VMEM using 9F20/21
    // 9F22 controls how much the VMEM address increments after each read/write
    // Then you can peek or poke using 0x9F23
    // The address is auto incremented and you can peek/poke again
    vMemSetBank0();
    vMemSetAddr(SPRITE_MEM);
    vMemSetIncMode(1);
    
    // Poke a block of solid data into vmem to use as sprite data
    // Big enough for 32x32 sprites
    for (i=0; i<32*32; i++) {
        vMemSetData0(199);
    }

    // Turn on sprites
    spriteSetGlobalOn();

    // Setup the IRQ handler for sprite collisions and enable global sprite collisions
    set_irq(&spriteCollisionIRQHandler, IRQHandlerStack, IRQ_HANDLER_STACK_SIZE);
    spriteCollisionsEnable();
}

void main() {
    unsigned short x;
    unsigned short y;
    unsigned char collision;
    unsigned char joy;
    unsigned char speed = 2;
    short bulletX = 520;
    short bulletY = 240;
    unsigned char bulletActive = 0;

    videoConfig();
    tilesConfig();
    layerMapsAddSomeStuff();
    spritesCreate();

    // Let's do a sprite collision test
    spriteInit(1, 0, 1, 0, SPRITE_MEM, 0b1011, BetweenL0L1, PX16, PX16); // bad guy
    spriteIdxSetXY(1, 0, 320, 240);

    spriteInit(1, 1, 1, 0, SPRITE_MEM, 0b1101, BetweenL0L1, PX32, PX32); // player
    x = 380;
    y = 235;
    spriteIdxSetXY(1, 1, x, y);

    spriteInit(1, 2, 1, 0, SPRITE_MEM, 0b1010, BetweenL0L1, PX16, PX8); // bullet
    spriteIdxSetXY(1, 2, bulletX, bulletY);
    
    // Configure the joysticks
    joy_install(cx16_std_joy);
    
    while (1) {
        waitforjiffy();
        
        joy = joy_read(0);
        if (JOY_UP(joy)) {
            y-=speed;
        } else if (JOY_DOWN(joy)) {
            y+=speed;
        }

        if (JOY_LEFT(joy)) {
            x-=speed;
        } else if (JOY_RIGHT(joy)) {
            x+=speed;
        }

        // Quit
        if (JOY_BTN_1(joy)) {
            break;
        }

        if (bulletActive == 1) {
            bulletX-= 4;
            spriteIdxSetXY(1, 2, bulletX, bulletY);
        }

        if (JOY_BTN_2(joy) && bulletActive==0) {
            bulletActive = 1;
            bulletX = x;
            bulletY = y;
            spriteIdxSetZDepth(1, 2, BetweenL0L1);
            spriteIdxSetXY(1, 2, bulletX, bulletY);
        }

        spriteIdxSetXY(1, 1, x, y);

        // Get the Collision bits and shift them down
        collision = spriteCollisionBitsGet();

        // The collision bits will be the OVERLAP of the Collision Masks of the two sprites.
        // We can then look at all the sprites that have that bit in their mask

        // Player and Enemy collisions
        if (collision == 0b1001) {
            // Move the sprite back
            // This will cause repeated collisions
            // Want to make sure that works
            x = 380;
            y = 235;
            spriteIdxSetXY(1, 1, x, y);
        } else if (collision == 0b1010) {
            bulletActive = 0;
            spriteIdxSetZDepth(1, 2, Disabled);
        }

        // Bullet is off screen
        if (bulletX < 0) {
            bulletActive = 0;
            spriteIdxSetZDepth(1, 2, Disabled);
        }
    }

    // Disable sprite collisions before quitting
    // or the UI hangs if sprites are still touching.
    spriteCollisionsDisable();
}