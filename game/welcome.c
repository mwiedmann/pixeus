#include <peekpoke.h>
#include <joystick.h>
#include "x16graphics.h"
#include "memmap.h"
#include "imageload.h"

void showTitleScreen() {
    unsigned char joy = 0;

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

    imageFileLoad(2, 0, 0, "images/title.bin", 76800L);

    while(!JOY_BTN_1(joy) && !JOY_BTN_2(joy)) {
        joy = joy_read(0);
    }

    // Turn off Layer 1 (both layers will be off while things are loading)
    POKE(VMEM_VIDEO, PEEK(VMEM_VIDEO) ^ 0b00100000);
}
