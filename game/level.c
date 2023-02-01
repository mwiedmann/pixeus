#include "x16graphics.h"
#include "level.h"
#include "memmap.h"

void addLevelTiles(unsigned short length, TileLayout layout[]) {
    unsigned short layerMem, i;
    unsigned char l, byte2;

    vMemSetBank(LAYER1_MAP_MEM_BANK);
    for (i=0; i<length; i++) {
        layerMem = layout[i].layer == 0 ? LAYER0_MAP_MEM : LAYER1_MAP_MEM;
        vMemSetAddr(layerMem+(layout[i].y * TILES_ACROSS * 2)+(layout[i].x * 2));
        // 2nd byte controls vert/horz flips
        byte2 = layout[i].hFlip<<2 | layout[i].vFlip<<3;

        for (l=0; l<layout[i].length; l++) {    
            vMemSetData0(layout[i].type);
            vMemSetData0(byte2);
        }
    }
}