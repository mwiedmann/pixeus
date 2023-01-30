#ifndef IMAGELOAD_H
#define IMAGELOAD_H

/**
 * Copy data from Banked RAM into VRAM
*/
void copyBankedRAMToVRAM(unsigned char startMemBank, unsigned char vramBank, unsigned short vramAddr, 
    unsigned char xTiles, unsigned char yTiles, unsigned char frameWidth, unsigned char frameHeight, unsigned short startingImageAddr);

/**
 * Load an image file into VRAM. Used to load sprites and tilesets.
 * WARN: Large files will load across RAM Banks. This works but is slower to load into VRAM. Consider files no larger than 8k bytes to avoid this.
*/
void imageFileLoad(unsigned char startMemBank, unsigned char vramBank, unsigned short vramAddr, unsigned char *filename,
    unsigned char xTiles, unsigned char yTiles, unsigned char frameWidth, unsigned char frameHeight);

#endif