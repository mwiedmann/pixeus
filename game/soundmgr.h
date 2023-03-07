#ifndef SOUNDMGR_H
#define SOUNDMGR_H

#include "zsmplayer.h"

void loadSounds();
void playSoundsThisGameLoop();
void soundsCleanup();
void playLaser();
void playAlienHit();
void pauseSounds();
void loadTitleMusic();
void loadForestMusic();
void loadDesertMusic();
void loadUnderworldMusic();
void loadEmptyMusic();
void loadStartMusic(zsm_callback cb);
void loadGameOverMusic(zsm_callback cb);
void loadVictoryMusic(zsm_callback cb);

#endif