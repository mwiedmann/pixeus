#include <cx16.h>
#include <cbm.h>
#include <stdio.h>
#include <stdlib.h>
#include <peekpoke.h>
#include "level.h"

void main() {
    unsigned char r;
    int i;
    int a=0;

    for (i=0; i<10000; i++) {
        // if (rand() & 1 == 1) {
        //     a++;
        // }
        r = rand();
        if (r > 127) {
            a++;
        }
    }

    printf("%u", a);
}

// cl65 -o testload.prg -t cx16 testload.c
// ../../emu/x16emu -prg testload.prg -run

// 10 for i=0 to 15 : print peek($a000+i) : next i