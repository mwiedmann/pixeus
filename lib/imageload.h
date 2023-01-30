#ifndef IMAGELOAD_H
#define IMAGELOAD_H

/**
 * Copy data from Banked RAM into VRAM
*/
void copyBankedRAMToVRAM(unsigned char startMemBank, unsigned char vramBank, unsigned short vramAddr, 
    unsigned char frames, unsigned char width, unsigned char height, unsigned char imageData[]);

/**
 * Load an image file into VRAM.
 * Used to load sprites and tilesets.
*/
void imageFileLoad(unsigned char startMemBank, unsigned char vramBank, unsigned short vramAddr, unsigned char *filename,
    unsigned char frames, unsigned char width, unsigned char height);


#endif