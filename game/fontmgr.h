#ifndef FONTMGR_H
#define FONTMGR_H

unsigned char letterToTile(unsigned char letter);
void tileRowGet(unsigned char *orig, unsigned char row);
void tileRowSet(unsigned char *orig, unsigned char row) ;
void drawTextRow(unsigned char* text, unsigned char* orig, unsigned char row, unsigned char col) ;
void drawCenteredTextRow(unsigned char* text, unsigned char* orig, unsigned char row);
void drawGameHeader(unsigned short gold, unsigned char energy, unsigned char lives,
    unsigned short hasScuba, unsigned char hasWeapon, unsigned char hasBoots,
    unsigned char coldCount, unsigned char hotCount);
void showMessage(unsigned char* text);
void drawTextFile(unsigned char *filename, unsigned short argComp);
void drawTextFileFromBank(unsigned char bank, unsigned short argComp);
void loadTextFile(unsigned char *filename, unsigned char bank);
void debugMsg(unsigned char* text, unsigned char val);
void preloadTextFiles();

#endif