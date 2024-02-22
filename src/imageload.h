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

unsigned long loadFileToVRAM(char *filename, unsigned char vramBank, unsigned short vramAddr);

#endif