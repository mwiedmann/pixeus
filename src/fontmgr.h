#ifndef FONTMGR_H
#define FONTMGR_H

/**
 * Convert a char to a Tile ID
*/
unsigned char letterToTile(char letter);

/**
 * Get a list of the Tile IDs for a screen row.
 * Used so we can restore them after displaying a message (which wipes them out)
*/
void tileRowGet(unsigned char *orig, unsigned char row);

/**
 * Set a row of Tile IDs. Used to restore a row after displaying over it.
*/
void tileRowSet(unsigned char *orig, unsigned char row);

/**
 * Draw text to the screen starting at a row and col.
 * Optional pointer to a buffer to save the current Tile IDs that will be overwritten.
*/
void drawTextRow(char* text, unsigned char* orig, unsigned char row, unsigned char col);

/**
 * Draw text to the screen centered in a row.
 * Optional pointer to a buffer to save the current Tile IDs that will be overwritten.
*/
void drawCenteredTextRow(char* text, unsigned char* orig, unsigned char row);

/**
 * Draw the top row game header
*/
void drawGameHeader(unsigned short gold, unsigned char energy, unsigned char lives,
    unsigned short hasScuba, unsigned char hasWeapon, unsigned char hasBoots,
    unsigned char coldCount, unsigned char hotCount);

/**
 * Show a centered (row and col) text message. It is dismissed with a button press.
 * It will restore the overwritten tiles when dismissed.
*/
void showMessage(char* text);

/**
 * Load a "text file" to Banked RAM
*/
void loadTextFile(char *filename, unsigned char bank);

/**
 * Draw a previously loaded "text file".
 * Text files take an arg that can control which lines are printed
 * (value in text file line must be >= argComp)
*/
void drawTextFileFromBank(unsigned char bank, unsigned short argComp);

/**
 * Load and draw a "text file". See "drawTextFileFromBank" for more info.
*/
void drawTextFile(char *filename, unsigned short argComp);

/**
 * Preload a few common "text files"
*/
void preloadTextFiles();

#endif