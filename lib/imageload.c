#include <cx16.h>
#include <peekpoke.h>
#include <cbm.h>
#include "x16graphics.h"

#define ROM_START 0xC000
#define BANK_SIZE 8192

void clearVRAM(unsigned char vramBank, unsigned short vramAddr, unsigned long length) {
    vMemSetBank(vramBank);
    vMemSetAddr(vramAddr);
    vMemSetIncMode(1);


    POKEW(0x2, VMEM_DATA_0);
    POKEW(0x4, length);
    POKEW(0x6, 0);

    __asm__("jsr $FEE4");
}

void clearFullVRAM(unsigned char vramBank) {
    clearVRAM(vramBank, 0, 32768L);
    clearVRAM(vramBank, 32768L, 32768L);
}

void copyBankedRAMToVRAM(unsigned char startMemBank, unsigned char vramBank, unsigned short vramAddr, unsigned long length, unsigned short startingImageAddr) {
    unsigned long remaining;
    unsigned short i;

    vMemSetBank(vramBank);
    vMemSetAddr(vramAddr);
    vMemSetIncMode(1);

    POKE(0, startMemBank);

    // This crazy routine uses the kernal memory_copy function to bulk copy RAM to VRAM
    // I had to increment the bank and do it in chunks though.
    for (i=0; i<length/8192+1; i++) {
        // Set the bank to read from
        POKE(0, 2+i);

        // void memory_copy(word source: r0, word target: r1, word num_bytes: r2);
        POKEW(0x2, (unsigned int)startingImageAddr);
        POKEW(0x4, 0x9F23);

        remaining = length - (8192*i);
        if (remaining < 8192) {
            POKEW(0x6, remaining);
        } else {
            POKEW(0x6, 8192);
        }
        __asm__("jsr $FEE7");
    }
}

void imageFileLoad(unsigned char startMemBank, unsigned char vramBank, unsigned short vramAddr, unsigned char *filename, unsigned long length)
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

    // This uses a kernal function which automatically switches to the next 8k Bank as they run out of space
    cbm_k_load(0, (unsigned int)BANK_RAM);

    // Copy the image from RAM to VRAM
    // TODO: This routine is already super fast BUT forum guys say you can load DIRECTLY to VRAM.
    /*
        This could stand to be documented clearly, but the X16 has extended the behavior of the LOAD kernal function.

        Normally, if the A register is 0, the kernal loads the file into system memory. Non-zero means the kernal should perform a verify instead.

        On the X16:
        If the A register is zero, the kernal loads into system memory.
        If the A register is 1, the kernal performs a verify.
        If the A register is 2, the kernal loads into VRAM, starting from $00000 + the specified starting address.
        If the A register is 3, the kernal loads into VRAM, starting from $10000 + the specified starting address.
    */
    copyBankedRAMToVRAM(startMemBank, vramBank, vramAddr, length, (unsigned short)BANK_RAM);
}