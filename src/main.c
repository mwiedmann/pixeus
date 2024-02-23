#include <cx16.h>
#include <joystick.h>
#include <stdlib.h>

// Libs
#include "x16graphics.h"
#include "sprites.h"
#include "waitforjiffy.h"

// Game specific
#include "startup.h"
#include "gametiles.h"
#include "gamesprites.h"
#include "memmap.h"
#include "level.h"
#include "layoutdefs.h"
#include "textmgr.h"

// These managers are honestly just places to hold functions
// related to these entities
#include "levelmgr.h"
#include "enemymgr.h"
#include "levelutils.h"
#include "fontmgr.h"
#include "entitymgr.h"
#include "sound.h"

#define PLAYER_SHOOT_FRAMES 15

#define PLAYER_FALL_SPEED 20
#define PLAYER_WATER_FALL_SPEED 3
#define PLAYER_WATER_FALL_SPEED_FORCED 6

#define PLAYER_JUMP_SPEED_NORMAL 16
#define PLAYER_JUMP_SPEED_BOOTS 20
#define PLAYER_WATER_JUMP_SPEED_NORMAL 12
#define PLAYER_WATER_JUMP_SPEED_BOOTS 14

#define PLAYER_SPEED_NORMAL 12
#define PLAYER_SPEED_WITH_BOOTS 14
#define PLAYER_SWIM_SPEED_NORMAL 7
#define PLAYER_SWIM_SPEED_WITH_BOOTS 8

#define PLAYER_JUMP_FRAMES 24
#define PLAYER_WATER_JUMP_FRAMES 16

#define ICE_SLIDE_AMOUNT 3

#define BULLET_DIST_NORMAL 128
#define BULLET_DIST_WITH_WEAPON 144
#define BULLET_SPEED_NORMAL 35
#define BULLET_SPEED_WITH_WEAPON 45

#define DEATH_PAUSE_FRAMES 60

#define LEAVE_LEVEL_X_LEFT -8
#define LEAVE_LEVEL_X_RIGHT 631
#define LEAVE_LEVEL_Y_DOWN 463
#define LEAVE_LEVEL_Y_UP 4

// TEST LEVEL has a bunch of enemies, items, gold, energy, and tile types
#define TEST_LEVEL 7
#define STARTING_LEVEL 0

// WARNING: THIS NEEDS TO BE STARTING_LEVEL OR 0 FOR FINAL GAME BUILD !!!!
#define START_LEVEL STARTING_LEVEL

// For the ship landing animation
#define SHIP_STOP_Y 256

// NOTE: If adding more of these, there is a < PLAYER_ESCAPED check to update
// entityType for special exit conditions
#define PLAYER_ESCAPED 240
#define PLAYER_BURNED 250
#define PLAYER_OVERHEATED 251
#define PLAYER_FROZE 252
#define PLAYER_DROWNED 253
#define PLAYER_SHOT 254
#define PLAYER_EATEN 255

// Special EntranceId meaning the player exited via the edge of the screen
#define LEAVE_SCREEN_ENTRACE_ID 255
#define SHIP_ENTRACE_ID 254

#define PLAYER_FREEZES_COUNT 60
#define PLAYER_OVERHEATS_COUNT 30

#define ENERGY_TO_ESCAPE 10

unsigned char testMode = 0;
unsigned long mainFrameCount = 0;

// A few top level structs to hold things that stay
// active throughout the game life
Sprite player, bullet, expSmall, ship;
LevelOveralLayout* level;

// Player values
unsigned char energy;
unsigned short gold;
unsigned char lives;
unsigned char hasScuba;
unsigned char hasWeapon;
unsigned char hasBoots;
unsigned char hasShield;
unsigned char coldCount;
unsigned char hotCount;

unsigned char enemyCount = 0;
unsigned char entityCount = 0;

// These are special exits returned when the player has died
// OR exited by leaving the screen
Exit playerEaten = { PLAYER_EATEN };
Exit playerShot = { PLAYER_SHOT };
Exit playerDrowned = { PLAYER_DROWNED };
Exit playerFroze = { PLAYER_FROZE };
Exit playerOverheated = { PLAYER_OVERHEATED };
Exit playerBurned = { PLAYER_BURNED };

Exit playerEnterShipExit = { PLAYER_ESCAPED };

Exit playerScreenExit = { ExitEnum, 0, 0, 0, LEAVE_SCREEN_ENTRACE_ID };

void gameStartValues() {
    energy = 0;
    gold = 0;
    lives = 4;
    hasScuba = 0;
    hasWeapon = 0;
    hasBoots = 0;
    hasShield = 1;
    coldCount = 0;
    hotCount = 0;
}

