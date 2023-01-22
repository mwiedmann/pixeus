#include <peekpoke.h>
#include "x16graphics.h"
#include "waitforjiffy.h"


void main() {
    unsigned short x;
    unsigned short y;
    unsigned char d;
    unsigned short spriteGraphicsAddr = 0x4000;

    // You have to set the address you want to write to in VMEM using 9F20/21
    // 9F22 controls how much the VMEM address increments after each read/write
    // Then you can peek or poke using 0x9F23
    // The address is auto incremented and you can peek/poke again
    vMemSetBank0();
    vMemSetAddr(spriteGraphicsAddr);
    vMemSetIncMode(1);
    
    // Poke a block of solid data into vmem
    // use as sprite data
    for (d=0; d<255; d++) {
        vMemSetData0(55);
    }

    // Turn on sprites
    spriteSetGlobalOn();

    spriteInit(1, 0, 1, 0, spriteGraphicsAddr, InFrontOfL1, PX16, PX16);

    for (y=0; y<479; y+=8) {
        for (x=0; x<639; x+=8) {
            waitforjiffy();
            // Set new video address
            POKE(0x9F20, 0x02);
            POKE(0x9F21, 0xFC);

            POKE(0x9F23, x & 0xFF);
            POKE(0x9F23, x/256);
            POKE(0x9F23, y & 0xFF);
            POKE(0x9F23, y/256);
        }
    }
}

// To build and run
/*
cl65 -o test.prg -t cx16 x16graphics.c main.c waitforjiffy.s
../emu/x16emu -prg test.prg -run
*/