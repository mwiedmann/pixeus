#ifndef MEMMAP_H
#define MEMMAP_H

// Default video on X16 startup
// $9F29 - 10100010 - Current Field:1, Sprites:0, Layer 1:On, Layer 0:Off, Unused:0, Chroma Disable:0, Output Mode: 10 (NTSC composite) 
// Layer 1 is ON (Layer 0 is OFF)
// $9F34 - 01100000 - 1 bpp, T256 is 0, so this is 16 color text mode, bitmap mode OFF, 128 cols (needed to show 80), 64 rows (needed to show 60) 
// $9F35 Layer 1 - Mapbase  - 216 - 11011000 - VBank 1 - 1011000000000000 - B000 / 45056
// $9F36 Layer 1 - Tilebase - 248 - 11111000 - VBank 1 - 1111000000000000 - F000 / 61440 (4k from the end)

// 10 FOR I=0 TO 127 : VPOKE 1, $B000+(I*2), I : NEXT I

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

#define SPRITE_MEM_PLAYER_SCUBA SPRITE_MEM_PLAYER + 1536
#define SPRITE_MEM_EXP_SMALL SPRITE_MEM_PLAYER_SCUBA + 1024
#define SPRITE_MEM_SNAKE SPRITE_MEM_EXP_SMALL + 1024
#define SPRITE_MEM_BEE SPRITE_MEM_SNAKE + 1024
#define SPRITE_MEM_GHOST SPRITE_MEM_BEE + 1024
#define SPRITE_MEM_SCORPION SPRITE_MEM_GHOST + 1024
#define SPRITE_MEM_WASP SPRITE_MEM_SCORPION + 1024
#define SPRITE_MEM_FISH1 SPRITE_MEM_WASP + 1024
#define SPRITE_MEM_BIG_GHOST SPRITE_MEM_FISH1 + 1024
#define SPRITE_MEM_LASER SPRITE_MEM_BIG_GHOST + 4096
#define SPRITE_MEM_SHIP SPRITE_MEM_LASER + 256
#define SPRITE_MEM_ENERGY SPRITE_MEM_SHIP + 16384L
#define SPRITE_MEM_GOLD SPRITE_MEM_ENERGY + 1024
#define SPRITE_MEM_SCUBA SPRITE_MEM_GOLD + 1024
#define SPRITE_MEM_WEAPON SPRITE_MEM_SCUBA + 256
#define SPRITE_MEM_BOOTS SPRITE_MEM_WEAPON + 256
#define SPRITE_MEM_EXTRA_LIFE SPRITE_MEM_BOOTS + 256

// **** BANK 0 ****
// Tiles eat up the most memory so it lives alone in Bank 0
// Currently its around 46k
// We will need to figure something out if it exceeds 64k
#define TILE_MEM 0
#define TILE_EMPTY_MEM TILE_MEM
#define TILE_SCUBA TILE_EMPTY_MEM + 256
#define TILE_WEAPON TILE_SCUBA + 256
#define TILE_BOOTS TILE_WEAPON + 256
#define TILE_SET_FONT TILE_BOOTS + 256
#define TILE_SET_1_MEM TILE_SET_FONT + 10240L
#define TILE_MEM_BANK 0

#define SCUBA_TILEID 1
#define WEAPON_TILEID 2
#define BOOTS_TILEID 3
#define FONT_TILEID_START 4

#endif