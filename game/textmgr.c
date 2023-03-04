#include <stdio.h>
#include <cbm.h>
#include <cx16.h>

#include <peekpoke.h>
#include <joystick.h>
#include "waitforjiffy.h"
#include "loopmgr.h"
#include "x16graphics.h"
#include "memmap.h"
#include "imageload.h"
#include "fontmgr.h"
#include "sprites.h"
#include "levelutils.h"
#include "gamesprites.h"
#include "levelmgr.h"
#include "soundmgr.h"

unsigned char showTitleScreen() {
    unsigned char joy = 0;
    unsigned char testMode = 0;
    unsigned char wait = 60;

    // Clear the VRAM we will use for the bitmap mode title
    clearFullVRAM(0);
    clearVRAM(1, 0, 11264);

    // Set to 320x240 size
    POKE(0x9F2A, 64);
    POKE(0x9F2B, 64);

    // Enable Layer 1 only (probably already set this way but just in case) (leave other settings the same)
    POKE(VMEM_VIDEO, PEEK(VMEM_VIDEO) | 0b00100000);
    POKE(VMEM_VIDEO, PEEK(VMEM_VIDEO) & 0b11101111); // Disable layer 0 if its on

    // Config Layer 1 - H/W, 256clr, Bitmap Mode, 8bpp color depth
    POKE(LAYER_1_CONFIG, 0b00001111);

    // Bitmap mode loading a 320x240 image takes most of VRAM
    // Start in VBank 0, Addr 0
    POKE(LAYER_1_TILEBASE, 0);

    imageFileLoad(IMAGE_LOAD_BANK, 0, 0, "images/title.bin");
    preloadTextFiles();

    // Short delay before playing the music and button press skips title screen
    while (wait > 0) {
        waitforjiffy();
        wait--;
    }

    loadTitleMusic();
    startMusic();

    while(1) {
        loopUpdates();

        joy = joy_read(0);

        // Exit when either button is pressed
        // Exit with testMode ON if UP is pressed
        if (JOY_BTN_1(joy)) {
            while(JOY_BTN_1(joy)) {
                loopUpdates();
        
                joy = joy_read(0);
            }
            break;
        } else if (JOY_BTN_2(joy)) {
            while(JOY_BTN_2(joy)) {
                loopUpdates();
        
                joy = joy_read(0);
            }
            break;
        } if (JOY_UP(joy)) {
            while(JOY_UP(joy)) {
                loopUpdates();
        
                joy = joy_read(0);
            }
            testMode = 1;
            break;
        }
    }

    // Turn off Layer 1 (both layers will be off while things are loading)
    POKE(VMEM_VIDEO, PEEK(VMEM_VIDEO) ^ 0b00100000);

    return testMode;
}

void waitWithShipAnimation(Sprite *ship) {
    while(1) {
        // Wait for screen to finish drawing since we are animating the ship
        loopUpdates();
        spriteAnimationAdvance(ship);

        // Exit when either button is pressed
        if (readButtonPress()) {
            break;
        }
    }
}

void showIntroScene(Sprite *ship) {
    drawTextFileFromBank(WELCOME_BANK, 0);

    spriteMove(ship, 288, 350);
    x16SpriteIdxSetXY(ship->index, ship->x, ship->y);
    ship->zDepth = BetweenL0L1;
    x16SpriteIdxSetZDepth(ship->index, ship->zDepth);

    waitWithShipAnimation(ship);

    layerMapsClear();

    drawTextFileFromBank(INSTRUCTIONS_BANK, 0);

    waitWithShipAnimation(ship);

    spriteMoveBack(ship);
    x16SpriteIdxSetXY(ship->index, ship->x, ship->y);
}

void gameOverScreen(unsigned short gold, unsigned char energy) {
    unsigned char scoreRow[41];

    layerMapsClear();

    sprintf(scoreRow, "GOLD:%04u ENERGY:%03u", gold, energy);
    drawCenteredTextRow(scoreRow, 0, 4);

    drawTextFile("text/gameover.bin", 0);

    while(1) {
        loopUpdates();
        
        // Exit when either button is pressed
        if (readButtonPress()) {
            break;
        }
    }
}

void victoryScreen(Sprite *ship, unsigned short gold) {
    unsigned char text[41];

    layerMapsClear();

    if (gold == 0) {
        drawTextFile("text/nogold.bin", 0);
    } else {
        sprintf(text, "PIXEUS SPENDS %u GOLD ON:", gold);
        drawCenteredTextRow(text, 0, 6);

        drawTextFile("text/victory.bin", gold);
    }
   
    spriteMove(ship, 288, 350);
    x16SpriteIdxSetXY(ship->index, ship->x, ship->y);
    ship->zDepth = BetweenL0L1;
    x16SpriteIdxSetZDepth(ship->index, ship->zDepth);

    while(1) {
        // Wait for screen to finish drawing since we are animating the ship
        loopUpdates();
        
        spriteAnimationAdvance(ship);

        // Exit when either button is pressed
        if (readButtonPress()) {
            break;
        }
    }
}