#include <stdio.h>
#include <conio.h>
#include <peekpoke.h>
#include "gametiles.h"

#define WELCOME_SPACER "  "

void welcomeStart() {
    clrscr();

    loadFont();
   
    POKE(0x9F2A, 64);
    POKE(0x9F2B, 64);

    printf("\n\n%swelcome to \"pixeus\" v0.0.1\n", WELCOME_SPACER);
    printf("%sby mark wiedmann\n\n", WELCOME_SPACER);
    printf("%sloading tiles\n", WELCOME_SPACER);
    printf("%sthis will take a moment...\n", WELCOME_SPACER);
}

void welcomeTiles() {
    printf("%stiles loaded\n", WELCOME_SPACER);
    printf("%sconfiguring tiles...\n", WELCOME_SPACER);
}

void welcomeMaps() {
    printf("%stiles done\n", WELCOME_SPACER);
    printf("%sloading sprites...\n", WELCOME_SPACER);
}

void welcomeSprites() {
    printf("%ssprites done\n", WELCOME_SPACER);
    printf("%sgame starting...\n", WELCOME_SPACER);
}

void welcomeEnd() {
    POKE(0x9F2A, 128);
    POKE(0x9F2B, 128);
}