#include "x16graphics.h"
#include "memmap.h"

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

    // Make a light background tile
    for (y=0; y < 16; y++) {
        for (x=0; x < 16; x++) {
            vMemSetData0(138);            
        }
    }

    // Platform tile
    for (y=0; y < 16; y++) {
        for (x=0; x < 16; x++) {
            vMemSetData0(165);
        }
    }
}
