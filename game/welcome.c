#include <stdio.h>
#include <conio.h>
#include <peekpoke.h>

#define WELCOME_SPACER "  "

void welcomeStart() {
    clrscr();
    POKE(0x9F2A, 64);
    POKE(0x9F2B, 64);

    printf("\n\n%sWELCOME TO: \"PIXEUS\" V0.0.1\n", WELCOME_SPACER);
    printf("%sBY MARK WIEDMANN\n\n", WELCOME_SPACER);
    printf("%sLOADING TILES\n", WELCOME_SPACER);
    printf("%sTHIS WILL TAKE A MOMENT...\n", WELCOME_SPACER);
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

void welcomeEnd() {
    POKE(0x9F2A, 128);
    POKE(0x9F2B, 128);
}