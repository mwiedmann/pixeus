#include <cx16.h>
#include <cbm.h>
#include <stdio.h>
#include <peekpoke.h>

#define LOADADDR BANK_RAM

/**
 * Program to test loading .bin files
*/
void main() {
    unsigned int ret;
    unsigned int diff;
    unsigned int i;

    // Bank 2
    POKE(0, 2);

    printf("\nHELLO\n");
    cbm_k_setnam("images/sample.bin");
    cbm_k_setlfs(0, 8, 0);
    ret = cbm_k_load(0, LOADADDR);
    diff = ret - (unsigned int)LOADADDR;
    printf("At Addr %u Finshed at %u Diff %u", LOADADDR, ret, diff);
    printf("\nFINISHED\n");
    for (i=0; i<16; i++) {
        printf("%u ", PEEK((LOADADDR) + i));
    }
}

// cl65 -o testload.prg -t cx16 testload.c
// ../emu/x16emu -prg testload.prg -run

// 10 for i=0 to 15 : print peek($a000+i) : next i