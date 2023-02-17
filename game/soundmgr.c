#include <cbm.h>
#include <cx16.h>
#include <peekpoke.h>

#include "memmap.h"
#include "pcmplayer.h"

void loadSounds() {
    unsigned short ramstart = (unsigned short)BANK_RAM;

    pcm_init();

    POKE(0, SOUND_BANK_LASER);
    cbm_k_setnam("zsound/mvcursor2.zcm");
	cbm_k_setlfs(0, 8, 2);
    cbm_k_load(0, (unsigned int)ramstart);
}

void playLaser() {
    pcm_trigger_digi(SOUND_BANK_LASER, (unsigned short)BANK_RAM);
}