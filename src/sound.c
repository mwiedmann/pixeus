/*
 * SPDX-FileCopyrightText: 2023 Toboter
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "sound.h"
#include "memmap.h"
#include "waitforjiffy.h"

#include <cbm.h>
#include <cx16.h>

unsigned char sfxAddressHigh[] = {0xa0, 0xa2, 0xa4, 0xa6, 0xa8, 0xaa, 0xac, 0xae, 0xb0, 0xb2, 0xb4};

unsigned char currentMusic = SOUND_INDEX_NONE;
unsigned char loadedMusic = SOUND_INDEX_NONE;

unsigned char musicOn = 1;
unsigned char sfxOn = 1;

unsigned char param1, param2;

char * soundFileNames[] = {
	"",
	"title.zsm",
	"forest.zsm",
	"desert.zsm",
	"tundra.zsm",
	"hell.zsm",
	"credits.zsm"
};

void loadSound(char* name, unsigned char index) {
	cbm_k_setlfs(0, 8, 2);
	cbm_k_setnam(name);
	cbm_k_load(0, ((unsigned short)sfxAddressHigh[index])<<8);
}

void soundLoadMusic(unsigned char index) {
	unsigned char prevBank = RAM_BANK;
	param2 = SOUND_PRIORITY_MUSIC;

	asm volatile ("ldx %v", param2);
	asm volatile ("jsr zsm_stop");

	RAM_BANK = MUSIC_BANK;

	cbm_k_setlfs(0, 8, 2);
	cbm_k_setnam(soundFileNames[index]);
	cbm_k_load(0, 0xa000);
	loadedMusic = index;
	RAM_BANK = prevBank;
}

void volume(unsigned char volume, unsigned char priority) {
	param1 = priority;
	param2 = volume; // 0=FULL - 63=MUTE

	asm volatile ("ldx %v", param1);
	asm volatile ("lda %v", param2);
	asm volatile ("jsr zsm_setatten");
}

void soundInit() {
	unsigned char i=0;
	asm volatile ("lda #%b", ZSM_BANK);
	asm volatile ("jsr zsm_init_engine");
	asm volatile ("jsr zsmkit_setisr");

	RAM_BANK = SFX_BANK;

	loadSound("shoot.zsm", SOUND_SFX_SHOOT);
	loadSound("hit.zsm", SOUND_SFX_HIT);
	loadSound("powerdn.zsm", SOUND_SFX_POWER_DOWN);
	loadSound("powerup.zsm", SOUND_SFX_POWER_UP);
	loadSound("gold.zsm", SOUND_SFX_GOLD);

	volume(24, SOUND_PRIORITY_SFX_1);
	volume(24, SOUND_PRIORITY_SFX_2);

	soundLoadMusic(SOUND_INDEX_TITLE);
}

void soundStopChannel(unsigned char priority) {
	if (priority == SOUND_PRIORITY_MUSIC) {
		currentMusic = 0;
	}
	
	param2 = priority;
	asm volatile ("ldx %v", param2);
	asm volatile ("jsr zsm_stop");
}

void soundPlaySFX(unsigned char effect, unsigned char priority) {
	unsigned char prevBank = RAM_BANK;

	if (!sfxOn) {
		return;
	}
	
	RAM_BANK = SFX_BANK;

	param1 = sfxAddressHigh[effect];
	param2 = priority;

	asm volatile ("ldx %v", param2);
	asm volatile ("jsr zsm_stop");

	asm volatile ("lda #$00");
	asm volatile ("ldx %v", param2);
	asm volatile ("ldy %v", param1); //address hi to Y
	asm volatile ("jsr zsm_setmem");

	asm volatile ("ldx %v", param2);
	asm volatile ("jsr zsm_play");

	RAM_BANK = prevBank;
}

void soundPlayMusic(unsigned char index) {
	unsigned char prevBank = RAM_BANK;

	if (!musicOn || index == currentMusic) {
		return;
	}

	currentMusic = index;

	param1 = 0xa0;
	param2 = SOUND_PRIORITY_MUSIC;

	asm volatile ("ldx %v", param2);
	asm volatile ("jsr zsm_stop");

	if (!index) {
		return;
	}

	if (index != loadedMusic) {
		soundLoadMusic(index);
	}

	loadedMusic = index;
	RAM_BANK = MUSIC_BANK;

	asm volatile ("lda #$00");
	asm volatile ("ldx %v", param2);
	asm volatile ("ldy %v", param1); //address hi to Y
	asm volatile ("jsr zsm_setmem");


	asm volatile ("ldx %v", param2);
	asm volatile ("jsr zsm_play");

	asm volatile ("ldx %v", param2); //music loops
	asm volatile ("sec");
	asm volatile ("jsr zsm_setloop");

	waitforjiffy();

	volume(48, SOUND_PRIORITY_MUSIC);

	RAM_BANK = prevBank;
}

void toggleMusic() {
	soundStopChannel(SOUND_PRIORITY_MUSIC);

	musicOn = !musicOn;
}
