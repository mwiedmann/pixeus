#ifndef X16_GRAPHICS_H
#define X16_GRAPHICS_H

#define VMEM_ADDR_LO 0x9F20
#define VMEM_ADDR_MED 0x9F21
#define VMEM_ADDR_HI 0x9F22

#define VMEM_DATA_0 0x9F23
#define VMEM_DATA_1 0x9F24
#define VMEM_VIDEO 0x9F29

#define IEN_REG 0x9F26
#define ISR_REG 0x9F27

#define VMEM_ADDR_INC_MASK 0b00001111
#define VMEM_SPRITE_ENABLE_MASK 0b10111111
#define SPRITE_MEM_START 0xFC00
#define SPRCOL_MASK 4

#define LAYER_0_CONFIG 0x9F2D
#define LAYER_0_MAPBASE 0x9F2E
#define LAYER_0_TILEBASE 0x9F2F

#define LAYER_1_CONFIG 0x9F34
#define LAYER_1_MAPBASE 0x9F35
#define LAYER_1_TILEBASE 0x9F36

typedef enum { Disabled = 0, BetweenBackL0 = 1, BetweenL0L1 = 2, InFrontOfL1 = 3 } ZDepth;
typedef enum { PX8 = 0, PX16 = 1, PX32 = 2, PX64 = 3 } SpriteSize;

unsigned char pixelSizes[];

struct MemSplit {
    unsigned char low; 
    unsigned char high;
};

/**
 * Split a 16 bit address into hi/lo a pair
*/
struct MemSplit splitMem(unsigned short addr);

/**
 * Set the Video Memory bank to 0
 * "Address Hi" 0x9F22 bit 1
*/
void vMemSetBank0();

/**
 * Set the Video Memory bank to 1
 * "Address Hi" 0x9F22 bit 1
*/
void vMemSetBank1();

/**
 * Set the Video Memory bank to 1 or 0
 * "Address Hi" 0x9F22 bit 1
*/
void vMemSetBank(unsigned char bank);

/**
 * Set the Video Memory Address
 * Bytes "Address Lo" 0x9F20 and "Address Med" 0x9F21
*/
void vMemSetAddr(unsigned short mem);

/**
 * Set the Video Memory Increment mode
 * "Address Hi" 0x9F22 bits 4-7
*/
void vMemSetIncMode(unsigned char mode);

/**
 * Set the "Data 0" $9F23 register
 * This will set the value into the current Video Memory address 
*/
void vMemSetData0(unsigned char val);

/**
 * Set the "Data 1" $9F24 register
 * This will set the value into the current Video Memory address 
*/
void vMemSetData1(unsigned char val);

/**
 * Turn on the global sprites flag
 * Sets the "Video" $9F29 bit 6 to 1
*/
void x16SpriteSetGlobalOn();

/**
 * Sets the graphic address on a sprite
 * Assumes the bank and sprite address have already been set (see vMemSetBank and vMemSetAddr)
 * and Inc Mode is 1 (1 byte)
*/
void x16SpriteSetGraphicsPointer(unsigned char use256ColorMode, unsigned char graphicsBank, unsigned short graphicsAddr);

/**
 * Sets the graphic address on a sprite
*/
void x16SpriteIdxSetGraphicsPointer(unsigned char spriteBank, unsigned char spriteIdx,
    unsigned char use256ColorMode, unsigned char graphicsBank, unsigned short graphicsAddr);

/**
 * Sets the X,Y values on a sprite
 * Assumes the Video Memory address is pointing at the correct sprite X Position (Offset 2)
 * and Inc Mode is 1 (1 byte)
*/
void x16SpriteSetXY(unsigned short x, unsigned short y);

/**
 * Sets the X,Y values on a sprite
*/
void x16SpriteIdxSetXY(unsigned char spriteBank, unsigned char spriteIdx, unsigned short x, unsigned short y);

/**
 * Sets the Z-depth on a sprite
*/
void x16SpriteIdxSetZDepth(unsigned char spriteBank, unsigned char spriteIdx, ZDepth zDepth);

/**
 * Set the H-Flip on a sprite
*/
void x16SpriteIdxSetHFlip(unsigned char spriteBank, unsigned char spriteIdx, unsigned char hflip);

/**
 * Sets the Z-depth and collision mask on a sprite (they share the same byte)
 * Assumes the Video Memory address is pointing at the correct sprite ZDepth position (Offset 6)
*/
void x16SpriteSetZDepthAndCollisionMask(ZDepth zDepth, unsigned char collisionMask);

/**
 * Sets the width and height on a sprite
 * Assumes the Video Memory address is pointing at the correct sprite Width/Height position (Offset 7)
*/
void x16SpriteSetWidthHeight(SpriteSize width, SpriteSize height);

/**
 * Initialize a sprite.
 * Includes assigning the graphics and setting most of the required fields
 * like color mode, zDepth, width, height
*/
void x16SpriteInit(unsigned char spriteBank, unsigned char spriteIdx, 
    unsigned char use256ColorMode, unsigned char graphicsBank, unsigned short graphicsAddr,
    unsigned char collisionMask, ZDepth zDepth, SpriteSize width, SpriteSize height);

/**
 * Enable global sprite collision detection
*/
void x16SpriteCollisionsEnable();

/**
 * Disable global sprite collision detection
*/
void x16SpriteCollisionsDisable();

/**
 * Get the sprite collision bits (shifted down)
 * These give the overlap of which collision masks collided.
*/
unsigned char x16SpriteCollisionBitsGet();

/**
 * Use this function as the "irq_handler f" param for "set_irq" (6502.h) to handle IRQ for collisions.
 * It will handle the IRQ when there is a collision and clear the flag
 * You can check the collision bits (spriteCollisionBitsGet) in your game loop to see if there were collisions.
 * Make sure to enable collisions (spriteCollisionsEnable).
 * Make sure to disable collisions (spriteCollisionsDisable) on program exit.
*/
unsigned char x16SpriteCollisionIRQHandler();

/**
 * Convert a bank and memory address into a Map memory address.
 * Map memory uses bytes 9-16 (with 16 being the bank).
 * This means you can only address in 512 byte increments.
*/
unsigned char memoryToMapMemoryAddress(unsigned char bank, unsigned short mem);

/**
 * Convert a bank and memory address into a tile memory address.
 * Also add the heigh/width bits
*/
unsigned char tileBaseConfig(unsigned char bank, unsigned short mem, unsigned char height, unsigned char width);

#endif