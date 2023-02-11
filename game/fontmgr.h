#ifndef FONTMGR_H
#define FONTMGR_H

unsigned char letterToTile(unsigned char letter);
void drawTextRow(unsigned char* text, unsigned char length, unsigned char row, unsigned char col);
void drawGameHeader(unsigned short gold, unsigned char energy, unsigned char lives);

#endif