void levelExitCleanup(unsigned char hideShip) {
    // This is jumping to another level. We need to cleanup this level.
    // Clean up the enemies and return the exit info
    enemiesReset(enemyCount);
    entitiesReset(entityCount);

    // Hide the ship, generally only needs to be done when leaving level 0,
    // but with test mode just hide it every time
    if (hideShip) {
        ship.zDepth = Disabled;
        x16SpriteIdxSetZDepth(ship.index, ship.zDepth);
    }
    // Reset any active bullets
    bullet.active = 0;
    bullet.zDepth = Disabled;
    x16SpriteIdxSetZDepth(bullet.index, bullet.zDepth);
}

/**
 * Parse the current level, draw the tiles, create the enemies,
 * and run the level until the player hits an exit or dies.
*/
Exit* runLevel(unsigned char nextSpriteIndex, unsigned char *lastTilesetId, unsigned char showShipScene) {
    unsigned char collision, joy, loopCount, fallSpeed;
    unsigned char jumpFrames = 0;
    unsigned char releasedBtnAfterJump = 1;
    unsigned char updateHeader = 0;
    unsigned char framesUntilNextShot = 0;
    short momentum = 0;
    short momentumChange, moveBase;

    TileInfo tileCollision;
    TileInfo feetLastTouched;

    Entity *entityCollision;
    Exit *exitCollision;
    Energy *energyCollision;
    Gold *goldCollision;

    AISprite *hitEnemy;
    Sprite *hitLaser;
    Entrance *entrance;

    // Load the tileset for this level if it changed
    if (*lastTilesetId != level->tileList->tilesetId) {
        // Play the music for this environment/tileset
        switch (level->tileList->tilesetId)
        {
            case 1: soundPlayMusic(SOUND_INDEX_FOREST); break;
            case 2: soundPlayMusic(SOUND_INDEX_DESERT); break;
            case 3: soundPlayMusic(SOUND_INDEX_TUNDRA); break;
            case 4: soundPlayMusic(SOUND_INDEX_HELL); break;
        }

        // Stopping music causes issues with the layer maps (zsound bug?)
        // Call the loop update to allow music change to take effect,
        // then layerMapsClear should clean things up
        waitforjiffy();

        // Clear before loading the next tileset or the existing level
        // briefly shows with a new tileset. Looks kinda weird for a sec.
        layerMapsClear();

        // Load in the new tileset
        RAM_BANK = CODE_BANK;
        tilesConfig(level->tileList->tilesetId);
        RAM_BANK = LEVEL_BANK;    
    } else {
        // No music/tileset change. Just clear the maps/screen
        layerMapsClear();
    }

    *lastTilesetId = level->tileList->tilesetId;

    // Draw the tiles and create enemies
    layerMapsLevelInit(level);
    drawGameHeader(gold, energy, lives, hasScuba, hasWeapon, hasBoots, coldCount, hotCount);
    enemyCount = enemiesCreate(level, nextSpriteIndex, mainFrameCount);
    nextSpriteIndex+= enemyCount;
    RAM_BANK = CODE_BANK;
    entityCount = entitiesCreate(level, nextSpriteIndex);
    RAM_BANK = LEVEL_BANK;
    nextSpriteIndex+= entityCount;

    if (level->levelNum == 0) {
        // On level 0 we need to show the ship
        // If skipping the scene, make sure the ship is in the correct location
        if (!showShipScene) {
            ship.y = SHIP_STOP_Y;
            x16SpriteIdxSetXY(ship.index, ship.x, ship.y);
        }
        ship.zDepth = BetweenL0L1;
        x16SpriteIdxSetZDepth(ship.index, ship.zDepth);
    }

    // Make sure the player is visible
    if (!showShipScene) {
        player.zDepth = BetweenL0L1;
        x16SpriteIdxSetZDepth(player.index, player.zDepth);
    }

    loopCount = 0;

    while (1) {
        waitforjiffy();

        // Special ship landing scene before the player can move
        if (showShipScene) {
            spriteMoveYL(&ship, ship.yL+(ship.speed+ship.speed));
            x16SpriteIdxSetXY(ship.index, ship.x, ship.y);
            if (ship.y >= SHIP_STOP_Y) {
                showShipScene = 0;
                ship.animationFrame = ship.animationStopFrame;
                x16SpriteIdxSetGraphicsPointer(ship.index, ship.clrMode, ship.graphicsBank,
                    ship.graphicsAddress+(ship.animationFrame * ship.frameSize));

                player.zDepth = BetweenL0L1;
                x16SpriteIdxSetZDepth(player.index, player.zDepth);
                continue;
            }
            spriteAnimationAdvance(&ship);
            continue;
        }

        // See if player is touching an entity (exit, item, energy, gold, etc.)
        entityCollision = playerTouchingEntity(level->entityList, &player);
        if (entityCollision != 0) {
            if (entityCollision->entityType == ExitEnum) {
                exitCollision = (Exit*)entityCollision;
                // Special exit entering the ship and completing the game
                if (exitCollision->entranceId == SHIP_ENTRACE_ID) {
                    if (energy >= ENERGY_TO_ESCAPE) {
                        levelExitCleanup(0);
                        return &playerEnterShipExit;
                    }
                } 
                // If this jumps somewhere on the same level, just move the player
                else if (exitCollision->level == level->levelNum) {
                    entrance = findEntranceForExit((EntranceList*)(level->entityList), exitCollision->entranceId);
                    spriteMoveToTile(&player, entrance->x, entrance->y, TILE_PIXEL_WIDTH, TILE_PIXEL_HEIGHT);    
                    x16SpriteIdxSetXY(player.index, player.x, player.y);
                } else {
                    levelExitCleanup(1);
                    return exitCollision;
                }
            } else if (entityCollision->entityType == EnergyEnum) {
                energyCollision = (Energy*)entityCollision;
                energy+= energyCollision->amount;
                hideEntity(entityCount, entityCollision);
                updateHeader = 1;
                hasShield=1; // Energy also restores the shield
            } else if (entityCollision->entityType == GoldEnum) {
                goldCollision = (Gold*)entityCollision;
                gold+= goldCollision->amount;
                hideEntity(entityCount, entityCollision);
                updateHeader = 1;
            } else if (entityCollision->entityType == ScubaEnum) {
                hasScuba = 1;
                showMessage("SCUBA GEAR: EXPLORE THE WATER");
                hideEntity(entityCount, entityCollision);
                updateHeader = 1;
            } else if (entityCollision->entityType == WeaponEnum) {
                hasWeapon = 1;
                showMessage("WEAPON UPGRADE");
                hideEntity(entityCount, entityCollision);
                updateHeader = 1;
            } else if (entityCollision->entityType == BootsEnum) {
                hasBoots = 1;
                showMessage("BOOTS: ENHANCED SPEED AND JUMPING");
                hideEntity(entityCount, entityCollision);
                updateHeader = 1;
            } else if (entityCollision->entityType == ExtraLifeEnum) {
                lives++;
                showMessage("EXTRA LIFE!!!");
                hideEntity(entityCount, entityCollision);
                updateHeader = 1;
            }
        }

        enemiesMove(level, &player, enemyCount);
        enemyLasersMove(level);
        entitiesAnimate(entityCount);

        // Count game loops so we can animate sprites.
        // Only animate if the guy is "going" somewhere (don't shuffle his feet if standing still!)
        player.going=0;
        player.animationCount++;

        // We are changing the guys animation every few game loops
        // but hold at this count as we only animate if moving.
        if (player.animationCount > player.animationSpeed) {
            player.animationCount=player.animationSpeed;
        }

        joy = joy_read(0);

        // FOR TESTING...LEVEL WARPS
        // if (JOY_BTN_3(joy)) {
        //     playerScreenExit.level = 255;
        //     if (JOY_DOWN(joy)) {
        //         levelExitCleanup(1);
        //         spriteMove(&player, player.x, LEAVE_LEVEL_Y_UP);
        //         playerScreenExit.level = level->downLevel;
        //     } else if (JOY_UP(joy)) {
        //         levelExitCleanup(1);
        //         spriteMove(&player, player.x, LEAVE_LEVEL_Y_DOWN-8);
        //         playerScreenExit.level = level->upLevel;
        //     } else if (JOY_LEFT(joy)) {
        //         levelExitCleanup(1);
        //         spriteMove(&player, LEAVE_LEVEL_X_RIGHT - 1, player.y);
        //         playerScreenExit.level = level->leftLevel;
        //     } else if (JOY_RIGHT(joy)) {
        //         levelExitCleanup(1);
        //         spriteMove(&player, LEAVE_LEVEL_X_LEFT + 1, player.y);
        //         playerScreenExit.level = level->rightLevel;
        //     }
            
        //     if (playerScreenExit.level != 255) {
        //         showMessage("WARP");
        //         return &playerScreenExit;
        //     }
        // }

        // Falling
        // jumpFrames are the number of frames the player moves UP (jumps)
        // At 0, the player is always falling, even if on the ground.
        if (jumpFrames == 0) {
            // See what the player is currently touching to see his fall (or swim-fall) speed
            spriteTouchingTile(level, &player, &tileCollision);

            // See how quickly the player is falling (based on if in air or water)
            // In water they can slowly float down or press DOWN and swim down more quickly
            fallSpeed = tileCollision.type == Empty ?
                PLAYER_FALL_SPEED :
                JOY_DOWN(joy)
                    // The player falls in water too, just more slowly
                    ? PLAYER_WATER_FALL_SPEED_FORCED
                    : PLAYER_WATER_FALL_SPEED;
            
            // If swimming down, animate the player
            if (fallSpeed == PLAYER_WATER_FALL_SPEED_FORCED) {
                player.going = 1;
            }
            
            spriteMoveYL(&player, player.yL + fallSpeed);
            spriteTouchingTile(level, &player, &tileCollision);

            // If the player is standing on a tile, a few things happen
            if (tileCollision.type != Empty) {
                // If a solid tile, move the player to the top of the tile
                // We do this because as the player falls they may end up wedged a few pixels into the tile
                if (tileCollision.type == Ground || tileCollision.type == Ice) {
                    // This is the last surface the player touched
                    // We track this to know if they are sliding on ice or on better ground
                    feetLastTouched = tileCollision;
                    spriteMoveY(&player, ((tileCollision.y * TILE_PIXEL_HEIGHT) - pixelSizes[player.height]) - 1);
                }

                // Since the player is touching something, see if they pressed the jump button
                if (JOY_BTN_1(joy) || JOY_UP(joy)) {
                    // Only let them jump if they released the jump button since the last jump.
                    // Without this, the player just hops as you hold the button.
                    if (releasedBtnAfterJump) {
                        jumpFrames = tileCollision.type == Water ? PLAYER_WATER_JUMP_FRAMES : PLAYER_JUMP_FRAMES;
                        releasedBtnAfterJump = 0;
                        // The jump animation wasn't great. The normal animation is fine
                    }
                } else {
                    // Player is on ground and isn't holding the button
                    // They can jump again next game loop.
                    releasedBtnAfterJump = 1;
                }
            }

            // Player leaving bottom of the screen
            if (player.y >= LEAVE_LEVEL_Y_DOWN && level->downLevel != LEAVE_SCREEN_ENTRACE_ID) {
                levelExitCleanup(1);
                spriteMove(&player, player.x, LEAVE_LEVEL_Y_UP);
                playerScreenExit.level = level->downLevel;

                return &playerScreenExit;
            }
        } else {
            // Player is jumping, move them up
            jumpFrames--;
            player.going=1;
            spriteMoveYL(&player, player.yL-(
                tileCollision.type == Water 
                ? hasBoots 
                    ? PLAYER_WATER_JUMP_SPEED_BOOTS
                    : PLAYER_WATER_JUMP_SPEED_NORMAL 
                : hasBoots
                    ? PLAYER_JUMP_SPEED_BOOTS
                    : PLAYER_JUMP_SPEED_NORMAL
                )
            );
            
            // Don't let the player jump through solid ground
            // Move them back if they hit something
            spriteTouchingTile(level, &player, &tileCollision);
            if (tileCollision.type == Ground || tileCollision.type == Ice) {
                spriteMoveBackY(&player);
            }

            // Player leaving top of screen (normally only by swimming)
            if (player.y <= LEAVE_LEVEL_Y_UP && level->upLevel != LEAVE_SCREEN_ENTRACE_ID) {
                levelExitCleanup(1);
                // Move the player an extra few pixels because they will immediately start falling back down
                // When swimming we want to give them a chance to kick up a few times and not fall back to the previous level
                spriteMove(&player, player.x, LEAVE_LEVEL_Y_DOWN-8);
                playerScreenExit.level = level->upLevel;

                return &playerScreenExit;
            }
        }

        // Check the final tile the player is touching to see if the graphics need to change to scuba or back to running
        spriteTouchingTile(level, &player, &tileCollision);
        if ((tileCollision.type == Ground || tileCollision.type == Ice || tileCollision.type == Empty)) {
            // Player dies in the desert after some time...needs water
            if (level->tileList->tilesetId == TILESETID_DESERT) {
                if (hotCount == 0) {
                    hotCount = PLAYER_FREEZES_COUNT;
                }
                coldCount = 0;
                if (loopCount == 60) { 
                    hotCount--;
                    drawGameHeader(gold, energy, lives, hasScuba, hasWeapon, hasBoots, coldCount, hotCount);
                    if (hotCount == 0) {
                        hotCount = 0;
                        return &playerOverheated;
                    }
                }
            } else {
                if (hotCount > 0 || coldCount > 0) {
                    hotCount = 0;
                    coldCount = 0;
                    drawGameHeader(gold, energy, lives, hasScuba, hasWeapon, hasBoots, coldCount, hotCount);
                }
            }

            spriteAnimationAddressSet(&player, hasShield ? SPRITE_MEM_PLAYER_SHIELD_IDX : SPRITE_MEM_PLAYER_IDX);
        } else if (tileCollision.type == Water) {
            // Player freezes in the cold water after some time
            if (level->tileList->tilesetId == TILESETID_WINTER) {
                if (coldCount == 0) {
                    coldCount = PLAYER_FREEZES_COUNT;
                }
                hotCount = 0;
                if (loopCount == 60) { 
                    coldCount--;
                    drawGameHeader(gold, energy, lives, hasScuba, hasWeapon, hasBoots, coldCount, hotCount);
                    if (coldCount == 0) {
                        coldCount = 0;
                        return &playerFroze;
                    }
                }
            } else {
                if (hotCount > 0 || coldCount > 0) {
                    hotCount = 0;
                    coldCount = 0;
                    drawGameHeader(gold, energy, lives, hasScuba, hasWeapon, hasBoots, coldCount, hotCount);
                }
            }

            if (!hasScuba && !testMode) {
                // Player needs the Scuba gear to survive in water. DEAD!
                return &playerDrowned;
            }

            spriteAnimationAddressSet(&player, hasShield ? SPRITE_MEM_PLAYER_SCUBA_SHIELD_IDX : SPRITE_MEM_PLAYER_SCUBA_IDX);
        } else if (tileCollision.type == Lava) {
            return &playerBurned;
        }

        // Based on what the player is currently in, set their speed
        // Since we are dealing with ints, we multiply by 10 to give us some room
        // with the momentum. We divide by 10 later.
        moveBase = (tileCollision.type == Water
            ? (hasBoots ? PLAYER_SWIM_SPEED_WITH_BOOTS : PLAYER_SWIM_SPEED_NORMAL)
            : (hasBoots ? PLAYER_SPEED_WITH_BOOTS : PLAYER_SPEED_NORMAL)
        ) * 10;

        // If standing on Ice, momentum changes slowly
        // On other ground, movement is instant
        momentumChange = feetLastTouched.type == Ice ? ICE_SLIDE_AMOUNT : moveBase * 2;
        
        // Check if player is moving left/right
        if (JOY_LEFT(joy)) {
            player.going=1;
            // We also flip the animation depending on direction
            if (player.animationDirection != 0) {
                player.animationDirection=0;
                x16SpriteIdxSetHFlip(player.index, player.animationDirection);
            }

            momentum-= momentumChange;
            if (momentum < (-moveBase)) {
                momentum = (-moveBase);
            }
            spriteMoveXL(&player, player.xL+(momentum/10));

            // Player leaving left side of the screen
            if (player.x <= LEAVE_LEVEL_X_LEFT && level->leftLevel != LEAVE_SCREEN_ENTRACE_ID) {
                levelExitCleanup(1);
                spriteMove(&player, LEAVE_LEVEL_X_RIGHT - 1, player.y);
                playerScreenExit.level = level->leftLevel;

                return &playerScreenExit;
            }
        } else if (JOY_RIGHT(joy)) {
            player.going=1;
            // Maybe flip animation
            if (player.animationDirection != 1) {
                player.animationDirection=1;
                x16SpriteIdxSetHFlip(player.index, player.animationDirection);
            }

            momentum+= momentumChange;
            if (momentum > moveBase) {
                momentum = moveBase;
            }
            spriteMoveXL(&player, player.xL+(momentum/10));

            // Player leaving right side of the screen
            if (player.x >= LEAVE_LEVEL_X_RIGHT && level->rightLevel != LEAVE_SCREEN_ENTRACE_ID) {
                levelExitCleanup(1);
                spriteMove(&player, LEAVE_LEVEL_X_LEFT + 1, player.y);
                playerScreenExit.level = level->rightLevel;

                return &playerScreenExit;
            }
        } else {
            // Player is not trying to move
            // Slow them down if they are still moving
            if (momentum > 0) {
                momentum-= momentumChange;
                if (momentum < 0) {
                    momentum = 0;
                }
            } else if (momentum < 0) {
                momentum+= momentumChange;
                if (momentum > 0) {
                    momentum = 0;
                }
            }
            if (momentum != 0) {
                spriteMoveXL(&player, player.xL+(momentum/10));
            }
        }

        // Change animation if jumping or moving and hit loop count
        if (jumpFrames > 0) {
            // No "jump" animation because it looked kinda dumb
            // Leaving this here though in case I change my mind later
        } else if (player.going==1 && player.animationCount >= player.animationSpeed) {
            player.animationCount=0;
            player.animationFrame++;
            if (player.animationFrame >= player.frames) {
                player.animationFrame = 0;
            }
            x16SpriteIdxSetGraphicsPointer(player.index, player.clrMode, player.graphicsBank,
                player.graphicsAddress+(player.animationFrame * player.frameSize));
        } else if (player.animationCount >= player.animationSpeed && player.animationFrame != player.animationStopFrame) {
            // If the guy is standing still, always show a certain frame
            // In future this could be a totally different animation
            // We could have a yawn animation for instance for when waiting too long.
            player.animationFrame = player.animationStopFrame;
            x16SpriteIdxSetGraphicsPointer(player.index, player.clrMode, player.graphicsBank,
                player.graphicsAddress+(player.animationFrame * player.frameSize));
        }
        
        // Move the bullet (yes, the player can only have 1 active bullet right now)
        if (bullet.active) {
            spriteMoveXL(&bullet, bullet.animationDirection == 0
                ? bullet.xL-(hasWeapon ? BULLET_SPEED_WITH_WEAPON : BULLET_SPEED_NORMAL) 
                : bullet.xL+(hasWeapon ? BULLET_SPEED_WITH_WEAPON : BULLET_SPEED_NORMAL));
            x16SpriteIdxSetXY(bullet.index, bullet.x, bullet.y);
        }

        
        // Player has to wait a certain number of frames before shooting
        if (framesUntilNextShot > 0) {
            framesUntilNextShot--;
        }

        // See if player is shooting
        if (bullet.active==0 && framesUntilNextShot == 0 && JOY_BTN_2(joy)) {
            bullet.active = 1;
            bullet.animationDirection = player.animationDirection;
            // Start the bullet out of the player's grid
            bullet.startX = player.x; // + (player.animationDirection == 0 ? -24 : 40);
            spriteMove(&bullet, player.x, player.y);
            // Show the bullet
            bullet.zDepth = BetweenL0L1;
            x16SpriteIdxSetZDepth(bullet.index, bullet.zDepth);
            x16SpriteIdxSetXY(bullet.index, bullet.x, bullet.y);
            x16SpriteIdxSetHFlip(bullet.index, bullet.animationDirection);

            soundPlaySFX(SOUND_SFX_SHOOT, SOUND_PRIORITY_SFX_1);
            framesUntilNextShot = PLAYER_SHOOT_FRAMES;
        }

        // See if the player is touching any tiles left/right
        // Move back if so.
        // With the current code we may end up checking tiles more than needed
        // but it's an O(1) operation since the tiles are in an array, so it should be fine. 
        spriteTouchingTile(level, &player, &tileCollision);
        if (tileCollision.type == Ground || tileCollision.type == Ice) {
            spriteMoveBackX(&player);
        }

        // Final position of the player. Update their sprite
        x16SpriteIdxSetXY(player.index, player.x, player.y);

        // Manage explosion sprites
        expSmall.animationCount++;
        if (expSmall.animationCount >= expSmall.animationSpeed) {
            expSmall.animationCount=0;
            expSmall.animationFrame++;
            x16SpriteIdxSetGraphicsPointer(expSmall.index, expSmall.clrMode, expSmall.graphicsBank,
                expSmall.graphicsAddress+(expSmall.animationFrame * expSmall.frameSize));

            if (expSmall.animationFrame >= expSmall.frames) {
                expSmall.zDepth = Disabled;
                x16SpriteIdxSetZDepth(expSmall.index, expSmall.zDepth);
            }
        }
            
        // Get the Collision bits and shift them down
        collision = x16SpriteCollisionBitsGet();

        // Check if there were any collisions
        // Chance here if player and multiple enemy sprites collide that this is skipped
        if (collision == COLLISION_RESULT_ENEMY_PLAYER) {
            // Player collided with an enemy
            if (!testMode) {
                return &playerEaten;
            }
        } else if (collision == COLLISION_RESULT_ENEMY_LASER_PLAYER) {
            // Player was hit by an enemy laser
            hitLaser = findEnemyLaserCollision(&player);
            if (hitLaser != 0) {
                resetLaser(hitLaser);
                if (!testMode) {
                    if (hasShield) {
                        hasShield = 0;
                        spriteAnimationAddressSet(&player, SPRITE_MEM_PLAYER_IDX);
                    } else {
                        return &playerShot;
                    }
                }
            }
        } else if (((collision & COLLISION_RESULT_ENEMY_PLAYER_LASER) == COLLISION_RESULT_ENEMY_PLAYER_LASER) && bullet.active) {
            // Enemy was hit by the player's laser
            // Show explosion
            smallExplosion(&expSmall, BetweenL0L1, bullet.x, bullet.y);
            soundPlaySFX(SOUND_SFX_HIT, SOUND_PRIORITY_SFX_2);

            // Find the enemy
            hitEnemy = findEnemyCollision(&bullet);
            if (hitEnemy != 0) {
                // Having the weapon does 2 dmg
                hitEnemy->health = hitEnemy->health == 1 ? 0 : hitEnemy->health - (1 + hasWeapon);
                if (hitEnemy->health == 0) {
                    hitEnemy->sprite.active = 0;
                    hitEnemy->sprite.zDepth = Disabled;
                    x16SpriteIdxSetZDepth(hitEnemy->sprite.index, hitEnemy->sprite.zDepth);
                }
            }

            // Deactivate the bullet so player can fire again
            bullet.active = 0;
            bullet.zDepth = Disabled;
            x16SpriteIdxSetXY(bullet.index, 0, 0);
            x16SpriteIdxSetZDepth(bullet.index, bullet.zDepth);
        }

        // Bullet is off screen or collided with a solid tile
        if (bullet.active) {
            spriteTouchingTile(level, &bullet, &tileCollision);
            if (tileCollision.type == Ground || tileCollision.type == Ice || bullet.x < 0 || bullet.x > 639 || 
                abs(bullet.x - bullet.startX) >= (hasWeapon ? BULLET_DIST_WITH_WEAPON : BULLET_DIST_NORMAL)) {
                if (tileCollision.type == Ground || tileCollision.type == Ice) {
                    // Explosion
                    smallExplosion(&expSmall, InFrontOfL1, bullet.x, bullet.y);
                    soundPlaySFX(SOUND_SFX_HIT, SOUND_PRIORITY_SFX_2);
                }
                bullet.active = 0;
                bullet.zDepth = Disabled;
                x16SpriteIdxSetZDepth(bullet.index, bullet.zDepth);
            }
        }

        // If anything in the header changes (gold, energy, lives, etc.), redraw it
        if (updateHeader) {
            updateHeader = 0;
            drawGameHeader(gold, energy, lives, hasScuba, hasWeapon, hasBoots, coldCount, hotCount);
        }

        mainFrameCount++;
        loopCount++;
        if (loopCount > 60) {
            loopCount=1;
        }
    }
}

