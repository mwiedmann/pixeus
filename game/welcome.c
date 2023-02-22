#include <stdio.h>

#include <peekpoke.h>
#include <joystick.h>
#include "waitforjiffy.h"
#include "x16graphics.h"
#include "memmap.h"
#include "imageload.h"
#include "fontmgr.h"
#include "sprites.h"
#include "levelutils.h"
#include "gamesprites.h"
#include "levelmgr.h"

unsigned char showTitleScreen() {
    unsigned char joy = 0;
    unsigned char testMode = 0;

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

    while(1) {
        joy = joy_read(0);

        // Exit when either button is pressed
        // Exit with testMode ON if UP is pressed
        if (JOY_BTN_1(joy)) {
            while(JOY_BTN_1(joy)) {
                joy = joy_read(0);
            }
            break;
        } else if (JOY_BTN_2(joy)) {
            while(JOY_BTN_2(joy)) {
                joy = joy_read(0);
            }
            break;
        } if (JOY_UP(joy)) {
            while(JOY_UP(joy)) {
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

void showIntroScene(Sprite *ship) {

    drawCenteredTextRow("PIXEUS THE EXPLORER", 0, 3);
    
    drawCenteredTextRow("LOST IN SPACE IN A", 0, 5);
    drawCenteredTextRow("SHIP LOW ON ENERGY", 0, 6);

    drawCenteredTextRow("PERHAPS THIS PLANET PROVIDES HOPE?", 0, 8);

    drawCenteredTextRow("PLENTIFUL IN GOLD AND ENERGY", 0, 10);
    drawCenteredTextRow("PIXEUS LANDS IN THIS STRANGE WORLD", 0, 11);

    drawCenteredTextRow("ESCAPE IS THE GOAL", 0, 13);
    drawCenteredTextRow("BUT GOLD IS THE SIREN'S CALL", 0, 14);
    
    spriteMove(ship, 288, 350);
    x16SpriteIdxSetXY(ship->index, ship->x, ship->y);
    ship->zDepth = BetweenL0L1;
    x16SpriteIdxSetZDepth(ship->index, ship->zDepth);

    while(1) {
        // Wait for screen to finish drawing since we are animating the ship
        waitforjiffy();
        spriteAnimationAdvance(ship);

        // Exit when either button is pressed
        if (readButtonPress()) {
            break;
        }
    }

    layerMapsClear();

    drawCenteredTextRow("HOW TO PLAY", 0, 2);
    
    drawTextRow(" MOVE - JOYSTICK LEFT-RIGHT", 0, 4, 5);
    drawTextRow(" JUMP - BUTTON 1 OR JOYSTICK UP", 0, 5, 5);
    drawTextRow("SHOOT - BUTTON 2", 0, 6, 5);

    drawCenteredTextRow("FIND 100 ENERGY AND", 0, 8);
    drawCenteredTextRow("RETURN TO YOUR SHIP TO ESCAPE", 0, 9);

    drawCenteredTextRow("THE MORE GOLD YOU FIND", 0, 11);
    drawCenteredTextRow("THE GREATER YOUR ENDING", 0, 12);

    drawCenteredTextRow("WATER IS:", 0, 14);

    drawCenteredTextRow("DEADLY UNTIL YOU FIND SCUBA GEAR", 0, 16);
    drawCenteredTextRow("YOUR FRIEND IN THE DESERT", 0, 17);
    drawCenteredTextRow("BONE CHILLING IN THE TUNDRA", 0, 18);

    drawCenteredTextRow("GOOD LUCK!", 0, 20);

    while(1) {
        // Wait for screen to finish drawing since we are animating the ship
        waitforjiffy();
        spriteAnimationAdvance(ship);

        // Exit when either button is pressed
        if (readButtonPress()) {
            break;
        }
    }

    spriteMoveBack(ship);
    x16SpriteIdxSetXY(ship->index, ship->x, ship->y);
}

void gameOverScreen() {
    layerMapsClear();

    drawCenteredTextRow("PIXEUS SURRENDERS TO SLEEP AT LAST", 0, 7);

    drawCenteredTextRow("NO LONGER KEEPING WATCH", 0, 9);
    drawCenteredTextRow("THROUGH THE NIGHT", 0, 10);
    
    drawCenteredTextRow("PERHAPS IN THE NEXT LIFE PIXEUS", 0, 12);
    drawCenteredTextRow("YOU'LL COME UP FROM UNDER", 0, 13);
    drawCenteredTextRow("ALL YOUR TROUBLES", 0, 14);
    
    drawCenteredTextRow("GAME OVER", 0, 17);

    while(1) {
        // Exit when either button is pressed
        if (readButtonPress()) {
            break;
        }
    }
}