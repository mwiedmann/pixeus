#ifndef GAMESPRITES_H
#define GAMESPRITES_H

#include "sprites.h"

void spriteDataLoad();
void spriteIRQConfig();

void playerCreate(Sprite *p);
void badguyCreate(Sprite *bg);
void bulletCreate(Sprite *b);

#endif