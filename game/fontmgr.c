#include <stdio.h>
#include <stdlib.h>

#include "memmap.h"
#include "gametiles.h"
#include "x16graphics.h"
#include "levelutils.h"

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
    } else if (letter == 39) {
        tile = 41;
    } else if (letter == SCUBA_TILEID || letter == WEAPON_TILEID || letter == BOOTS_TILEID) {
        // Special case to show some item icons
        // Not really part of the font but they exist as tiles and show with text sometimes (e.g. the header)
        return letter;
    } else {
        return 0;
    }

    return tile + FONT_TILEID_START;
}

unsigned char textLengthGet(unsigned char* text) {
    unsigned char i;

    for (i=0; i<40; i++) {
        // Stop if a null terminator is hit
        if (text[i] == 0) {
            return i;
        }
    }
}

void tileRowGet(unsigned char *orig, unsigned char row){
    unsigned char i;

    vMemSetIncMode(1);
    vMemSetBank(LAYER1_MAP_MEM_BANK);
    vMemSetAddr(LAYER1_MAP_MEM + (row * TILES_ACROSS * 2));

    for (i=0; i<80; i++) {
        orig[i] = vMemGetData0();
    }
}

void tileRowSet(unsigned char *orig, unsigned char row) {
    unsigned char i;

    if (orig == 0) {
        return;
    }

    vMemSetIncMode(1);
    vMemSetBank(LAYER1_MAP_MEM_BANK);
    vMemSetAddr(LAYER1_MAP_MEM + (row * TILES_ACROSS * 2));

    for (i=0; i<80; i++) {
         vMemSetData0(orig[i]);
    }
}

void drawTextRow(unsigned char* text, unsigned char* orig, unsigned char row, unsigned char col) {
    unsigned char i;
    
    if (orig != 0) {
      tileRowGet(orig, row);
    }

    vMemSetIncMode(1);
    vMemSetBank(LAYER1_MAP_MEM_BANK);
    vMemSetAddr(LAYER1_MAP_MEM + (row * TILES_ACROSS * 2) + (col * 2));

    for (i=0; i<40; i++) {
        // Stop if a null terminator is hit
        if (text[i] == 0) {
            return;
        }
        vMemSetData0(letterToTile(text[i]));
        vMemSetData0(0);
    }
}

void drawCenteredTextRow(unsigned char* text, unsigned char* orig, unsigned char row) {
    unsigned char col;
    unsigned char length;

    length = textLengthGet(text);
    col = (40 - length) / 2;
    drawTextRow(text, orig, row, col);
}

void drawGameHeader(unsigned short gold, unsigned char energy, unsigned char lives,
    unsigned short hasScuba, unsigned char hasWeapon, unsigned char hasBoots) {
    unsigned char text[41];

    sprintf(text, " GOLD:%05u  ENERGY:%03u  LIVES:%02u  %c%c%c", gold, energy, lives, 
        (hasScuba ? SCUBA_TILEID : '?'),
        (hasWeapon ? WEAPON_TILEID : '?'),
        (hasBoots ? BOOTS_TILEID : '?'));

    drawTextRow(text, 0, 0, 0);
}

void showMessage(unsigned char* text) {
    unsigned char i, length;
    unsigned char *orig1, *orig2, *orig3;
    unsigned char line[41];

    orig1 = malloc(80);
    orig2 = malloc(80);
    orig3 = malloc(80);

    length = textLengthGet(text);
    sprintf(line, "- %s -", text);

    drawCenteredTextRow(line, orig1, 14);

    for (i=0; i<length+4; i++) {
        line[i] = '-';
    }
    line[i] = 0;

    drawCenteredTextRow(line, orig2, 13);
    drawCenteredTextRow(line, orig3, 15);

    // TODO: Maybe just wait for a certain amount of time?
    waitForButtonPress();

    tileRowSet(orig1, 14);
    tileRowSet(orig2, 13);
    tileRowSet(orig3, 15);

    free(orig1);
    free(orig2);
    free(orig3);
}