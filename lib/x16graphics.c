#include <peekpoke.h>
#include <6502.h>
#include <cx16.h>
#include "x16graphics.h"

unsigned char pixelSizes[4] = {8, 16, 32, 64};

struct MemSplit splitMem(unsigned short addr) {
    struct MemSplit ms;
    ms.low = addr & 255;
    ms.high = (addr>>8) & 255;

    return ms;
}

void vMemSetBank0() {
    VERA.address_hi = VERA.address_hi & 254;
}

void vMemSetBank1() {
    VERA.address_hi = VERA.address_hi | 1;
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
    unsigned char currentCleared = VERA.address_hi & VMEM_ADDR_INC_MASK;
    unsigned char newVal = (mode<<4) | currentCleared;
    VERA.address_hi = newVal;
}

unsigned char vMemGetData0() {
    return VERA.data0;
}

unsigned char vMemGetData1() {
    return VERA.data1;
}

void vMemSetData0(unsigned char val) {
    VERA.data0 = val;
}

void vMemSetData1(unsigned char val) {
    VERA.data1 = val;
}

void x16SpriteSetGlobalOn() {
    VERA.display.video = VERA.display.video | 0b01000000;
}

void x16SpriteSetGraphicsPointer(unsigned char use256ColorMode, unsigned char graphicsBank, unsigned short graphicsAddr) {
    // This is a little wonky but stay with me
    // Video Memory addresses at 17bits (1 for the "bank" and 16 for the address)
    // The Sprite "Pointer to sprite graphics" is only 12 bits
    // We basically chop off the bottom 5 bits of the address and use the 
    unsigned short shiftedAddr = graphicsAddr>>5;
    unsigned char addr0 = shiftedAddr & 255;
    unsigned char addr1 = shiftedAddr>>8 & 255;
    // unsigned short spritePtr = SPRITE_ATTRIBUTES_START + (spriteIdx * 8);
    unsigned char mode = use256ColorMode == 1 ? 128 : 0;
    unsigned char graphicsBankVal = graphicsBank == 1 ? 0b1000 : 0;

    VERA.data0 = addr0; //POKE $1,$FC00,$00 sets the bits 5-12 of the pointer to graphics ($040000)
    
    // sets the bits 13-16 of the same pointer plus it sets the bit 7 to 1 to turn on 256 color sprite mode
    VERA.data0 = addr1 | mode | graphicsBankVal;
}

void x16SpriteIdxSetGraphicsPointer(unsigned char spriteIdx,
    unsigned char use256ColorMode, unsigned char graphicsBank, unsigned short graphicsAddr) {
    vMemSetBank(1);
    vMemSetAddr(SPRITE_ATTRIBUTES_START + (spriteIdx*8));
    vMemSetIncMode(1);
    x16SpriteSetGraphicsPointer(use256ColorMode, graphicsBank, graphicsAddr);
}

void x16SpriteSetXY(unsigned short x, unsigned short y) {
    // Each position is spread across 2 registers
    // First one takes the 1st 8 bits
    // Second one takes the next 2 bits (hence the /256)
    vMemSetIncMode(1);
    VERA.data0 = x & 0xFF;
    VERA.data0 = x/256;
    VERA.data0 = y & 0xFF;
    VERA.data0 = y/256;
}

void x16SpriteIdxSetXY(unsigned char spriteIdx, unsigned short x, unsigned short y) {
    vMemSetBank(1);
    vMemSetAddr(SPRITE_ATTRIBUTES_START + (spriteIdx*8) + 2); // +2 to point to the X Position Offset
    x16SpriteSetXY(x, y);
}

void x16SpriteIdxSetZDepth(unsigned char spriteIdx, ZDepth zDepth) {
    vMemSetBank(1);
    vMemSetAddr(SPRITE_ATTRIBUTES_START + (spriteIdx*8) + 6); // +6 to point to the byte with Z Depth
    vMemSetIncMode(0); // Needed because we PEEK to get the existing value as to not lose other bits
    VERA.data0 = (VERA.data0 & 0b11110011) | zDepth<<2;
}

void x16SpriteIdxSetHFlip(unsigned char spriteIdx, unsigned char hflip) {
    vMemSetBank(1);
    vMemSetAddr(SPRITE_ATTRIBUTES_START + (spriteIdx*8) + 6); // +6 to point to the byte with Z Depth
    vMemSetIncMode(0); // Needed because we PEEK to get the existing value as to not lose other bits
    VERA.data0 = (VERA.data0 & 0b11111110) | hflip;
}

void x16SpriteSetZDepthAndCollisionMask(ZDepth zDepth, unsigned char collisionMask) {
    // Set together because they are in the same byte
    VERA.data0 = zDepth<<2 | collisionMask<<4;
}

void x16SpriteSetWidthHeight(SpriteSize width, SpriteSize height) {
    VERA.data0 = height<<6 | width<<4;
}

void x16SpriteInit(unsigned char spriteIdx, 
    unsigned char use256ColorMode, unsigned char graphicsBank, unsigned short graphicsAddr,
    unsigned char collisionMask, ZDepth zDepth, SpriteSize width, SpriteSize height) {
    
    vMemSetBank(1);
    vMemSetAddr(SPRITE_ATTRIBUTES_START + (spriteIdx*8));
   
    // Most of these functions assume an Inc Mode of 1 since they are setting bytes
    // It will automatically move the address 1 byte so we can set the next field
    vMemSetIncMode(1);

    // Point our sprite at the graphics block
    // This also handles setting color color mode (usually 256)
    x16SpriteSetGraphicsPointer(use256ColorMode, graphicsBank, graphicsAddr);

    // Set X/Y, ZDepth, and width/height
    x16SpriteSetXY(0, 0);

    // Set together because they are in the same byte
    x16SpriteSetZDepthAndCollisionMask(zDepth, collisionMask);
    x16SpriteSetWidthHeight(width, height);
}

void x16SpriteCollisionsEnable() {
    VERA.irq_enable = VERA.irq_enable | SPRCOL_MASK;
}

void x16SpriteCollisionsDisable() {
    VERA.irq_enable = VERA.irq_enable & (255 - SPRCOL_MASK);
}

unsigned char x16SpriteCollisionBitsGet() {
    // Get the Collision bits and shift them down
    return (VERA.irq_flags & 0b11110000)>>4;
}

unsigned char x16SpriteCollisionIRQHandler()
{
    // See if the SPRCOL flag (bit 2) is set on the IEN register
    unsigned char collision = PEEK(IEN_REG) & SPRCOL_MASK;
    
    if (collision > 0) {
        // Clear the collision IRQ by writing to the SPRCOL (bit 2) in the ISR
        // NOTE: It appears that ISR is special read-only in a way
        // We just write the bit and the other data seems to stay untouched
        VERA.irq_flags = SPRCOL_MASK; 
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