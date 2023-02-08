#include <cx16.h>
#include <cbm.h>
#include <stdio.h>
#include <stdlib.h>
#include <peekpoke.h>
#include "level.h"

void main() {
    unsigned short finalmem;
    unsigned long totalBytes, bankedBytes;
    unsigned char startingBank, endingBank;
    char filename[32];
    unsigned short ramstart = (unsigned short)BANK_RAM;

    startingBank = 2;
    POKE(0, 2);

    sprintf(filename, "images/title.bin");
    printf("Loading file %s\n", filename);

    cbm_k_setnam(filename);
    cbm_k_setlfs(0, 8, 0);
    finalmem = cbm_k_load(0, (unsigned int)ramstart);

    endingBank = PEEK(0);
    printf("banks: %u %u\n", startingBank, endingBank);

    bankedBytes = (endingBank - startingBank) * 8192UL;
    printf("bankedBytes: %lu\n", bankedBytes);

    totalBytes =  ((endingBank - startingBank) * 8192UL) + (finalmem-ramstart); // bankedBytes + (finalmem-ramstart);
    
    
    printf("ram: %u %u\n", ramstart, finalmem);

    printf("totalBytes %lu\n", totalBytes); // 44032 since it rolls to other bank(s), actual is 76800
}

// cl65 -o testload.prg -t cx16 testload.c
// ../../emu/x16emu -prg testload.prg -run

// 10 for i=0 to 15 : print peek($a000+i) : next i