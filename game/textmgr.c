#include <stdio.h>
#include <cbm.h>
#include <cx16.h>

#include <peekpoke.h>
#include <joystick.h>
#include "loopmgr.h"
#include "x16graphics.h"
#include "memmap.h"
#include "imageload.h"
#include "fontmgr.h"
#include "sprites.h"
#include "levelutils.h"
#include "gamesprites.h"
#include "levelmgr.h"
#include "soundmgr.h"

#define SELECTION_START_GAME 1
#define SELECTION_JUKEBOX 2
#define SELECTION_MUTE_SOUND 3
#define SELECTION_TEST_MODE 4

#define TEST_MODE_HIDE 0
#define SELECTION_MAX 4 - TEST_MODE_HIDE

// Uncomment if we use the game start sound
// unsigned char gameStartDone;

void showTitleScreen() {
    unsigned char joy = 0;
    unsigned char wait = 60;

    // Clear the VRAM we will use for the bitmap mode title
    clearFullVRAM(0);
    clearVRAM(1, 0, 11264);

    // Set to 320x240 size
    POKE(0x9F2A, 64);
    POKE(0x9F2B, 64);

    // Enable Layer 1 only (probably already set this way but just in case) (leave other settings the same)
    POKE(VMEM_VIDEO, PEEK(VMEM_VIDEO) | 0b00100000);
    POKE(VMEM_VIDEO, PEEK(VMEM_VIDEO) & 0b11101111); // Disable layer 0 if its on

    // Config Layer 1 - H/W, 256clr, Bitmap Mode, 8bpp color depth
    POKE(LAYER_1_CONFIG, 0b00001111);

    // Bitmap mode loading a 320x240 image takes most of VRAM
    // Start in VBank 0, Addr 0
    POKE(LAYER_1_TILEBASE, 0);

    imageFileLoad(IMAGE_LOAD_BANK, 0, 0, "images/title.bin");
    preloadTextFiles();

    loadTitleMusic();

    while(1) {
        loopUpdates();

        joy = joy_read(0);

        // Exit when either button is pressed
        // Exit with testMode ON if UP is pressed
        if (JOY_BTN_1(joy)) {
            while(JOY_BTN_1(joy)) {
                loopUpdates();
        
                joy = joy_read(0);
            }
            break;
        } else if (JOY_BTN_2(joy)) {
            while(JOY_BTN_2(joy)) {
                loopUpdates();
        
                joy = joy_read(0);
            }
            break;
        }
    }

    // Turn off Layer 1 (both layers will be off while things are loading)
    POKE(VMEM_VIDEO, PEEK(VMEM_VIDEO) ^ 0b00100000);
}

void waitWithShipAnimation(Sprite *ship) {
    while(1) {
        // Wait for screen to finish drawing since we are animating the ship
        loopUpdates();
        spriteAnimationAdvance(ship);

        // Exit when either button is pressed
        if (readButtonPress()) {
            break;
        }
    }
}

unsigned char showSelectionScreen() {
    unsigned char selection = 1;
    unsigned char joy = 0;

    layerMapsClear();

    while (1) {
        drawCenteredTextRow("PIXEUS", 0, 6);
        drawCenteredTextRow("-GAME DESIGN AND CODING-", 0, 8);
        drawCenteredTextRow("MARK WIEDMANN", 0, 9);

        drawCenteredTextRow("-MUSIC-", 0, 11);
        drawCenteredTextRow("CRISPS", 0, 12);
        
        drawCenteredTextRow("USE THE JOYSTICK TO SELECT", 0, 15);

        drawCenteredTextRow(selection == 1 ? "::START GAME::" : "  START GAME  ", 0, 17);
        drawCenteredTextRow(selection == 2 ? "::JUKEBOX::" : "  JUKEBOX  ", 0, 18);
        drawCenteredTextRow(selection == 3 ? "::MUTE TOGGLE::" : "  MUTE TOGGLE  ", 0, 19);

        #if !TEST_MODE_HIDE
            drawCenteredTextRow(selection == 4 ? "::TEST MODE::" : "  TEST MODE  ", 0, 20);
        #endif

        while(1) {
            loopUpdates();
            
            joy = joy_read(0);

            if (JOY_UP(joy)) {
                selection--;
                if (selection==0) {
                    selection = SELECTION_MAX;
                }
                while(JOY_UP(joy)) {
                    loopUpdates();
            
                    joy = joy_read(0);
                }
                break;
            } else if (JOY_DOWN(joy)) {
                selection++;
                if (selection>SELECTION_MAX) {
                    selection = 1;
                }
                while(JOY_DOWN(joy)) {
                    loopUpdates();
            
                    joy = joy_read(0);
                }
                break;
            }

            if (readButtonPress()) {
                return selection;
            }
        }
    }
}

