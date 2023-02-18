#include <cbm.h>
#include <cx16.h>
#include <peekpoke.h>

#include "memmap.h"
#include "pcmplayer.h"
#include "zsmplayer.h"

void loadSounds() {
    pcm_init();
    zsm_init();

    POKE(0, SOUND_BANK_LASER);
    cbm_k_setnam("sounds/guylaser.zcm");
	cbm_k_setlfs(0, 8, 2);
    cbm_k_load(0, (unsigned short)BANK_RAM);

    POKE(0, SOUND_BANK_ALIEN_HIT);
    cbm_k_setnam("sounds/alienhit.zcm");
	cbm_k_setlfs(0, 8, 2);
    cbm_k_load(0, (unsigned short)BANK_RAM);

    POKE(0, MUSIC_BANK);
    // cbm_k_setnam("sounds/drake.zsm");
    cbm_k_setnam("sounds/outrun.zsm");
	cbm_k_setlfs(0, 8, 2);
    cbm_k_load(0, (unsigned short)BANK_RAM);

    zsm_startmusic(MUSIC_BANK, (unsigned short)BANK_RAM);
	zsm_forceloop(0);

    // Back to the default bank
    POKE(0, LEVEL_BANK);
}

void playSoundsThisGameLoop() {
    pcm_play();
    zsm_play();
    POKE(0, LEVEL_BANK);
}

void soundsCleanup() {
    zsm_stopmusic();
}

void playLaser() {
    pcm_stop();
    pcm_trigger_digi(SOUND_BANK_LASER, (unsigned short)BANK_RAM);
}


void playAlienHit() {
    pcm_stop();
    pcm_trigger_digi(SOUND_BANK_ALIEN_HIT, (unsigned short)BANK_RAM);
}