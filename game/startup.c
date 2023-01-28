#include <peekpoke.h>

#include "x16graphics.h"
#include "memmap.h"

void videoConfig() {
    // clear the screen to start
    // This happens when we draw the screen anyway. If we need again...
    // #include <conio.h>
    // clrscr();

    // We are using 640x480 with 256clr, 16x16 Tiles
    // Each tile is 256 bytes, but we won't need THAT many so this feels ok

    // For the layers, we will use both layers with the sprites inbetween them (L0 in back).
    // Layer Maps will be in mode:
    //  Height: 00 (gives us 32 Tiles which is more than the 30 we need: 480/16=30)
    //  Width:  01 (gives us 64 Tiles which is more than the 40 we need: 640/16=40)
    // Each Map will be 4096 bytes

    // Enable both layers (leave other settings the same)
    POKE(VMEM_VIDEO, PEEK(VMEM_VIDEO) | 0b00110000);

    // Config both layers - H/W, 256clr, 11 color depth
    POKE(LAYER_0_CONFIG, 0b00011011);
    POKE(LAYER_1_CONFIG, 0b00011011);

    // Mapbase for both layers - Bank 0
    POKE(LAYER_0_MAPBASE, memoryToMapMemoryAddress(LAYER0_MAP_MEM_BANK, LAYER0_MAP_MEM));
    POKE(LAYER_1_MAPBASE, memoryToMapMemoryAddress(LAYER1_MAP_MEM_BANK, LAYER1_MAP_MEM));

    // Tilebase - Put after the Mapbases for both layers
    POKE(LAYER_0_TILEBASE, tileBaseConfig(TILE_MEM_BANK, TILE_MEM, 1, 1));
    POKE(LAYER_1_TILEBASE, tileBaseConfig(TILE_MEM_BANK, TILE_MEM, 1, 1));
}
