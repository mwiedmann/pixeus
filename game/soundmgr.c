#include <cbm.h>
#include <cx16.h>
#include <peekpoke.h>

#include "memmap.h"
#include "pcmplayer.h"
#include "zsmplayer.h"

unsigned char MUSIC_ON = 1;
unsigned char SOUND_ON = 1;
unsigned char musicPlaying = 0;
unsigned char muted = 0;

void pauseSounds() {
    if (musicPlaying) {
        musicPlaying = 0;
        zsm_stopmusic();
    }
}

void loadSounds() {
    unsigned char currentMemBank;

    // Save the current bank
    currentMemBank = PEEK(0);

    if (SOUND_ON) {
        pcm_init();
        POKE(0, SOUND_BANK_LASER);
        cbm_k_setnam("sounds/shoot.zcm");
        cbm_k_setlfs(0, 8, 2);
        cbm_k_load(0, (unsigned short)BANK_RAM);

        POKE(0, SOUND_BANK_ALIEN_HIT);
        cbm_k_setnam("sounds/hit.zcm");
        cbm_k_setlfs(0, 8, 2);
        cbm_k_load(0, (unsigned short)BANK_RAM);
    }

    if (MUSIC_ON) {
        zsm_init();
        // We will load the music we want on demand
    }

    // Back to the previous bank
    POKE(0, currentMemBank);
}

void startMusic(zsm_callback cb) {
    unsigned char currentMemBank;

    if (!muted) {
        // Save the current bank
        currentMemBank = PEEK(0);

        zsm_startmusic(MUSIC_BANK, (unsigned short)BANK_RAM);

        if (cb != 0) {
            zsm_setcallback(cb);
        } else {
            zsm_forceloop(0);
        }

        musicPlaying = 1;

        // Back to the previous bank
        POKE(0, currentMemBank);
    }
}

void loadMusic(unsigned char* filename, zsm_callback cb) {
    if (MUSIC_ON && !muted) {
        unsigned char currentMemBank;

        // Save the current bank
        currentMemBank = PEEK(0);

        pauseSounds();

        POKE(0, MUSIC_BANK);

        cbm_k_setnam(filename);
        cbm_k_setlfs(0, 8, 2);
        cbm_k_load(0, (unsigned short)BANK_RAM);
        
        startMusic(cb);

        // Back to the previous bank
        POKE(0, currentMemBank);
    }
}

void loadEmptyMusic() {
    loadMusic("sounds/empty.zsm", 0);
}

void loadTitleMusic() {
    loadMusic("sounds/title.zsm", 0);
}

void loadCreditsMusic() {
    loadMusic("sounds/credits.zsm", 0);
}

void loadForestMusic() {
    loadMusic("sounds/forest.zsm", 0);
}

void loadDesertMusic() {
    loadMusic("sounds/desert.zsm", 0);
}

void loadTundraMusic() {
    loadMusic("sounds/tundra.zsm", 0);
}

void loadUnderworldMusic() {
    loadMusic("sounds/hell.zsm", 0);
}

// void loadStartMusic(zsm_callback cb) {
//     loadMusic("sounds/start.zsm", cb);
// }

// void loadGameOverMusic(zsm_callback cb) {
//     loadMusic("sounds/gameover.zsm", cb);
// }

// void loadVictoryMusic(zsm_callback cb) {
//     loadMusic("sounds/victory.zsm", cb);
// }

void playSoundsThisGameLoop() {
    if (SOUND_ON && !muted) {
        pcm_play();
    }

    if (MUSIC_ON && musicPlaying) {
        zsm_play();
    }

    POKE(0, LEVEL_BANK);
}

void soundsCleanup() {
    if (MUSIC_ON) {
        zsm_stopmusic();
    }
}

void playLaser() {
    if (SOUND_ON && !muted) {
        // pcm_stop();
        pcm_trigger_digi(SOUND_BANK_LASER, (unsigned short)BANK_RAM);
    }
}

void playAlienHit() {
    if (SOUND_ON && !muted) {
        // pcm_stop();
        pcm_trigger_digi(SOUND_BANK_ALIEN_HIT, (unsigned short)BANK_RAM);
    }
}

void muteToggle() {
    if (!muted) {
        loadEmptyMusic();
        muted = 1 - muted;
    } else {
        muted = 1 - muted;
        loadTitleMusic();
    }
}