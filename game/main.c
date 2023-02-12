#include <cx16.h>
#include <joystick.h>
#include <stdlib.h>

// Libs
#include "x16graphics.h"
#include "sprites.h"

// Game specific
#include "startup.h"
#include "gametiles.h"
#include "gamesprites.h"
#include "memmap.h"
#include "level.h"
#include "layoutdefs.h"
#include "welcome.h"

// These managers are honestly just places to hold functions
// related to these entities
#include "levelmgr.h"
#include "enemymgr.h"
#include "levelutils.h"
#include "fontmgr.h"
#include "entitymgr.h"

// The "waitvsync" function is broken in r41
// People say to use this until fixed
#include "waitforjiffy.h"

#define PLAYER_FALL_SPEED 20
#define PLAYER_WATER_FALL_SPEED 4

#define PLAYER_JUMP_SPEED_NORMAL 20
#define PLAYER_JUMP_SPEED_BOOTS 24
#define PLAYER_WATER_JUMP_SPEED_NORMAL 12
#define PLAYER_WATER_JUMP_SPEED_BOOTS 14

#define PLAYER_JUMP_FRAMES 16
#define PLAYER_WATER_JUMP_FRAMES 16

#define DEATH_PAUSE_FRAMES = 120

unsigned char testMode = 0;

// A few top level structs to hold things that stay
// active throughout the game life
Sprite player, bullet, expSmall, ship;
LevelOveralLayout* level;

unsigned char energy = 0;
unsigned short gold = 0;
unsigned char lives = 2;
unsigned char hasScuba = 0;
unsigned char hasWeapon = 0;
unsigned char hasBoots = 0;

// This is a "fake" exit returned when the player has died
Exit playerDied = {
    255
};

