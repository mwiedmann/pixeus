#include <peekpoke.h>
#include <6502.h>
#include <cx16.h>
#include <joystick.h>
#include "x16graphics.h"
#include "waitforjiffy.h"

// Not sure how big the stack needs to be. Unclear how this works.
#define IRQ_HANDLER_STACK_SIZE 8
unsigned char IRQHandlerStack[IRQ_HANDLER_STACK_SIZE];

void main() {
    unsigned short x;
    unsigned short y;
    unsigned char d;
    unsigned short spriteGraphicsAddr = 0x4000;
    unsigned char collision;
    unsigned char joy;

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
    set_irq(&spriteCollisionIRQHandler, IRQHandlerStack, IRQ_HANDLER_STACK_SIZE);
    spriteCollisionsEnable();

    // Let's do a sprite collision test
    spriteInit(1, 0, 1, 0, spriteGraphicsAddr, 3, InFrontOfL1, PX16, PX16);
    spriteInit(1, 1, 1, 0, spriteGraphicsAddr, 6, InFrontOfL1, PX16, PX16);

    spriteIdxSetXY(1, 0, 320, 240);

    x = 380;
    y = 235;
    spriteIdxSetXY(1, 1, x, y);

    joy_install(cx16_std_joy);
    
    while (1) {
        waitforjiffy();
        
        joy = joy_read(0);
        if (JOY_UP(joy)) {
            y--;
        } else if (JOY_DOWN(joy)) {
            y++;
        }

        if (JOY_LEFT(joy)) {
            x--;
        } else if (JOY_RIGHT(joy)) {
            x++;
        }

        if (JOY_BTN_1(joy)) {
            break;
        }

        spriteIdxSetXY(1, 1, x, y);

        // Get the Collision bits and shift them down
        collision = spriteCollisionBitsGet();

        // The collision bits will be the OVERLAP of the Collision Masks of the two sprites.
        // We can then look at all the sprites that have that bit in their mask
        if (collision == 2) {
            // Move the sprite back
            // This will cause repeated collisions
            // Want to make sure that works
            x = 380;
            y = 235;
            spriteIdxSetXY(1, 1, x, y);
        }
    }

    // Disable sprite collisions before quitting
    // or the UI hangs if sprites are still touching.
    spriteCollisionsDisable();
}
