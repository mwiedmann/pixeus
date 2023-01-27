#include "x16graphics.h"
#include "memmap.h"

extern unsigned char greentreeTilesImage[];

void tilesConfig() {
    unsigned short x;
    unsigned short y;
    unsigned short xTiles, yTiles, start, tx, ty;

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

    // This tileset is 6x10 Tiles, each tile is 16x16 pixels
    xTiles = 6;
    yTiles = 10;
    for (ty=0; ty < yTiles; ty++) {
        for (tx=0; tx < xTiles; tx++) {
            for (y=0; y < 16; y++) {
                start = (ty * xTiles * 16 * 16) + (tx * 16) + (y * xTiles * 16);
                for (x=0; x < 16; x++) {
                    vMemSetData0(greentreeTilesImage[start+x]);
                }
            }        
        }
    }
}