/**
 * Parse the current level, draw the tiles, create the enemies,
 * and run the level until the player hits an exit.
*/
Exit* runLevel(unsigned char nextSpriteIndex, unsigned char lastTilesetId, unsigned char showShipScene) {
    unsigned char collision, joy, enemyCount, entityCount;
    unsigned char jumpFrames = 0;
    unsigned char releasedBtnAfterJump = 1;
    unsigned char updateHeader = 0;

    TileInfo tileCollision;
    Entity *entityCollision;
    Exit *exitCollision;
    Energy *energyCollision;
    Gold *goldCollision;

    AISprite *hitEnemy;
    Entrance *entrance;

    layerMapsClear();

    // Load the tileset for this level if it changed
    if (lastTilesetId != level->tileList->tilesetId) {
        tilesConfig(level->tileList->tilesetId);
    }

    lastTilesetId = level->tileList->tilesetId;

    // Draw the tiles and create enemies
    layerMapsLevelInit(level);
    drawGameHeader(gold, energy, lives, hasScuba, hasWeapon, hasBoots);
    enemyCount = enemiesCreate(level, nextSpriteIndex);
    nextSpriteIndex+= enemyCount;
    entityCount = entitiesCreate(level, nextSpriteIndex);
    nextSpriteIndex+= entityCount;

    if (level->levelNum == 0) {
        // On level 0 we need to show the ship
        ship.zDepth = BetweenL0L1;
        x16SpriteIdxSetZDepth(ship.index, ship.zDepth);
    }

    while (1) {
        waitforjiffy(); // Wait for screen to finish drawing

        // Special ship landing scene before the player can move
        if (showShipScene) {
            // testMode will speed up the ship so the intro doesn't last so long
            spriteMoveYL(&ship, ship.yL+(ship.speed+(ship.speed * testMode)));
            x16SpriteIdxSetXY(ship.index, ship.x, ship.y);
            if (ship.y >= 288) {
                showShipScene = 0;
                ship.animationFrame = ship.animationStopFrame;
                x16SpriteIdxSetGraphicsPointer(ship.index, ship.clrMode, ship.graphicsBank,
                    ship.graphicsAddress+(ship.animationFrame * ship.frameSize));

                player.zDepth = BetweenL0L1;
                x16SpriteIdxSetZDepth(player.index, player.zDepth);
                continue;
            }
            ship.animationCount++;
            if (ship.animationCount == ship.animationSpeed) {
                ship.animationCount=0;
                ship.animationFrame++;
                if (ship.animationFrame == ship.frames) {
                    ship.animationFrame = 0;
                }
                x16SpriteIdxSetGraphicsPointer(ship.index, ship.clrMode, ship.graphicsBank,
                    ship.graphicsAddress+(ship.animationFrame * ship.frameSize));
            }
            continue;
        }

        // See if player is touching an exit
        entityCollision = playerTouchingEntity(level->entityList, &player);
        if (entityCollision != 0) {
            if (entityCollision->entityType == ExitEnum) {
                exitCollision = (Exit*)entityCollision;
                // If this jumps somewhere on the same level, just move the player
                if (exitCollision->level == level->levelNum) {
                    entrance = findEntranceForExit((EntranceList*)(level->entityList), exitCollision->entranceId);
                    spriteMoveToTile(&player, entrance->x, entrance->y, TILE_PIXEL_WIDTH, TILE_PIXEL_HEIGHT);    
                    x16SpriteIdxSetXY(player.index, player.x, player.y);
                } else {
                    // This is jumping to another level. We need to cleanup this level.
                    // Clean up the enemies and return the exit info
                    enemiesReset(enemyCount);
                    entitiesReset(entityCount);

                    // Hide the ship if leaving level 0
                    if (level->levelNum == 0) {
                        ship.zDepth = Disabled;
                        x16SpriteIdxSetZDepth(ship.index, ship.zDepth);
                    }
                    // Reset any active bullets
                    bullet.active = 0;
                    bullet.zDepth = Disabled;
                    x16SpriteIdxSetZDepth(bullet.index, bullet.zDepth);

                    return exitCollision;
                }
            } else if (entityCollision->entityType == EnergyEnum) {
                energyCollision = (Energy*)entityCollision;
                energy+= energyCollision->amount;
                hideEntity(entityCount, entityCollision);
                updateHeader = 1;
            } else if (entityCollision->entityType == GoldEnum) {
                goldCollision = (Gold*)entityCollision;
                gold+= goldCollision->amount;
                hideEntity(entityCount, entityCollision);
                updateHeader = 1;
            } else if (entityCollision->entityType == ScubaEnum) {
                hasScuba = 1;
                hideEntity(entityCount, entityCollision);
                updateHeader = 1;
            } else if (entityCollision->entityType == WeaponEnum) {
                hasWeapon = 1;
                hideEntity(entityCount, entityCollision);
                updateHeader = 1;
            } else if (entityCollision->entityType == BootsEnum) {
                hasBoots = 1;
                hideEntity(entityCount, entityCollision);
                updateHeader = 1;
            } else if (entityCollision->entityType == ExtraLifeEnum) {
                lives++;
                hideEntity(entityCount, entityCollision);
                updateHeader = 1;
            }
        }

        enemiesMove(&player, enemyCount);
        enemyLasersMove(level);
        entitiesAnimate(entityCount);

        // Count game loops so we can animate sprites.
        // Only animate if the guy is "going" somewhere (don't shuffle his feet if standing still!)
        player.going=0;
        player.animationCount++;

        // We are changing the guys animation every 6 game loops
        // but hold at this count as we only animate if moving.
        if (player.animationCount > player.animationSpeed) {
            player.animationCount=player.animationSpeed;
        }

        joy = joy_read(0);

        // Falling
        // jumpFrames are the number of frames the player moves UP (jumps)
        // At 0, the player is always falling, even if on the ground.
        if (jumpFrames == 0) {
            // See what the player is currently touching to see his fall (or swim-fall) speed
            spriteTouchingTile(level, &player, &tileCollision);

            // The player falls in water too, just more slowly
            spriteMoveYL(&player, player.yL + (tileCollision.type == Empty ? PLAYER_FALL_SPEED : PLAYER_WATER_FALL_SPEED));
            spriteTouchingTile(level, &player, &tileCollision);

            // If the player is standing on a tile, a few things happen
            if (tileCollision.type != Empty) {
                // Move the player to the top of the tile
                // We do this because as the player falls they may end up wedged a few pixels into a tile
                if (tileCollision.type == Ground) {
                    spriteMoveY(&player, ((tileCollision.y * TILE_PIXEL_HEIGHT) - pixelSizes[player.height]) - 1);
                }

                // Since the player is touching something, see if they pressed jump button
                if (JOY_BTN_1(joy) || JOY_UP(joy)) {
                    // Only let them jump if they released the jump button since the last jump.
                    // Without this, the player just hops as you hold the button.
                    if (releasedBtnAfterJump == 1) {
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
        } else {
            // Player is jumping, move them up
            jumpFrames--;
            player.going==1;
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
            if (tileCollision.type == Ground) {
                spriteMoveBackY(&player);
            }
        }

        // Check the final tile the player is touching to see if the graphics need to change to scuba or back to running
        spriteTouchingTile(level, &player, &tileCollision);
        if ((tileCollision.type == Ground || tileCollision.type == Empty) && player.graphicsAddress != SPRITE_MEM_PLAYER) {
            player.graphicsAddress = SPRITE_MEM_PLAYER;
            x16SpriteIdxSetGraphicsPointer(player.index, player.clrMode, player.graphicsBank,
                player.graphicsAddress);
        } else if (tileCollision.type == Water) {
            if (!hasScuba) {
                // Player needs the Scuba gear to survive in water. DEAD!
                return &playerDied;
            }
            if (player.graphicsAddress != SPRITE_MEM_PLAYER_SCUBA) {
                player.graphicsAddress = SPRITE_MEM_PLAYER_SCUBA;
                x16SpriteIdxSetGraphicsPointer(player.index, player.clrMode, player.graphicsBank,
                    player.graphicsAddress);
            }
        }

        // Check if player is moving left/right
        if (JOY_LEFT(joy)) {
            player.going=1;
            // We also flip the animation depending on direction
            if (player.animationDirection != 0) {
                player.animationDirection=0;
                x16SpriteIdxSetHFlip(player.index, player.animationDirection);
            }
            spriteMoveXL(&player, player.xL-(tileCollision.type == Water ? player.swimSpeed : player.speed));
        } else if (JOY_RIGHT(joy)) {
            player.going=1;
            // Maybe flip animation
            if (player.animationDirection != 1) {
                player.animationDirection=1;
                x16SpriteIdxSetHFlip(player.index, player.animationDirection);
            }
            spriteMoveXL(&player, player.xL+(tileCollision.type == Water ? player.swimSpeed : player.speed));
        }

        // Change animation if jumping or moving and hit loop count
        if (jumpFrames > 0) {
            // No "jump" animation because it looked kinda dumb
            // Leaving this here though in case I change my mind later
        } else if (player.going==1 && player.animationCount == player.animationSpeed) {
            player.animationCount=0;
            player.animationFrame++;
            if (player.animationFrame == player.frames) {
                player.animationFrame = 0;
            }
            x16SpriteIdxSetGraphicsPointer(player.index, player.clrMode, player.graphicsBank,
                player.graphicsAddress+(player.animationFrame * player.frameSize));
        } else if (player.animationCount == player.animationSpeed && player.animationFrame != player.animationStopFrame) {
            // If the guy is standing still, always show a certain frame
            // In future this could be a totally different animation
            // We could have a yawn animation for instance for when waiting too long.
            player.animationFrame = player.animationStopFrame;
            x16SpriteIdxSetGraphicsPointer(player.index, player.clrMode, player.graphicsBank,
                player.graphicsAddress+(player.animationFrame * player.frameSize));
        }
        
        // Move the bullet (yes, the player can only have 1 active bullet right now)
        if (bullet.active == 1) {
            spriteMoveX(&bullet, bullet.animationDirection == 0 ? bullet.x-bullet.speed : bullet.x+bullet.speed);
            x16SpriteIdxSetXY(bullet.index, bullet.x, bullet.y);
        }

        // See if player is shooting
        if (JOY_BTN_2(joy) && bullet.active==0) {
            bullet.active = 1;
            bullet.animationDirection = player.animationDirection;
            bullet.startX = player.x;
            spriteMove(&bullet, player.x, player.y);
            // Show the bullet
            bullet.zDepth = BetweenL0L1;
            x16SpriteIdxSetZDepth(bullet.index, bullet.zDepth);
            x16SpriteIdxSetXY(bullet.index, bullet.x, bullet.y);
            x16SpriteIdxSetHFlip(bullet.index, bullet.animationDirection);
        }

        // See if the player is touching any tiles left/right
        // Move back if so.
        // With the current code we may end up checking tiles more than needed
        // but it's an O(1) operation since the tiles are in an array, so it should be fine. 
        spriteTouchingTile(level, &player, &tileCollision);
        if (tileCollision.type == Ground) {
            spriteMoveBackX(&player);
        }

        // Final position of the player. Update their sprite
        x16SpriteIdxSetXY(player.index, player.x, player.y);

        // Manage explosion sprites
        expSmall.animationCount++;
        if (expSmall.animationCount == expSmall.animationSpeed) {
            expSmall.animationCount=0;
            expSmall.animationFrame++;
            x16SpriteIdxSetGraphicsPointer(expSmall.index, expSmall.clrMode, expSmall.graphicsBank,
                expSmall.graphicsAddress+(expSmall.animationFrame * expSmall.frameSize));

            if (expSmall.animationFrame == expSmall.frames) {
                expSmall.zDepth = Disabled;
                x16SpriteIdxSetZDepth(expSmall.index, expSmall.zDepth);
            }
        }
            
        // Get the Collision bits and shift them down
        collision = x16SpriteCollisionBitsGet();

        // The collision bits will be the OVERLAP of the Collision Masks of the two sprites.
        // We can then look at all the sprites that have that bit in their mask
        // NOTE: Collisions seem to be triggered twice.
        // Could be because there are 2 sprites involved?
        // OR maybe at this point we haven't Disabled the sprite yet and its too late?
        // HACK: I check the "active" status on the bullet before considering this a valid collision
        // The collision system still helps a TON even with this.

        // Player and Enemy/Laser collisions
        if (collision == 0b1001 || collision == 0b0101) {
            // If laser hit, find and reset that laser
            resetClosestLaser(player.x, player.y);

            // TODO: Getting some false collisions, check the collisions here to make sure its valid
            // Move the sprite back to the start
            if (!testMode) {
                // Exit the level. The outer loop will reset.
                return &playerDied;
            }
        } else if (bullet.active == 1 && collision == 0b1010) {
            // This is a player bullet/enemy collision
            // Show explosion
            smallExplosion(&expSmall, BetweenL0L1, bullet.x, bullet.y);
            
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
        if (bullet.active == 1) {
            spriteTouchingTile(level, &bullet, &tileCollision);
            if (tileCollision.type == Ground || bullet.x < 0 || bullet.x > 639 || abs(bullet.x - bullet.startX) >= 128) {
                if (tileCollision.type == Ground) {
                    // Explosion
                    smallExplosion(&expSmall, InFrontOfL1, bullet.x, bullet.y);
                }
                bullet.active = 0;
                bullet.zDepth = Disabled;
                x16SpriteIdxSetZDepth(bullet.index, bullet.zDepth);
            }
        }

        // If anything in the header changes (gold, energy, lives, etc.), redraw it
        if (updateHeader) {
            updateHeader = 0;
            drawGameHeader(gold, energy, lives, hasScuba, hasWeapon, hasBoots);
        }
    }
}

void main() {
    unsigned char i;

    // CX16 has 128 sprites for us to use. Each has an id/index.
    // As we create sprites we increase this value so we know what the next
    // available sprite index is.
    unsigned char nextSpriteIndex = 0;

    // On the 1st level we show the ship landing before the game starts
    unsigned char showShipScene = 1;

    // When we go to the next level, we may need to load a new tileset
    // Use this to track the last tileset so we know if we need to load
    unsigned char lastTilesetId = 255;

    Exit exitCollision;
    Entrance *entrance;
    
    // Clear the cache
    // We cache some level data (entities) because certain things don't respawn (energy, gold)
    // so we need to remember which ones are gone
    initCachedLevelData();

    // Get the starting level and main entrance
    // It loads quickly but do it before showing title to minimize time after title screen
    level = levelGet(0);
    entrance = findEntranceForExit(((EntranceList*)level->entityList), 0);
    
    // Configure the joysticks
    joy_install(cx16_std_joy);
    
    testMode = showTitleScreen();

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

    // Wait to switch to game mode until everything is loaded
    // If you switch video modes first, you get crazy stuff on screen (kind cool?)
    videoConfig();

    // Load some standard tiles (font)
    // These stay in the Tileset RAM for the entire game
    // Level tilesets are loaded in RAM after
    standardTilesLoad();

    // Show the intro screen before starting the level
    showIntroScene(&ship);

    while(1) {
        // Get a copy of the exitCollision because we will free the level next
        exitCollision = *runLevel(nextSpriteIndex, lastTilesetId, showShipScene);
        showShipScene = 0;

        // If this was a normal level exit (not a player death)
        // The load the next level
        if (exitCollision.entityType != 255) {
            // Free the memory for the last level and load the next one
            freeLevel(level);
            level = levelGet(exitCollision.level);

            // Find the entrance the player is linking to and place them there
            entrance = findEntranceForExit(((EntranceList*)level->entityList), exitCollision.entranceId);
        } else {
            // Pause the game for a moment since player died
            // TODO: Come up with something better
            for (i=0; i<DEATH_PAUSE_FRAMES; i++) {
                waitforjiffy();
            }
        }

        spriteMoveToTile(&player, entrance->x, entrance->y, TILE_PIXEL_WIDTH, TILE_PIXEL_HEIGHT);    
        x16SpriteIdxSetXY(player.index, player.x, player.y);
    }

    // Disable sprite collisions before quitting
    x16SpriteCollisionsDisable();

    // TODO: Restore the text video mode
}