#ifndef SOUND_H
#define SOUND_H

#define SOUND_PRIORITY_MUSIC 0
#define SOUND_PRIORITY_SFX_1 1
#define SOUND_PRIORITY_SFX_2 2

#define SOUND_INDEX_NONE 0
#define SOUND_INDEX_TITLE 1
#define SOUND_INDEX_FOREST 2
#define SOUND_INDEX_DESERT 3
#define SOUND_INDEX_TUNDRA 4
#define SOUND_INDEX_HELL 5
#define SOUND_INDEX_CREDITS 6

#define SOUND_SFX_SHOOT 0
#define SOUND_SFX_HIT 1

void musicVolume(unsigned char volume);
void soundInit();
void soundPlaySFX(unsigned char effect, unsigned char priority) ;
void soundStopChannel(unsigned char priority);
void soundPlayMusic(unsigned char index);
void toggleMusic();

extern unsigned char currentMusic;

#endif
