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

    // TODO: The length of the data needs to be dynamic
    // The imageload gets back a mem location, we should be able to calc from there
    imageFileLoad(2, TILE_MEM_BANK, TILE_SET_1_MEM, filename, 46080L);
}

void loadFont() {
    imageFileLoad(2, FONT_MEM_BANK, FONT_MEM, "images/fontsimple.bin", 208);
}