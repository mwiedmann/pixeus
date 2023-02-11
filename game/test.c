#include <cx16.h>
#include <cbm.h>
#include <stdio.h>
#include <stdlib.h>
#include <peekpoke.h>
#include "level.h"

void main() {
    unsigned char letter = '!';

    printf("%u", letter);
}

// cl65 -o testload.prg -t cx16 testload.c
// ../../emu/x16emu -prg testload.prg -run

// 10 for i=0 to 15 : print peek($a000+i) : next i