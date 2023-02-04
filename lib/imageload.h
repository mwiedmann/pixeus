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
 * WARN: Large files will load across RAM Banks. This works but is slower to load into VRAM. Consider files no larger than 8k bytes to avoid this.
*/
void imageFileLoad(unsigned char startMemBank, unsigned char vramBank, unsigned short vramAddr, unsigned char *filename, unsigned long length);

#endif