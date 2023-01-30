#include <cx16.h>
#include <peekpoke.h>
#include <cbm.h>
#include "x16graphics.h"

void copyBankedRAMToVRAM(unsigned char startMemBank, unsigned char vramBank, unsigned short vramAddr, 
    unsigned char frames, unsigned char width, unsigned char height, unsigned char imageData[])
{
    unsigned char f, h, i;
    unsigned short start;

    vMemSetBank(vramBank);
    vMemSetAddr(vramAddr);
    vMemSetIncMode(1);

    POKE(0, startMemBank);

    for (f=0; f<frames; f++) {
        for (h=0; h<height; h++) {
            start = (f * width) + (h * frames * width);
            for (i=0; i<width; i++) {
                vMemSetData0(imageData[start+i]);
            }
        }
    }
}

void imageFileLoad(unsigned char startMemBank, unsigned char vramBank, unsigned short vramAddr, unsigned char *filename,
    unsigned char frames, unsigned char width, unsigned char height)
{
    // The memory bank to start loading data into.
    // The bank will auto-jump to the next bank when this bank is full (pretty neat).
    POKE(0, startMemBank);

    // Note about the files
    // The first 2 bytes are ignored in this mode so you have to pad your images with 2 leading bytes
    // Those bytes can be used as an address to load the contents into rather than specifying the address
    // NOTE: BANK_RAM is the starting addr of Banked RAM. Its part of cx16.h, good to use in case it changes
    cbm_k_setnam(filename);
    cbm_k_setlfs(0, 8, 0);
    cbm_k_load(0, (unsigned int)BANK_RAM);
    copyBankedRAMToVRAM(startMemBank, vramBank, vramAddr, frames, width, height, BANK_RAM);
}