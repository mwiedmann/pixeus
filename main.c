#include <peekpoke.h>
#include <6502.h>
#include "x16graphics.h"
#include "waitforjiffy.h"

#define STACK_SIZE 8
unsigned char TempStack[STACK_SIZE];

// IRQ handler
unsigned char spriteCollisionIRQHandler(void)
{
    // See if the SPRCOL flag (bit 2) is set on the IEN register
    unsigned char collision = PEEK(0x9F26) & 0b100;
    
    if (collision > 0) {
        // Clear the collision IRQ by writing to the SPRCOL (bit 2) in the ISR
        // NOTE: It appears that ISR is special read-only in a way
        // We just write the bit and the other data seems to stay untouched
        POKE(0x9F27,  4); 
        return IRQ_HANDLED;
    }

    return IRQ_NOT_HANDLED;
}

void main() {
    unsigned short x;
    unsigned short y;
    unsigned char d;
    unsigned short spriteGraphicsAddr = 0x4000;
    unsigned char collision;
    unsigned char count;

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

    // Setup the IRQ handler for sprite collisions and enable global sprite collisions
    set_irq(&spriteCollisionIRQHandler, TempStack, STACK_SIZE);
    spriteCollisionsEnable();

    // Let's do a sprite collision test
    spriteInit(1, 0, 1, 0, spriteGraphicsAddr, 3, InFrontOfL1, PX16, PX16);
    spriteInit(1, 1, 1, 0, spriteGraphicsAddr, 6, InFrontOfL1, PX16, PX16);

    spriteIdxSetXY(1, 0, 320, 240);
    spriteIdxSetXY(1, 1, 380, 235);

    // Collide 5 times then quit
    count = 5;

    for (x=380; x>0; x--) {
        waitforjiffy();
        spriteIdxSetXY(1, 1, x, 235);

        // Get the Collision bits and shift them down
        collision = spriteCollisionBitsGet();

        // The collision bits will be the OVERLAP of the Collision Masks of the two sprites.
        // We can then look at all the sprites that have that bit in their mask
        if (collision == 2) {
            // Quit after a certain number of collisions
            count--;
            if (count == 0) {
                break;
            }

            // Move the sprite back
            // This will cause repeated collisions
            // Want to make sure that works
            x = 380;
            spriteIdxSetXY(1, 1, x, 235);
        }
    }

    // Disable sprite collisions before quitting
    // or the UI hangs if sprites are still touching.
    // POKE(0x9F26, 1);
    spriteCollisionsDisable();
}

// To build and run
/*
cl65 -o test.prg -t cx16 x16graphics.c main.c waitforjiffy.s
../emu/x16emu -prg test.prg -run
*/