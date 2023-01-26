#include <peekpoke.h>
#include <6502.h>
#include <cx16.h>
#include <joystick.h>
#include <conio.h>
#include "x16graphics.h"
#include "sprites.h"

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

// External Sprites
extern unsigned char guyrunImage[];

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
                vMemSetData0(162);
        }
    }

    // Make a light background
    for (y=0; y < 16; y++) {
        for (x=0; x < 16; x++) {
            vMemSetData0(138);            
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

void spriteDataLoad() {
    unsigned short i;
    unsigned char frames, f;
    unsigned char width;
    unsigned char height, h;
    unsigned short start;

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

    // This has multiple frames so we have to jump around
    // Need to know the width, height, and number of frames
    frames = 8;
    height = 32;
    width = 32;
    for (f=0; f<8; f++) {
        for (h=0; h<height; h++) {
            start = (f * width) + (h * frames * width);
            for (i=0; i<width; i++) {
                vMemSetData0(guyrunImage[start+i]);
            }
        }
    }
    
}

void spriteIRQConfig() {
    // Turn on sprites
    x16SpriteSetGlobalOn();

    // Setup the IRQ handler for sprite collisions and enable global sprite collisions
    set_irq(&x16SpriteCollisionIRQHandler, IRQHandlerStack, IRQ_HANDLER_STACK_SIZE);
    x16SpriteCollisionsEnable();
}

void playerCreate(Sprite *p) {
    p->index = 1;
    p->spriteBank = 1;
    p->clrMode = 1;
    p->collisionMask = 0b1101;
    p->zDepth = BetweenL0L1;
    p->width = PX32;
    p->height = PX32;
    p->graphicsBank = 0;
    p->graphicsAddress = SPRITE_MEM+1024;
    p->frames = 8;
    p->frameSize = 1024; // Calculated as width * height
    p->animationCount = 0;
    p->animationSpeed = 6;
    p->animationStopFrame = 4;
    p->animationDirection = 0;
    p->animationFrame = 0;
    p->x = 380;
    p->y = 235;
    p->speed = 2;

    spriteInit(p);
    x16SpriteIdxSetXY(p->spriteBank, p->index, p->x, p->y);
}

void badguyCreate(Sprite *bg) {
    bg->index = 0;
    bg->spriteBank = 1;
    bg->clrMode = 1;
    bg->collisionMask = 0b1011;
    bg->zDepth = BetweenL0L1;
    bg->width = PX32;
    bg->height = PX32;
    bg->graphicsBank = 0;
    bg->graphicsAddress = SPRITE_MEM;
    bg->frames = 1;
    bg->x = 320;
    bg->y = 240;

    spriteInit(bg);
    x16SpriteIdxSetXY(bg->spriteBank, bg->index, bg->x, bg->y);
}

void bulletCreate(Sprite *b) {
    b->index = 2;
    b->spriteBank = 1;
    b->clrMode = 1;
    b->collisionMask = 0b1010;
    b->zDepth = Disabled;
    b->width = PX16;
    b->height = PX8;
    b->graphicsBank = 0;
    b->graphicsAddress = SPRITE_MEM;
    b->frames = 1;
    b->x = 320;
    b->y = 240;

    spriteInit(b);
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