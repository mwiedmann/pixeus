#ifndef GAMETILES_H
#define GAMETILES_H

#define TILES_START 1
#define EMPTY_TILE 0

#define TILES_ACROSS 64
#define TILES_DOWN 32
#define TILES_SCREEN_ACROSS 40
#define TILES_SCREEN_DOWN 30
#define TILE_PIXEL_WIDTH 16
#define TILE_PIXEL_WIDTH_HALF 8
#define TILE_PIXEL_HEIGHT 16
#define TILE_PIXEL_HEIGHT_HALF 8

void standardTilesLoad();
void tilesConfig(unsigned char tilesetId);
void paletteLoad();

#endif