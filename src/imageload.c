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

    // This crazy routine uses the kernal memory_copy function to bulk copy RAM to VRAM
    // I had to increment the bank and do it in chunks though.
    for (i=0; i<length/8192+1; i++) {
        // Set the bank to read from
        RAM_BANK = startMemBank+i;

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

void loadFileToVRAM(char *filename, unsigned char vramBank, unsigned short vramAddr) {
    // These 3 functions are basic wrappers for the Kernal Functions

    // You have to first set the name of the file you are working with.
    cbm_k_setnam(filename);

    // Next you setup the LFS (Logical File) for the file
    // First param is the Logical File Number
    //   Use 0 if you are just loading the file
    //   You can use other values to keep multiple files open
    // Second param is the device number
    //   The SD Card on the CX16 is 8
    // The last param is the Secondary Address
    // 0 - File has the 2 byte header, but skip it
    // 1 - File has the 2 byte header, use it
    // 2 - File does NOT have the 2 byte header
    cbm_k_setlfs(0, 8, 0);

    // Finally, load the file somewhere into RAM or VRAM
    // First param of cbm_k_load means:
    //   0, loads into system memory.
    //   1, perform a verify.
    //   2, loads into VRAM, starting from 0x00000 + the specified starting address.
    //   3, loads into VRAM, starting from 0x10000 + the specified starting address.
    // Second param is the 16 bit address 
    cbm_k_load(vramBank ? 3 : 2, vramAddr);
}

unsigned long imageFileLoad(unsigned char startMemBank, unsigned char vramBank, unsigned short vramAddr, char *filename)
{
    unsigned char endingMemBank;
    unsigned short finalmem;
    unsigned long length;
    unsigned char currentMemBank;

    currentMemBank = RAM_BANK;

    // The memory bank to start loading data into.
    // The bank will auto-jump to the next bank when this bank is full (pretty neat).
    RAM_BANK = startMemBank;

    // Note about the files
    // The first 2 bytes are ignored in this mode so you have to pad your images with 2 leading bytes
    // Those bytes can be used as an address to load the contents into rather than specifying the address
    // NOTE: BANK_RAM is the starting addr of Banked RAM. Its part of cx16.h, good to use in case it changes
    cbm_k_setnam(filename);
    cbm_k_setlfs(0, 8, 0);

    // This uses a kernal function which automatically switches to the next 8k Bank as they run out of space
    finalmem = cbm_k_load(0, (unsigned short)BANK_RAM);

    // The load rolls through banks, so get the ending bank and calc the total bytes read
    endingMemBank = PEEK(0);
    length = ((endingMemBank - startMemBank) * 8192UL) + (finalmem - (unsigned int)BANK_RAM);

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

    // Restore the previous mem bank
    RAM_BANK = currentMemBank;

    return length;
}