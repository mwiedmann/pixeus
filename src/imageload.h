#ifndef IMAGELOAD_H
#define IMAGELOAD_H

/**
 * Clear (set to 0) a section of VRAM
*/
void clearVRAM(unsigned char vramBank, unsigned short vramAddr, unsigned long length) ;

/**
 * Clear (set to 0) an entire VRAM bank
*/
void clearFullVRAM(unsigned char vramBank);

/**
 * Copy data from Banked RAM into VRAM
*/
void copyBankedRAMToVRAM(unsigned char startMemBank, unsigned char vramBank, unsigned short vramAddr, unsigned long length, unsigned short startingImageAddr);

/**
 * Load an image file into VRAM. Used to load sprites and tilesets.
 * Returns the number of bytes read (excluding the 2 byte header)
*/
unsigned long imageFileLoad(unsigned char startMemBank, unsigned char vramBank, unsigned short vramAddr, char *filename);

void loadFileToVRAM(char *filename, unsigned char vramBank, unsigned short vramAddr);

#endif