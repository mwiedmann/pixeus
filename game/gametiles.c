#include "x16graphics.h"
#include "memmap.h"

extern unsigned char greentreeTilesImage[];

void tilesConfig() {
    unsigned short x;
    unsigned short y;
    unsigned short frames;
    unsigned short f, h, start;

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

    // This has multiple frames so we have to jump around
    // Need to know the width, height, and number of frames
    frames = 5;
    for (f=0; f<frames; f++) {
        for (h=0; h<16; h++) {
            start = (f * 16) + (h * frames * 16);
            for (x=0; x<16; x++) {
                vMemSetData0(greentreeTilesImage[start+x]);
            }
        }
    }
}
