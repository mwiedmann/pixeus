#include <stdio.h>

#include <peekpoke.h>
#include <joystick.h>
#include "waitforjiffy.h"
#include "x16graphics.h"
#include "memmap.h"
#include "imageload.h"
#include "fontmgr.h"
#include "sprites.h"

unsigned char showTitleScreen() {
    unsigned char joy = 0;
    unsigned char testMode = 0;

    // Clear the VRAM we will use for the bitmap mode title
    clearFullVRAM(0);
    clearVRAM(1, 0, 11264);

    // Set to 320x240 size
    POKE(0x9F2A, 64);
    POKE(0x9F2B, 64);

    // Enable Layer 1 (probably already set this way but just in case) (leave other settings the same)
    POKE(VMEM_VIDEO, PEEK(VMEM_VIDEO) | 0b00100000);

    // Config Layer 1 - H/W, 256clr, Bitmap Mode, 8bpp color depth
    POKE(LAYER_1_CONFIG, 0b00001111);

    // Bitmap mode loading a 320x240 image takes most of VRAM
    // Start in VBank 0, Addr 0
    POKE(LAYER_1_TILEBASE, 0);

    imageFileLoad(2, 0, 0, "images/title.bin");

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
    unsigned char joy = 0;

    drawTextRow("        LOST IN SPACE IN A      ", 32, 3, 3);
    drawTextRow("        SHIP LOW ON ENERGY      ", 32, 4, 3);
    drawTextRow("      PIXEUS MUST CRASH LAND.   ", 32, 6, 3);
    drawTextRow("     ALTHOUGH THERE IS HOPE...  ", 32, 8, 3);
    drawTextRow("  WHILE THIS PLANET IS DANGEROUS", 32, 11, 3);
    drawTextRow("            AND STRANGE         ", 32, 12, 3);
    drawTextRow("    PIXEUS HAS DETECTED ENERGY  ", 32, 13, 3);
    drawTextRow("             AND GOLD!!!        ", 32, 14, 3);
    drawTextRow("      CAN PIXEUS MAKE IT HOME   ", 32, 16, 3);
    drawTextRow("       AND SAVE HIS COLONY?     ", 32, 17, 3);

    spriteMove(ship, 288, 350);
    x16SpriteIdxSetXY(ship->index, ship->x, ship->y);
    ship->zDepth = BetweenL0L1;
    x16SpriteIdxSetZDepth(ship->index, ship->zDepth);
        
    while(1) {
        // Wait for screen to finish drawing since we are animating the ship
        waitforjiffy();

        joy = joy_read(0);

        ship->animationCount++;
        if (ship->animationCount == ship->animationSpeed) {
            ship->animationCount=0;
            ship->animationFrame++;
            if (ship->animationFrame == ship->frames) {
                ship->animationFrame = 0;
            }
            x16SpriteIdxSetGraphicsPointer(ship->index, ship->clrMode, ship->graphicsBank,
                ship->graphicsAddress+(ship->animationFrame * ship->frameSize));
        }

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
        }
    }

    spriteMoveBack(ship);
    x16SpriteIdxSetXY(ship->index, ship->x, ship->y);
}