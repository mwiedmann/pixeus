#include <stdio.h>
#include "x16graphics.h"
#include "memmap.h"
#include "imageload.h"

void standardTilesLoad() {
    unsigned short x;
    unsigned short y;

    vMemSetBank(TILE_MEM_BANK);
    vMemSetAddr(TILE_MEM);
    vMemSetIncMode(1);

    // Empty tile
    for (y=0; y < 16; y++) {
        for (x=0; x < 16; x++) {
            vMemSetData0(0);
        }
    }

    // Solid Black tile
    for (y=0; y < 16; y++) {
        for (x=0; x < 16; x++) {
            vMemSetData0(16);
        }
    }

    imageFileLoad(IMAGE_LOAD_BANK, TILE_MEM_BANK, TILE_SCUBA, "images/scuba.bin");
    imageFileLoad(IMAGE_LOAD_BANK, TILE_MEM_BANK, TILE_WEAPON, "images/weapon.bin");
    imageFileLoad(IMAGE_LOAD_BANK, TILE_MEM_BANK, TILE_BOOTS, "images/boots.bin");
    imageFileLoad(IMAGE_LOAD_BANK, TILE_MEM_BANK, TILE_FLAME, "images/flame.bin");
    imageFileLoad(IMAGE_LOAD_BANK, TILE_MEM_BANK, TILE_SNOWFLAKE, "images/snowflake.bin");
    imageFileLoad(IMAGE_LOAD_BANK, TILE_MEM_BANK, TILE_SET_FONT, "images/fontsimple.bin");
}

void tilesConfig(unsigned char tilesetId) {
    char filename[32];

    // Create some tiles
    vMemSetBank(TILE_MEM_BANK);
    vMemSetAddr(TILE_MEM);
    vMemSetIncMode(1);

    sprintf(filename, "images/tileset%d.bin", tilesetId);
    imageFileLoad(IMAGE_LOAD_BANK, TILE_MEM_BANK, TILE_SET_1_MEM, filename);
}

void paletteLoad() {
    imageFileLoad(IMAGE_LOAD_BANK, PALETTE_VRAM_BANK, PALETTE_VRAM_ADDR, "images/palette.bin");
}