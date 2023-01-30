#ifndef IMAGELOAD_H
#define IMAGELOAD_H

/**
 * Copy data from Banked RAM into VRAM
*/
void copyBankedRAMToVRAM(unsigned char startMemBank, unsigned char vramBank, unsigned short vramAddr, unsigned short length, unsigned short startingImageAddr);

/**
 * Copy "tiled" data from Banked RAM into VRAM. Tiled data is organized like an image file so we have to jump around to different tiles
 * to "flatten" the data for use as x16 tile or sprite data
*/
void copyTiledBankedRAMToVRAM(unsigned char startMemBank, unsigned char vramBank, unsigned short vramAddr, 
    unsigned char xTiles, unsigned char yTiles, unsigned char frameWidth, unsigned char frameHeight, unsigned short startingImageAddr);

/**
 * Load an image file into VRAM. Used to load sprites and tilesets.
 * Images can be "tiled" or not. Tiled images are not flattened and look more like images with frames in squares.
 * WARN: Large files will load across RAM Banks. This works but is slower to load into VRAM. Consider files no larger than 8k bytes to avoid this.
*/
void imageTiledFileLoad(unsigned char startMemBank, unsigned char vramBank, unsigned short vramAddr, unsigned char *filename,
    unsigned char xTiles, unsigned char yTiles, unsigned char frameWidth, unsigned char frameHeight);

/**
 * Load an image file into VRAM. Used to load sprites and tilesets.
 * WARN: Large files will load across RAM Banks. This works but is slower to load into VRAM. Consider files no larger than 8k bytes to avoid this.
*/
void imageFileLoad(unsigned char startMemBank, unsigned char vramBank, unsigned short vramAddr, unsigned char *filename, unsigned short length);

#endif