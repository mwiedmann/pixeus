#include <cx16.h>
#include <peekpoke.h>
#include <cbm.h>
#include "x16graphics.h"

#define ROM_START 0xC000
#define BANK_SIZE 8192

#pragma code-name (push, "BANKRAM01")

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

unsigned long loadFileToVRAM(char *filename, unsigned char vramBank, unsigned short vramAddr) {
    unsigned short finalmem;
    unsigned long length;

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
    cbm_k_setlfs(0, 8, 2);

    // Finally, load the file somewhere into RAM or VRAM
    // First param of cbm_k_load means:
    //   0, loads into system memory.
    //   1, perform a verify.
    //   2, loads into VRAM, starting from 0x00000 + the specified starting address.
    //   3, loads into VRAM, starting from 0x10000 + the specified starting address.
    // Second param is the 16 bit address 
    finalmem = cbm_k_load(vramBank ? 3 : 2, vramAddr);

    length = finalmem - vramAddr;

    return length;
}

#pragma code-name (pop)
