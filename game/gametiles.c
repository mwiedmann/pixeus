#include <stdio.h>
#include "x16graphics.h"
#include "memmap.h"
#include "imageload.h"

void tilesConfig(unsigned char tilesetId) {
    unsigned short x;
    unsigned short y;
    char filename[32];

    // Create some tiles
    vMemSetBank(TILE_MEM_BANK);
    vMemSetAddr(TILE_MEM);
    vMemSetIncMode(1);

    // Empty tile
    for (y=0; y < 16; y++) {
        for (x=0; x < 16; x++) {
            vMemSetData0(0);
        }
    }

    sprintf(filename, "images/tileset%d.bin", tilesetId);
    imageFileLoad(2, TILE_MEM_BANK, TILE_SET_1_MEM, filename);
}

void loadFont() {
    imageFileLoad(2, FONT_MEM_BANK, FONT_MEM, "images/fontsimple.bin");
}