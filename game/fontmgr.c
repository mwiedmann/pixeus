#include <stdio.h>

#include "memmap.h"
#include "x16graphics.h"

unsigned char letterToTile(unsigned char letter) {
    unsigned char tile;

    if (letter >= 193 && letter <= 218) {
        tile = letter - 192;
    } else if (letter >= 48 && letter <= 57) {
        tile = letter - 21;
    } else if (letter == 58) {
        tile = 37;
    } else if (letter == 46) {
        tile = 38;
    } else if (letter == 63) {
        tile = 39;
    } else if (letter == 45) {
        tile = 40;
    } else {
        tile = 0;
    }

    return tile;
}

void drawGameHeader(unsigned short score, unsigned char energy, unsigned char lives) {
    unsigned char i;
    unsigned char text[40];

    sprintf(text, "SCORE:%06u   ENERGY:%03u       LIVES:%u", score, energy, lives);

    vMemSetBank(LAYER0_MAP_MEM_BANK);
    vMemSetAddr(LAYER0_MAP_MEM);
    for (i=0; i<40; i++) {
        vMemSetData0(letterToTile(text[i]));
        vMemSetData0(0);
    }
}