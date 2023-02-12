#include <stdio.h>

#include "memmap.h"
#include "gametiles.h"
#include "x16graphics.h"

unsigned char letterToTile(unsigned char letter) {
    unsigned char tile;

    if (letter >= 193 && letter <= 218) {
        tile = letter - 193;
    } else if (letter >= 48 && letter <= 57) {
        tile = letter - 22;
    } else if (letter == 58) {
        tile = 36;
    } else if (letter == 46) {
        tile = 37;
    } else if (letter == 63) {
        tile = 38;
    } else if (letter == 45) {
        tile = 39;
    } else if (letter == 33) {
        tile = 40;
    } else if (letter == SCUBA_TILEID || letter == WEAPON_TILEID || letter == BOOTS_TILEID) {
        // Special case to show some item icons
        // Not really part of the font but they exist as tiles and show with text sometimes (e.g. the header)
        return letter;
    } else {
        return 0;
    }

    return tile + FONT_TILEID_START;
}

void drawTextRow(unsigned char* text, unsigned char length, unsigned char row, unsigned char col) {
    unsigned char i;

    vMemSetBank(LAYER0_MAP_MEM_BANK);
    vMemSetAddr(LAYER0_MAP_MEM + (row * TILES_ACROSS * 2) + (col * 2));
    for (i=0; i<length; i++) {
        vMemSetData0(letterToTile(text[i]));
        vMemSetData0(0);
    }
}

void drawGameHeader(unsigned short gold, unsigned char energy, unsigned char lives,
    unsigned short hasScuba, unsigned char hasWeapon, unsigned char hasBoots) {
    unsigned char text[40];

    sprintf(text, " GOLD:%05u  ENERGY:%03u  LIVES:%02u  %c%c%c", gold, energy, lives, 
        (hasScuba ? SCUBA_TILEID : '?'),
        (hasWeapon ? WEAPON_TILEID : '?'),
        (hasBoots ? BOOTS_TILEID : '?'));

    drawTextRow(text, 40, 0, 0);
}