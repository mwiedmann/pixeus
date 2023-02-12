#include <stdio.h>
#include "x16graphics.h"
#include "memmap.h"
#include "imageload.h"

void standardTilesLoad() {
    unsigned short x;
    unsigned short y;

    // Empty tile
    for (y=0; y < 16; y++) {
        for (x=0; x < 16; x++) {
            vMemSetData0(0);
        }
    }

    imageFileLoad(2, TILE_MEM_BANK, TILE_SCUBA, "images/scuba.bin");
    imageFileLoad(2, TILE_MEM_BANK, TILE_WEAPON, "images/weapon.bin");
    imageFileLoad(2, TILE_MEM_BANK, TILE_BOOTS, "images/boots.bin");
    imageFileLoad(2, TILE_MEM_BANK, TILE_SET_FONT, "images/fontsimple.bin");
}

void tilesConfig(unsigned char tilesetId) {
    char filename[32];

    // Create some tiles
    vMemSetBank(TILE_MEM_BANK);
    vMemSetAddr(TILE_MEM);
    vMemSetIncMode(1);

    sprintf(filename, "images/tileset%d.bin", tilesetId);
    imageFileLoad(2, TILE_MEM_BANK, TILE_SET_1_MEM, filename);
}
