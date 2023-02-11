#include <stdio.h>

#include <peekpoke.h>
#include <joystick.h>
#include "x16graphics.h"
#include "memmap.h"
#include "imageload.h"
#include "fontmgr.h"

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
        if (JOY_BTN_1(joy) || JOY_BTN_2(joy)) {
            break;
        } else if (JOY_UP(joy)) {
            testMode = 1;
            break;
        }
    }

    // Turn off Layer 1 (both layers will be off while things are loading)
    POKE(VMEM_VIDEO, PEEK(VMEM_VIDEO) ^ 0b00100000);

    return testMode;
}

// void showIntroScene() {
//     unsigned char i;
//     unsigned char text[40];

//     sprintf(text, "LOST IN SPACE AND LOW ON ENERGY");
//     sprintf(text, "");

//     vMemSetBank(LAYER0_MAP_MEM_BANK);
//     vMemSetAddr(LAYER0_MAP_MEM);
//     for (i=0; i<40; i++) {
//         vMemSetData0(letterToTile(text[i]));
//         vMemSetData0(0);
//     }
// }