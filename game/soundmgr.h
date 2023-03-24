#ifndef SOUNDMGR_H
#define SOUNDMGR_H

#include "zsmplayer.h"

void muteToggle();
void loadSounds();
void playSoundsThisGameLoop();
void soundsCleanup();
void playLaser();
void playAlienHit();
void pauseSounds();

void loadEmptyMusic();
void loadTitleMusic();
void loadCreditsMusic();
void loadForestMusic();
void loadDesertMusic();
void loadTundraMusic();
void loadUnderworldMusic();

void loadStartMusic(zsm_callback cb);
void loadGameOverMusic(zsm_callback cb);
void loadVictoryMusic(zsm_callback cb);

#endif