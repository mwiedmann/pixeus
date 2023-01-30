#include <stdio.h>
#include <conio.h>

#define WELCOME_SPACER "  "

void welcomeStart() {
    clrscr();
    printf("\n\n%sWELCOME TO: \"UNAMED GAME\" V0.0.1\n", WELCOME_SPACER);
    printf("%sBY MARK WIEDMANN\n\n", WELCOME_SPACER);
    printf("%sLOADING TILES...\n", WELCOME_SPACER);
}

void welcomeTiles() {
    printf("%sTILES LOADED\n", WELCOME_SPACER);
    printf("%sCONFIGURING TILES...\n", WELCOME_SPACER);
}

void welcomeMaps() {
    printf("%sTILES DONE\n", WELCOME_SPACER);
    printf("%sLOADING SPRITES...\n", WELCOME_SPACER);
}

void welcomeSprites() {
    printf("%SPRITES DONE\n", WELCOME_SPACER);
    printf("%sGAME STARTING...\n", WELCOME_SPACER);
}