void songOverEmpty(unsigned char a, unsigned char b) {
    (void)a;
    (void)b;
    loadEmptyMusic();
}

// void gameStartOver(unsigned char a, unsigned char b) {
//     (void)a;
//     (void)b;
//     loadTitleMusic();
//     gameStartDone = 1;
// }

void showJukebox() {
    unsigned char selection = 1;
    unsigned char playMusic;
    unsigned char joy = 0;

    layerMapsClear();

    while (1) {
        drawCenteredTextRow("JUKEBOX", 0, 3);
        drawCenteredTextRow("ENJOY THESE AMAZING TUNES AND FX", 0, 5);
        drawCenteredTextRow("BY CRISPS", 0, 6);

        drawCenteredTextRow("USE THE JOYSTICK TO SELECT", 0, 9);

        drawCenteredTextRow("-MUSIC-", 0, 11);
        drawCenteredTextRow(selection == 1 ? "::TITLE::" : "  TITLE  ", 0, 12);
        drawCenteredTextRow(selection == 2 ? "::CREDITS::" : "  CREDITS  ", 0, 13);
        drawCenteredTextRow(selection == 3 ? "::FOREST::" : "  FOREST  ", 0, 14);
        drawCenteredTextRow(selection == 4 ? "::DESERT::" : "  DESERT  ", 0, 15);
        drawCenteredTextRow(selection == 5 ? "::TUNDRA::" : "  TUNDRA  ", 0, 16);
        drawCenteredTextRow(selection == 6 ? "::UNDERWORLD::" : "  UNDERWORLD  ", 0, 17);

        drawCenteredTextRow("-SOUND FX-", 0, 19);
        drawCenteredTextRow(selection == 7 ? "::START::" : "  START  ", 0, 20);
        drawCenteredTextRow(selection == 8 ? "::GAME OVER::" : "  GAME OVER  ", 0, 21);
        drawCenteredTextRow(selection == 9 ? "::VICTORY::" : "  VICTORY  ", 0, 22);
        

        drawCenteredTextRow(selection == 10 ? "::STOP MUSIC::" : "  STOP MUSIC  ", 0, 24);
        drawCenteredTextRow(selection == 11 ? "::EXIT::" : "  EXIT  ", 0, 25);

        playMusic = 0;

        while(1) {
            loopUpdates();
            
            joy = joy_read(0);

            if (JOY_UP(joy)) {
                selection--;
                if (selection==0) {
                    selection = 11;
                }
                while(JOY_UP(joy)) {
                    loopUpdates();
            
                    joy = joy_read(0);
                }
                break;
            } else if (JOY_DOWN(joy)) {
                selection++;
                if (selection>11) {
                    selection = 1;
                }
                while(JOY_DOWN(joy)) {
                    loopUpdates();
            
                    joy = joy_read(0);
                }
                break;
            }

            if (readButtonPress()) {
                playMusic = 1;
                break;
            }
        }

        if (playMusic) {
            switch(selection) {
                case 1: loadTitleMusic(); break;
                case 2: loadCreditsMusic(); break;
                case 3: loadForestMusic(); break;
                case 4: loadDesertMusic(); break;
                case 5: loadTundraMusic(); break;
                case 6: loadUnderworldMusic(); break;
                case 7: loadStartMusic(songOverEmpty); break;
                case 8: loadGameOverMusic(songOverEmpty); break;
                case 9: loadVictoryMusic(songOverEmpty); break;
                
                // Stopping the music breaks everything right now...WTF!
                // Cheat by playing an empty song
                case 10: loadEmptyMusic(); break; 
                case 11: return;
            }

            // zsound bug throws junk into the layer map when you stop/switch music
            // Just clear it as a workaround.
            layerMapsClear();
            loopUpdates();
        }
    }
}

