#ifndef MEMMAP_H
#define MEMMAP_H

// Default video on X16 startup
// Layer 1 is ON (Layer 0 is OFF)
// $9F35 Layer 1 - Mapbase  - 216 - 11011000 - VBank 1 - 1011000000000000 - B000 / 45056
// $9F36 Layer 1 - Tilebase - 248 - 11111000 - VBank 1 - 1111000000000000 - F000 / 61440 (4k from the end)

// We put the Map Memory and Sprites in Bank 1
// **** BANK 1 ****
// The Map Memory for each layer needs 4k
#define LAYER0_MAP_MEM 0
#define LAYER0_MAP_MEM_BANK 1

#define LAYER1_MAP_MEM 4096
#define LAYER1_MAP_MEM_BANK 1

// The sprites are also in Bank 1 and start after the Map Memory.
#define SPRITE_MEM 8192
#define SPRITE_MEM_BANK 1

// Start the sprites at SPRITE_MEM and increment by the size of each sprite
#define SPRITE_MEM_PLAYER SPRITE_MEM

// Player bullet is last frame on Player Sprite
#define SPRITE_MEM_PLAYER_BULLET SPRITE_MEM_PLAYER + 1280

#define SPRITE_MEM_EXP_SMALL SPRITE_MEM_PLAYER + 1536
#define SPRITE_MEM_SNAKE SPRITE_MEM_EXP_SMALL + 1024
#define SPRITE_MEM_BEE SPRITE_MEM_SNAKE + 1024
#define SPRITE_MEM_GHOST SPRITE_MEM_BEE + 1024
#define SPRITE_MEM_SCORPION SPRITE_MEM_GHOST + 1024
#define SPRITE_MEM_WASP SPRITE_MEM_SCORPION + 1024

// **** BANK 0 ****
// Tiles eat up the most memory so it lives alone in Bank 0
// Currently its around 46k
// We will need to figure something out if it exceeds 64k
#define TILE_MEM 0
#define TILE_EMPTY_MEM TILE_MEM
#define TILE_SET_1_MEM TILE_EMPTY_MEM + 256
#define TILE_MEM_BANK 0

#define TILES_ACROSS 64
#define TILES_DOWN 32
#define TILES_SCREEN_ACROSS 40
#define TILES_SCREEN_DOWN 30
#define TILE_PIXEL_WIDTH 16
#define TILE_PIXEL_WIDTH_HALF 8
#define TILE_PIXEL_HEIGHT 16

#endif