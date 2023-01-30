#include <cx16.h>
#include <peekpoke.h>
#include <cbm.h>
#include "x16graphics.h"

#define ROM_START 0xC000
#define BANK_SIZE 8192

void copyBankedRAMToVRAM(unsigned char startMemBank, unsigned char vramBank, unsigned short vramAddr, 
    unsigned char xTiles, unsigned char yTiles, unsigned char frameWidth, unsigned char frameHeight, unsigned short startingImageAddr)
{
    unsigned char ty, tx, y, x;
    unsigned short start;
    unsigned short imageAddr, diff, bankInc, remainingMem;
    unsigned char lastBank = startMemBank;
    vMemSetBank(vramBank);
    vMemSetAddr(vramAddr);
    vMemSetIncMode(1);

    POKE(0, startMemBank);

    for (ty=0; ty < yTiles; ty++) {
        for (tx=0; tx < xTiles; tx++) {
            for (y=0; y < frameHeight; y++) {
                start = (ty * xTiles * frameWidth * frameHeight) + (tx * frameWidth) + (y * xTiles * frameHeight);
                for (x=0; x < frameWidth; x++) {
                    imageAddr = startingImageAddr + start + x;
                    // See if we have crossed into another bank
                    // WARN - Jumping banks like this is a slower but it works
                    // It may be better to break images into 8k files so they fit
                    diff = (imageAddr - (unsigned short)BANK_RAM) + 1;
                    bankInc = (diff-1)/BANK_SIZE;

                    // See if we need to switch mem banks
                    if (lastBank != startMemBank+bankInc) {
                        lastBank = startMemBank+bankInc;
                        POKE(0, lastBank);
                    }

                    // If we are in a higher bank, we need to trim off the memory
                    // from lower banks and use the remaining mem address
                    if (bankInc > 0) {
                        remainingMem = diff-(bankInc*BANK_SIZE)-1;   
                        vMemSetData0(PEEK((unsigned short)BANK_RAM+remainingMem));
                    } else {
                        vMemSetData0(PEEK(imageAddr));
                    }
                }
            }        
        }
    }
}

void imageFileLoad(unsigned char startMemBank, unsigned char vramBank, unsigned short vramAddr, unsigned char *filename,
    unsigned char xTiles, unsigned char yTiles, unsigned char frameWidth, unsigned char frameHeight)
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

    copyBankedRAMToVRAM(startMemBank, vramBank, vramAddr, xTiles, yTiles, frameWidth, frameHeight, (unsigned short)BANK_RAM);
}