void main() {
    unsigned char i;
    unsigned char clearData = 0;
    short playerEnterX, playerEnterY;

    // CX16 has 128 sprites for us to use. Each has an id/index.
    // As we create sprites we increase this value so we know what the next
    // available sprite index is.
    unsigned char nextSpriteIndex;

    // On the 1st level we show the ship landing before the game starts
    unsigned char showShipScene = 1;

    // Running each level will return exit info
    // It says if the player exited to another level and how, OR if they died
    Exit exitCollision;

    // Need this to hold onto any specific entrance the player is jumping to
    // For the starting level, the game looks for EntranceId=0
    Entrance *entrance;

    // Sounds are loaded into HIRAM Banks for use later
    // Make sure not to overwrite these banks
    soundInit();

    RAM_BANK = CODE_BANK;
    cbm_k_setnam("pixeus.prg.01");
    cbm_k_setlfs(0, 8, 0);
    cbm_k_load(0, (unsigned short)BANK_RAM);

    // Configure the joysticks
    joy_install(cx16_std_joy);

    // Load the palette for the game
    // It is mostly the standard palette but some colors in the 17-30-ish range
    // have been swapped out
    paletteLoad();
    RAM_BANK = LEVEL_BANK;

    while(1) {
        // When we go to the next level, we may need to load a new tileset
        // Use this to track the last tileset so we know if we need to load
        unsigned char lastTilesetId = 255;

        soundStopChannel(SOUND_PRIORITY_MUSIC);

        nextSpriteIndex = 0;
        gameStartValues();

        // Init the level cache
        // We cache some level data (entities) because certain things don't respawn (energy, gold)
        // so we need to remember which ones are gone
        initCachedLevelData(clearData);

        // Get the starting level and main entrance
        // It loads quickly but do it before showing title to minimize time after title screen
        level = levelGet(START_LEVEL);
        entrance = findEntranceForExit(((EntranceList*)level->entityList), 0);

        // Clear the maps to remove any extra VMEM junk from showing on title screen
        layerMapsClear();

        // Show the bitmap title screen
        RAM_BANK = CODE_BANK;
        showTitleScreen();

        // Clear the maps to remove the title screen junk
        layerMapsClear();

        // Load all the sprites
        // TODO: Currently we load all possible sprites for the game. Refactor?
        // Currently we have enough memory for this BUT as the game grows...
        // Maybe refactor to load just the needed sprites with each level?
        spriteDataLoad();
        
        spriteIRQConfig();
        playerCreate(&player, entrance, nextSpriteIndex++);
        bulletCreate(&bullet, nextSpriteIndex++);
        explosionSmallCreate(&expSmall, nextSpriteIndex++);
        nextSpriteIndex = enemyLasersCreate(nextSpriteIndex);
        shipCreate(&ship, nextSpriteIndex++);

        RAM_BANK = LEVEL_BANK;
        // Wait to switch to game mode until everything is loaded
        // If you switch video modes first, you get crazy stuff on screen (kinda cool?)
        videoConfig();

        RAM_BANK = CODE_BANK;

        // Load some standard tiles (empty, black, font)
        // These stay in the Tileset RAM for the entire game
        // Level tilesets are loaded in RAM after
        standardTilesLoad();

        // Show the intro screen before starting the level
        
        testMode = showIntroScene(&ship);
        RAM_BANK = LEVEL_BANK;

        showShipScene = !testMode;

        // We use this as a general counter for game length to reset enemies after some time
        mainFrameCount = 0;

        while(1) {
            // Get the player's position at the start of the level
            // If they die, we put them back here
            playerEnterX = player.x;
            playerEnterY = player.y;

            // Get a copy of the exitCollision because we will free the level next
            exitCollision = *runLevel(nextSpriteIndex, &lastTilesetId, showShipScene);
            levelCacheUpdate(level, mainFrameCount);
            showShipScene = 0;

            // If this was a normal level exit (not a player death)
            // Then load the next level
            if (exitCollision.entityType < PLAYER_ESCAPED) {
                // Free the memory for the last level and load the next one
                freeLevel(level);
                level = levelGet(exitCollision.level);
                
                // If the player is going to a specific entranace place them there
                if (exitCollision.entranceId != LEAVE_SCREEN_ENTRACE_ID) {
                    entrance = findEntranceForExit(((EntranceList*)level->entityList), exitCollision.entranceId);
                    spriteMoveToTile(&player, entrance->x, entrance->y, TILE_PIXEL_WIDTH, TILE_PIXEL_HEIGHT);
                }
            } else {
                // Pause the game for a moment since player died (or escaped!)
                for (i=0; i<DEATH_PAUSE_FRAMES; i++) {
                    waitforjiffy();
                }
                switch(exitCollision.entityType) {
                    case PLAYER_EATEN: showMessage("PIXEUS IS CONSUMED BY THE CREATURE"); break;
                    case PLAYER_SHOT: showMessage("THE PIERCED BODY OF PIXEUS FALLS"); break;
                    case PLAYER_DROWNED: showMessage("TOXIC WATER DISSOLVES PIXEUS"); break;
                    case PLAYER_FROZE: showMessage("PIXEUS FREEZES IN THE ICY WATER"); break;
                    case PLAYER_OVERHEATED: showMessage("PIXEUS FAINTS IN THE DESERT HEAT"); break;
                    case PLAYER_BURNED: showMessage("PIXEUS BURNS IN THE MOLTEN LAVA"); break;
                    case PLAYER_ESCAPED: showMessage("PIXEUS LOADS THE ARGO WITH ENERGY"); break;
                }

                // See if game over
                if (lives == 0 || exitCollision.entityType == PLAYER_ESCAPED) {
                    clearData = 1;
                    freeLevel(level);

                    if (exitCollision.entityType == PLAYER_ESCAPED) {
                        levelExitCleanup(0);
                        x16SpriteIdxSetZDepth(player.index, Disabled);
                        RAM_BANK = CODE_BANK;
                        victoryScreen(&ship, gold, energy);
                        RAM_BANK = LEVEL_BANK;
                    } else {
                        levelExitCleanup(1);
                        // Place Pixeus for the game over screen
                        spriteMove(&player, 312, 360);
                        x16SpriteIdxSetXY(player.index, player.x, player.y);
                        x16SpriteIdxSetZDepth(ship.index, Disabled);
                        
                        RAM_BANK = CODE_BANK;
                        gameOverScreen(gold, energy);
                        RAM_BANK = LEVEL_BANK;
                    }
                    break;
                } else {
                    // We will recreate and redraw the level they are on
                    // Cleanup the level first though
                    levelExitCleanup(1);
                    hasShield = 1;
                    spriteAnimationAddressSet(&player, SPRITE_MEM_PLAYER_SHIELD_IDX);
                }
                
                // One less life remaining
                lives--;

                // Move them back to where they started the level
                spriteMove(&player, playerEnterX, playerEnterY);

                // Reset env counters
                hotCount = 0;
                coldCount = 0;
            }

            x16SpriteIdxSetXY(player.index, player.x, player.y);
        }
    }

    // Disable sprite collisions before quitting
    x16SpriteCollisionsDisable();

    // TODO: Restore the text video mode

}