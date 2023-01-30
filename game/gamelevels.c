#include "gamelevels.h"
#include "level.h"
#include "commontiles.h"

extern TileLayout cloudTiles1[];
extern TileLayout moonTiles[];

SolidLayout testLevelSolid[TEST_LEVEL_SOLID_LENGTH] = {
    { 7, 9, 1, 0},
    { 10, 8, 3, 0},
    { 0, 10, 20, 0},
    { 14, 16, 17, 0},
    { 7, 23, 25, 0},
    { 0, 29, 40, 0}
};

TileLayout testLevelTiles[TEST_LEVEL_TILES_LENGTH] = {
    { 7, 7, 1, 36, 0},
    { 7, 8, 1, 42, 0},
    { 7, 9, 1, 48, 0}, // Cactus blocks
    { 13, 9, 1, 6, 1},
    { 15, 9, 1, 19, 0},
    { 10, 8, 3, 255, 0}, // Floor
    { 0, 10, 20, 255, 1}, // Floor
    { 16, 15, 1, 17, 1},
    { 23, 15, 1, 49, 0},
    { 24, 15, 1, 50, 1},


    { 27, 11, 1, 31, 0}, // Tree start
    { 28, 11, 1, 32, 0},
    { 29, 11, 1, 33, 0},

    { 27, 12, 1, 37, 0},
    { 28, 12, 1, 38, 0},
    { 29, 12, 1, 39, 0},

    { 27, 13, 1, 43, 0},
    { 28, 13, 1, 44, 0},
    { 29, 13, 1, 45, 0},

    { 28, 14, 1, 42, 0},
    { 28, 15, 1, 48, 0}, // Tree End

    { 14, 16, 17, 255, 1}, // Floor
    { 7, 23, 25, 255, 1}, // Floor

    { 2, 22, 1, 14, 1}, // Tree start
    { 3, 22, 1, 15, 1},
    { 4, 22, 1, 16, 1},

    { 2, 23, 1, 20, 1},
    { 3, 23, 1, 21, 1},
    { 4, 23, 1, 22, 1},

    { 2, 24, 1, 26, 1},
    { 3, 24, 1, 27, 1},
    { 4, 24, 1, 28, 1},

    { 3, 25, 1, 42, 1},
    { 3, 26, 1, 42, 1},
    { 3, 27, 1, 42, 1},
    { 3, 28, 1, 48, 1}, // Tree End

    { 0, 29, 40, 255, 1} // Bottom
};

SolidLayoutList testLevelAllSolids[1] = {
    { TEST_LEVEL_SOLID_LENGTH, testLevelSolid }
};

TileLayoutList testLevelAllTiles[3] = {
    { TEST_LEVEL_TILES_LENGTH, testLevelTiles },
    { CLOUD_TILES_1_LENGTH, cloudTiles1 },
    { MOON_TILES_LENGTH, moonTiles }
};

EnemyLayout snakes[4] = {
    { 8, 9, 10, Snake, 0},
    { 14, 15, 17, Snake, 0},
    { 7, 22, 25, Snake, 0},
    { 0, 28, 40, Snake, 0}
};

EnemyLayout bees[3] = {
    { 14, 15, 17, Bee, 0},
    { 7, 22, 25, Bee, 0},
    { 0, 28, 40, Bee, 0}
};

EnemyLayoutList testLevelEnemies[2] = {
    { 4, snakes },
    { 3, bees }
};

LevelOveralLayout testLevel = {
    1,
    testLevelAllSolids,
    3,
    testLevelAllTiles,
    2,
    testLevelEnemies
};