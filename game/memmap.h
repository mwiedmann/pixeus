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
#define SPRITE_MEM_PLAYER_IDX 0

// Player bullet is last frame on Player Sprite
#define SPRITE_MEM_PLAYER_BULLET_OFFSET 1280

// Sprite image indexes (controls filename)
#define SPRITE_MEM_PLAYER_SCUBA_IDX 1
#define SPRITE_MEM_EXP_SMALL_IDX 2
#define SPRITE_MEM_SNAKE_IDX 3
#define SPRITE_MEM_BEE_IDX 4
#define SPRITE_MEM_GHOST_IDX 5
#define SPRITE_MEM_SCORPION_IDX 6
#define SPRITE_MEM_WASP_IDX 7
#define SPRITE_MEM_FISH1_IDX 8
#define SPRITE_MEM_BIG_GHOST_IDX 9
#define SPRITE_MEM_LASER_IDX 10
#define SPRITE_MEM_SHIP_IDX 11
#define SPRITE_MEM_ENERGY_IDX 12
#define SPRITE_MEM_GOLD_IDX 13
#define SPRITE_MEM_SCUBA_IDX 14
#define SPRITE_MEM_WEAPON_IDX 15
#define SPRITE_MEM_BOOTS_IDX 16
#define SPRITE_MEM_EXTRA_LIFE_IDX 17
#define SPRITE_MEM_EYEBALL_IDX 18
#define SPRITE_MEM_MUSHROOM_IDX 19
#define SPRITE_MEM_SLIME_IDX 20
#define SPRITE_MEM_SPIDER_IDX 21
#define SPRITE_MEM_RAT_IDX 22
#define SPRITE_MEM_ROCKMAN_IDX 23
#define SPRITE_MEM_EEL_IDX 24

// **** BANK 0 ****
// Tiles eat up the most memory so it lives alone in Bank 0
// Currently its around 46k
// We will need to figure something out if it exceeds 64k
#define TILE_MEM 0
#define TILE_EMPTY_MEM TILE_MEM
#define TILE_SOLID_MEM TILE_EMPTY_MEM + 256
#define TILE_SCUBA TILE_SOLID_MEM + 256
#define TILE_WEAPON TILE_SCUBA + 256
#define TILE_BOOTS TILE_WEAPON + 256
#define TILE_FLAME TILE_BOOTS + 256
#define TILE_SNOWFLAKE TILE_FLAME + 256
#define TILE_SET_FONT TILE_SNOWFLAKE + 256
#define TILE_SET_1_MEM TILE_SET_FONT + 10752L
#define TILE_MEM_BANK 0

/**
 * Hi Mem bank used to store current level data
*/
#define LEVEL_BANK 2

/**
 * Starting bank used when loading images
 * Needs to not collide with the LEVEL_BANK
*/
#define IMAGE_LOAD_BANK 3

#define WELCOME_BANK 10
#define INSTRUCTIONS_BANK 11

#define MUSIC_BANK 20
#define SOUND_BANK_LASER 28
#define SOUND_BANK_ALIEN_HIT 32

#endif