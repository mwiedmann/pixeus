#include <stdio.h>
#include <stdlib.h>
#include <cbm.h>
#include <cx16.h>

#include "memmap.h"
#include "gametiles.h"
#include "x16graphics.h"
#include "levelutils.h"

unsigned char letterToTile(char letter) {
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
    } else if (letter == SCUBA_TILEID || letter == WEAPON_TILEID || letter == BOOTS_TILEID || letter == FLAME_TILEID || letter == SNOWFLAKE_TILEID) {
        // Special case to show some item icons
        // Not really part of the font but they exist as tiles and show with text sometimes (e.g. the header)
        return letter;
    } else {
        return BLACK_TILE;
    }

    return tile + FONT_TILEID_START;
}

/**
 * String length get with a cap of 40 (our max screen width)
*/
unsigned char textLengthGet(char* text) {
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

void drawTextRow(char* text, unsigned char* orig, unsigned char row, unsigned char col) {
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

void drawCenteredTextRow(char* text, unsigned char* orig, unsigned char row) {
    unsigned char col;
    unsigned char length;

    length = textLengthGet(text);
    col = (40 - length) / 2;
    drawTextRow(text, orig, row, col);
}

void drawGameHeader(unsigned short gold, unsigned char energy, unsigned char lives,
    unsigned short hasScuba, unsigned char hasWeapon, unsigned char hasBoots,
    unsigned char coldCount, unsigned char hotCount) {
    char text[41];
    char dangerText[5] = "    ";
    
    // Show the snowflake or flame tile with count if needed
    if (coldCount > 0 || hotCount > 0) {
        sprintf(
            dangerText,
            "%c%03u",coldCount > 0 ? SNOWFLAKE_TILEID : FLAME_TILEID,
            coldCount > 0 ? coldCount : hotCount
        );
    }

    sprintf(text, " GOLD:%04u ENERGY:%02u LIVES:%02u %s %c%c%c", gold, energy, lives, dangerText, 
        (hasScuba ? SCUBA_TILEID : '?'),
        (hasWeapon ? WEAPON_TILEID : '?'),
        (hasBoots ? BOOTS_TILEID : '?'));

    drawTextRow(text, 0, 0, 0);
}

void showMessage(char* text) {
    unsigned char i, length;
    unsigned char orig1[80], orig2[80], orig3[80];
    char line[41];

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
}

void loadTextFile(char *filename, unsigned char bank) {
    unsigned char prevBank = RAM_BANK;
    RAM_BANK = bank;

    cbm_k_setnam(filename);
    cbm_k_setlfs(0, 8, 2);
    cbm_k_load(0, (unsigned short)BANK_RAM);

    RAM_BANK = prevBank;
}

void drawTextFileFromBank(unsigned char bank, unsigned short argComp) {
    unsigned char prevBank = RAM_BANK;
    unsigned char i;
    unsigned char *length, *row, *col;
    char *text;
    unsigned short *arg;

    RAM_BANK = bank;

    // 1 byte header with length
    length = BANK_RAM;
    
    /*
        1 byte - row
        1 byte - col (255 = center)
        2 bytes - arg (used for gold amount compare)
        41 bytes ending with 0 - text
    */

    for (i=0; i<(*length); i++) {
        row = BANK_RAM + 1 + (45*i);
        col = BANK_RAM + 1 + (45*i) + 1;
        arg = (unsigned short *)(BANK_RAM + 1 + (45*i) + 2);
        text = (char *)(BANK_RAM + 1 + (45*i) + 4);
        // arg/argComp are used to optionally display text
        // We use it to show what Pixeus buys depending on how much gold
        if (argComp >= *arg) {
            // 255 means center the text
            if (*col != 255) {
                drawTextRow(text, 0, *row, *col);
            } else {
                drawCenteredTextRow(text, 0, *row);
            }
        }
    }

    RAM_BANK = prevBank;
}

void drawTextFile(char *filename, unsigned short argComp) {
    unsigned char currentMemBank;

    currentMemBank = RAM_BANK;

    loadTextFile(filename, IMAGE_LOAD_BANK);
    drawTextFileFromBank(IMAGE_LOAD_BANK, argComp);

    RAM_BANK =  currentMemBank;
}

void preloadTextFiles() {
    loadTextFile("welcome.bin", WELCOME_BANK);
    loadTextFile("instr.bin", INSTRUCTIONS_BANK);
}