unsigned char showIntroScene(Sprite *ship) {
    unsigned char selection;

    while (1) {
        selection = showSelectionScreen();

        // Test Mode - Jump right to the game
        if (selection == SELECTION_TEST_MODE) {
            return 1;
        } else if (selection == SELECTION_MUTE_SOUND) {
            muteToggle();
            layerMapsClear();
            loopUpdates();
            continue;
        } else if (selection == SELECTION_JUKEBOX) {
            showJukebox();
            continue;
        }

        // I don't love this Game Start sound but leaving this hook here
        /*
        gameStartDone = 0;
        loadStartMusic(gameStartOver);

        while(gameStartDone == 0) {
            loopUpdates();
        }
        
        loopUpdates();
        */

        layerMapsClear();
        drawTextFileFromBank(WELCOME_BANK, 0);

        spriteMove(ship, 288, 350);
        x16SpriteIdxSetXY(ship->index, ship->x, ship->y);
        ship->zDepth = BetweenL0L1;
        x16SpriteIdxSetZDepth(ship->index, ship->zDepth);

        waitWithShipAnimation(ship);

        layerMapsClear();
        drawTextFileFromBank(INSTRUCTIONS_BANK, 0);

        waitWithShipAnimation(ship);

        spriteMoveBack(ship);
        x16SpriteIdxSetXY(ship->index, ship->x, ship->y);

        if (selection == SELECTION_START_GAME) {
            return 0;
        }
    }
}

void gameOverScreen(unsigned short gold, unsigned char energy) {
    unsigned char scoreRow[41];

    layerMapsClear();

    sprintf(scoreRow, "GOLD:%04u ENERGY:%03u", gold, energy);
    drawCenteredTextRow(scoreRow, 0, 4);

    drawTextFile("text/gameover.bin", 0);

    while(1) {
        loopUpdates();
        
        // Exit when either button is pressed
        if (readButtonPress()) {
            break;
        }
    }
}

void victoryScreen(Sprite *ship, unsigned short gold, unsigned char energy) {
    unsigned char text[41];
    unsigned char loop = 0;
    
    loadCreditsMusic();
    loopUpdates();

    while (loop <= 1) {
        layerMapsClear();

        if (loop == 0) {
            if (gold == 0) {
                drawTextFile("text/nogold.bin", 0);
            } else {
                sprintf(text, "PIXEUS SPENDS %u GOLD ON:", gold);
                drawCenteredTextRow(text, 0, 6);

                drawTextFile("text/gold.bin", gold);
            }
        } else {
            sprintf(text, "%u ENERGY TAKES PIXEUS TO:", energy);
            drawCenteredTextRow(text, 0, 6);

            drawTextFile("text/energy.bin", energy);
        }

        spriteMove(ship, 288, 366);
        x16SpriteIdxSetXY(ship->index, ship->x, ship->y);
        ship->zDepth = BetweenL0L1;
        x16SpriteIdxSetZDepth(ship->index, ship->zDepth);

        while(1) {
            // Wait for screen to finish drawing since we are animating the ship
            loopUpdates();
            
            spriteAnimationAdvance(ship);

            // Exit when either button is pressed
            if (readButtonPress()) {
                break;
            }
        }

        loop++;
    }
}