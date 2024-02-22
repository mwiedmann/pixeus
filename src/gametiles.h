#ifndef GAMETILES_H
#define GAMETILES_H

#define EMPTY_TILE 0
#define BLACK_TILE 1
#define SCUBA_TILEID 2
#define WEAPON_TILEID 3
#define BOOTS_TILEID 4
#define FLAME_TILEID 5
#define SNOWFLAKE_TILEID 6
#define FONT_TILEID_START 7

#define TILES_ACROSS 64
#define TILES_DOWN 32
#define TILES_SCREEN_ACROSS 40
#define TILES_SCREEN_DOWN 30
#define TILE_PIXEL_WIDTH 16
#define TILE_PIXEL_WIDTH_HALF 8
#define TILE_PIXEL_HEIGHT 16
#define TILE_PIXEL_HEIGHT_HALF 8

#define TILESETID_FOREST 1
#define TILESETID_DESERT 2
#define TILESETID_WINTER 3
#define TILESETID_FIRE 4

void standardTilesLoad();
void tilesConfig(unsigned char tilesetId);
void paletteLoad();

#endif