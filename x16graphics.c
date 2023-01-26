#include <peekpoke.h>
#include <6502.h>
#include "x16graphics.h"

struct MemSplit splitMem(unsigned short addr) {
    struct MemSplit ms;
    ms.low = addr & 255;
    ms.high = (addr>>8) & 255;

    return ms;
}

void vMemSetBank0() {
    POKE(VMEM_ADDR_HI, PEEK(VMEM_ADDR_HI) & 254);
}

void vMemSetBank1() {
    POKE(VMEM_ADDR_HI, PEEK(VMEM_ADDR_HI) | 1);
}

void vMemSetBank(unsigned char bank) {
    if (bank == 0) {
        vMemSetBank0();
    } else {
        vMemSetBank1();
    }
}

void vMemSetAddr(unsigned short mem) {
    struct MemSplit ms;
    ms = splitMem(mem);
    POKE(VMEM_ADDR_LO, ms.low);
    POKE(VMEM_ADDR_MED, ms.high);
}

void vMemSetIncMode(unsigned char mode) {
    unsigned char currentCleared = PEEK(VMEM_ADDR_HI) & VMEM_ADDR_INC_MASK;
    unsigned char newVal = (mode<<4) | currentCleared;
    POKE(VMEM_ADDR_HI, newVal);
}

void vMemSetData0(unsigned char val) {
    POKE(VMEM_DATA_0, val);
}

void vMemSetData1(unsigned char val) {
    POKE(VMEM_DATA_1, val);
}

void spriteSetGlobalOn() {
    POKE(VMEM_VIDEO, PEEK(VMEM_VIDEO) | 0b01000000);
}

void spriteSetGraphicsPointer(unsigned char use256ColorMode, unsigned char graphicsBank, unsigned short graphicsAddr) {
    // This is a little wonky but stay with me
    // Video Memory addresses at 17bits (1 for the "bank" and 16 for the address)
    // The Sprite "Pointer to sprite graphics" is only 12 bits
    // We basically chop off the bottom 5 bits of the address and use the 
    unsigned short shiftedAddr = graphicsAddr>>5;
    unsigned char addr0 = shiftedAddr & 255;
    unsigned char addr1 = shiftedAddr>>8 & 255;
    // unsigned short spritePtr = SPRITE_MEM_START + (spriteIdx * 8);
    unsigned char mode = use256ColorMode == 1 ? 128 : 0;
    unsigned char graphicsBankVal = graphicsBank == 1 ? 0b1000 : 0;

    POKE(VMEM_DATA_0, addr0); //POKE $1,$FC00,$00 sets the bits 5-12 of the pointer to graphics ($040000)
    
    // sets the bits 13-16 of the same pointer plus it sets the bit 7 to 1 to turn on 256 color sprite mode
    POKE(VMEM_DATA_0, addr1 | mode | graphicsBankVal);
}

void spriteIdxSetGraphicsPointer(unsigned char spriteBank, unsigned char spriteIdx,
    unsigned char use256ColorMode, unsigned char graphicsBank, unsigned short graphicsAddr) {
    vMemSetBank(spriteBank);
    vMemSetAddr(SPRITE_MEM_START + (spriteIdx*8));
    vMemSetIncMode(1);
    spriteSetGraphicsPointer(use256ColorMode, graphicsBank, graphicsAddr);
}

void spriteSetXY(unsigned short x, unsigned short y) {
    // Each position is spread across 2 registers
    // First one takes the 1st 8 bits
    // Second one takes the next 2 bits (hence the /256)
    vMemSetIncMode(1);
    POKE(VMEM_DATA_0, x & 0xFF);
    POKE(VMEM_DATA_0, x/256);
    POKE(VMEM_DATA_0, y & 0xFF);
    POKE(VMEM_DATA_0, y/256);
}

void spriteIdxSetXY(unsigned char spriteBank, unsigned char spriteIdx, unsigned short x, unsigned short y) {
    vMemSetBank(spriteBank);
    vMemSetAddr(SPRITE_MEM_START + (spriteIdx*8) + 2); // +2 to point to the X Position Offset
    spriteSetXY(x, y);
}

