#include <cbm.h>
#include <cx16.h>
#include <peekpoke.h>

#include "memmap.h"
#include "pcmplayer.h"
#include "zsmplayer.h"

unsigned char MUSIC_ON = 1;
unsigned char SOUND_ON = 1;

void loadSounds() {
    if (SOUND_ON) {
        pcm_init();
        POKE(0, SOUND_BANK_LASER);
        cbm_k_setnam("sounds/guylaser.zcm");
        cbm_k_setlfs(0, 8, 2);
        cbm_k_load(0, (unsigned short)BANK_RAM);

        POKE(0, SOUND_BANK_ALIEN_HIT);
        cbm_k_setnam("sounds/alienhit.zcm");
        cbm_k_setlfs(0, 8, 2);
        cbm_k_load(0, (unsigned short)BANK_RAM);
    }

    if (MUSIC_ON) {
        zsm_init();
        // We will load the music we want on demand
    }

    // Back to the default bank
    POKE(0, LEVEL_BANK);
}

void loadTitleMusic() {
    unsigned char currentMemBank;

    // Save the current bank
    currentMemBank = PEEK(0);

    POKE(0, MUSIC_BANK);
    cbm_k_setnam("sounds/title.zsm");
    cbm_k_setlfs(0, 8, 2);
    cbm_k_load(0, (unsigned short)BANK_RAM);
    
    // Back to the previous bank
    POKE(0, currentMemBank);
}

void loadForestMusic() {
    unsigned char currentMemBank;

    // Save the current bank
    currentMemBank = PEEK(0);

    POKE(0, MUSIC_BANK);
    cbm_k_setnam("sounds/forest.zsm");
    cbm_k_setlfs(0, 8, 2);
    cbm_k_load(0, (unsigned short)BANK_RAM);

    // Back to the previous bank
    POKE(0, currentMemBank);
}

void startMusic() {
    unsigned char currentMemBank;

    // Save the current bank
    currentMemBank = PEEK(0);

    zsm_startmusic(MUSIC_BANK, (unsigned short)BANK_RAM);
    zsm_forceloop(0);

    // Back to the previous bank
    POKE(0, currentMemBank);
}

void playSoundsThisGameLoop() {
    if (SOUND_ON) {
        pcm_play();
    }

    if (MUSIC_ON) {
        zsm_play();
    }

    POKE(0, LEVEL_BANK);
}

void pauseSounds() {
    zsm_stopmusic();
}

void soundsCleanup() {
    if (MUSIC_ON) {
        zsm_stopmusic();
    }
}

void playLaser() {
    if (SOUND_ON) {
        pcm_stop();
        pcm_trigger_digi(SOUND_BANK_LASER, (unsigned short)BANK_RAM);
    }
}


void playAlienHit() {
    if (SOUND_ON) {
        pcm_stop();
        pcm_trigger_digi(SOUND_BANK_ALIEN_HIT, (unsigned short)BANK_RAM);
    }
}