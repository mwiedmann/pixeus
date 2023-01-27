#include "x16graphics.h"
#include "memmap.h"

void tilesConfig() {
    unsigned short x;
    unsigned short y;

    // Create some tiles
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
