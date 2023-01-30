#include "x16graphics.h"
#include "level.h"
#include "memmap.h"

void addLevelTiles(unsigned char length, TileLayout layout[]) {
    unsigned short layerMem;
    unsigned char i, l, t, byte2;

    vMemSetBank(LAYER1_MAP_MEM_BANK);
    for (i=0,t=2; i<length; i++) {
        layerMem = layout[i].layer == 0 ? LAYER0_MAP_MEM : LAYER1_MAP_MEM;
        vMemSetAddr(layerMem+(layout[i].y * TILES_ACROSS * 2)+(layout[i].x * 2));
        // 2nd byte controls vert/horz flips
        byte2 = layout[i].hFlip<<2 | layout[i].vFlip<<3;

        for (l=0; l<layout[i].length; l++) {    
            // Some special type repeats floor tiles with tiles for some/all of the ends
            // 255 - Caps both ends
            if (layout[i].type == 255) {
                if (l==0) {
                    vMemSetData0(1);
                } else if (l==layout[i].length-1) {
                    vMemSetData0(5);
                } else {
                    vMemSetData0(t);
                    t++;
                    if (t==5) {
                        t=2;
                    }
                }
            } else if (layout[i].type == 253) { // Cap on left end only
                if (l==0) {
                    vMemSetData0(1);
                } else {
                    vMemSetData0(t);
                    t++;
                    if (t==5) {
                        t=2;
                    }
                }
            } else if (layout[i].type == 254) { // Cap on right end only
                if (l==layout[i].length-1) {
                    vMemSetData0(5);
                } else {
                    vMemSetData0(t);
                    t++;
                    if (t==5) {
                        t=2;
                    }
                }
            } else {
                vMemSetData0(layout[i].type);
            }
            vMemSetData0(byte2);
        }
    }
}