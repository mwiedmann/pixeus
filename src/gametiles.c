#include <stdio.h>
#include "x16graphics.h"
#include "memmap.h"
#include "imageload.h"

#pragma code-name (push, "BANKRAM01")

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

    // These are sprites because they also show as items in the game
    // Load them via their sprite IDs.
    loadFileToVRAM("sp9.bin", TILE_MEM_BANK, TILE_SCUBA);
    loadFileToVRAM("sp10.bin", TILE_MEM_BANK, TILE_WEAPON);
    loadFileToVRAM("sp11.bin", TILE_MEM_BANK, TILE_BOOTS);

    loadFileToVRAM("flame.bin", TILE_MEM_BANK, TILE_FLAME);
    loadFileToVRAM("snow.bin", TILE_MEM_BANK, TILE_SNOWFLAKE);
    loadFileToVRAM("font.bin", TILE_MEM_BANK, TILE_SET_FONT);
}

void tilesConfig(unsigned char tilesetId) {
    char filename[32];

    // Create some tiles
    vMemSetBank(TILE_MEM_BANK);
    vMemSetAddr(TILE_MEM);
    vMemSetIncMode(1);

    sprintf(filename, "tileset%d.bin", tilesetId);
    loadFileToVRAM(filename, TILE_MEM_BANK, TILE_SET_1_MEM);
}

void paletteLoad() {
    loadFileToVRAM("palette.bin", PALETTE_VRAM_BANK, PALETTE_VRAM_ADDR);
}

#pragma code-name (pop)