void spriteIdxSetZDepth(unsigned char spriteBank, unsigned char spriteIdx, enum ZDepth zDepth) {
    vMemSetBank(spriteBank);
    vMemSetAddr(SPRITE_MEM_START + (spriteIdx*8) + 6); // +6 to point to the byte with Z Depth
    vMemSetIncMode(0); // Needed because we PEEK to get the existing value as to not lose other bits
    POKE(VMEM_DATA_0, (PEEK(VMEM_DATA_0) & 0b11110011) | zDepth<<2);
}

void spriteIdxSetHFlip(unsigned char spriteBank, unsigned char spriteIdx, unsigned char hflip) {
    vMemSetBank(spriteBank);
    vMemSetAddr(SPRITE_MEM_START + (spriteIdx*8) + 6); // +6 to point to the byte with Z Depth
    vMemSetIncMode(0); // Needed because we PEEK to get the existing value as to not lose other bits
    POKE(VMEM_DATA_0, (PEEK(VMEM_DATA_0) & 0b11111110) | hflip);
}

void spriteSetZDepthAndCollisionMask(enum ZDepth zDepth, unsigned char collisionMask) {
    // Set together because they are in the same byte
    POKE(VMEM_DATA_0, zDepth<<2 | collisionMask<<4);
}

void spriteSetWidthHeight(enum SpriteSize width, enum SpriteSize height) {
    POKE(VMEM_DATA_0, height<<6 | width<<4);
}

void spriteInit(unsigned char spriteBank, unsigned char spriteIdx, 
    unsigned char use256ColorMode, unsigned char graphicsBank, unsigned short graphicsAddr,
    unsigned char collisionMask, enum ZDepth zDepth, enum SpriteSize width, enum SpriteSize height) {
    
    vMemSetBank(spriteBank);
    vMemSetAddr(SPRITE_MEM_START + (spriteIdx*8));
   
    // Most of these functions assume an Inc Mode of 1 since they are setting bytes
    // It will automatically move the address 1 byte so we can set the next field
    vMemSetIncMode(1);

    // Point our sprite at the graphics block
    // This also handles setting color color mode (usually 256)
    spriteSetGraphicsPointer(use256ColorMode, graphicsBank, graphicsAddr);

    // Set X/Y, ZDepth, and width/height
    spriteSetXY(0, 0);

    // Set together because they are in the same byte
    spriteSetZDepthAndCollisionMask(zDepth, collisionMask);
    spriteSetWidthHeight(width, height);
}

void spriteCollisionsEnable() {
    POKE(IEN_REG, PEEK(IEN_REG) | SPRCOL_MASK);
}

void spriteCollisionsDisable() {
    POKE(IEN_REG, PEEK(IEN_REG) & (255 - SPRCOL_MASK));
}

unsigned char spriteCollisionBitsGet() {
    // Get the Collision bits and shift them down
    return (PEEK(ISR_REG) & 0b11110000)>>4;
}

unsigned char spriteCollisionIRQHandler()
{
    // See if the SPRCOL flag (bit 2) is set on the IEN register
    unsigned char collision = PEEK(IEN_REG) & SPRCOL_MASK;
    
    if (collision > 0) {
        // Clear the collision IRQ by writing to the SPRCOL (bit 2) in the ISR
        // NOTE: It appears that ISR is special read-only in a way
        // We just write the bit and the other data seems to stay untouched
        POKE(ISR_REG,  SPRCOL_MASK); 
        return IRQ_HANDLED;
    }

    return IRQ_NOT_HANDLED;
}

unsigned char memoryToMapMemoryAddress(unsigned char bank, unsigned short mem) {
    unsigned char mapMem = bank << 7 | mem >> 9;
    return mapMem;
}

unsigned char tileBaseConfig(unsigned char bank, unsigned short mem, unsigned char height, unsigned char width) {
    unsigned char tileConfig = bank << 7 | ((mem >> 11) << 2) | height << 1 | width;
    return tileConfig;
}