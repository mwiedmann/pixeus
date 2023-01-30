#include "x16graphics.h"
#include "memmap.h"
#include "imageload.h"

void tilesConfig() {
    unsigned short x;
    unsigned short y;

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

    imageFileLoad(2, TILE_MEM_BANK, TILE_SET_1_MEM, "images/tileset1.bin", 19968);
}
