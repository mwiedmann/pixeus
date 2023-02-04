#include "level.h"

EnemyLayout level0Enemies[1] = {
    {0, 0, 0, 0, 0}
};

EnemyLayoutList level0EnemyList = { 0, level0Enemies };

SolidLayout level0Solids[1] = {
    { 0, 22, 40, 0}
};

SolidLayoutList level0SolidList = { 1, level0Solids };

TileLayout level0Tiles[40] = {
    { 0, 22, 1, 2, 1, 0, 0 },
    { 1, 22, 1, 3, 1, 0, 0 },
    { 2, 22, 1, 4, 1, 0, 0 },
    { 3, 22, 1, 2, 1, 0, 0 },
    { 4, 22, 1, 3, 1, 0, 0 },
    { 5, 22, 1, 4, 1, 0, 0 },
    { 6, 22, 1, 2, 1, 0, 0 },
    { 7, 22, 1, 3, 1, 0, 0 },
    { 8, 22, 1, 4, 1, 0, 0 },
    { 9, 22, 1, 2, 1, 0, 0 },
    { 10, 22, 1, 3, 1, 0, 0 },
    { 11, 22, 1, 4, 1, 0, 0 },
    { 12, 22, 1, 2, 1, 0, 0 },
    { 13, 22, 1, 3, 1, 0, 0 },
    { 14, 22, 1, 4, 1, 0, 0 },
    { 15, 22, 1, 2, 1, 0, 0 },
    { 16, 22, 1, 3, 1, 0, 0 },
    { 17, 22, 1, 4, 1, 0, 0 },
    { 18, 22, 1, 2, 1, 0, 0 },
    { 19, 22, 1, 3, 1, 0, 0 },
    { 20, 22, 1, 4, 1, 0, 0 },
    { 21, 22, 1, 2, 1, 0, 0 },
    { 22, 22, 1, 3, 1, 0, 0 },
    { 23, 22, 1, 4, 1, 0, 0 },
    { 24, 22, 1, 2, 1, 0, 0 },
    { 25, 22, 1, 3, 1, 0, 0 },
    { 26, 22, 1, 4, 1, 0, 0 },
    { 27, 22, 1, 2, 1, 0, 0 },
    { 28, 22, 1, 3, 1, 0, 0 },
    { 29, 22, 1, 4, 1, 0, 0 },
    { 30, 22, 1, 2, 1, 0, 0 },
    { 31, 22, 1, 3, 1, 0, 0 },
    { 32, 22, 1, 4, 1, 0, 0 },
    { 33, 22, 1, 2, 1, 0, 0 },
    { 34, 22, 1, 3, 1, 0, 0 },
    { 35, 22, 1, 4, 1, 0, 0 },
    { 36, 22, 1, 2, 1, 0, 0 },
    { 37, 22, 1, 3, 1, 0, 0 },
    { 38, 22, 1, 4, 1, 0, 0 },
    { 39, 22, 1, 2, 1, 0, 0 }
};

TileLayoutList level0TilesList = { 40, level0Tiles };

Entrance level0Entrances[2] = {
    { 19, 21, "GameStart" },
    { 38, 21, "Right" }
};

EntranceList level0EntranceList = { 2, level0Entrances };

Exit level0Exits[1] = {
    { 39, 21, 1, "TopLeft" }
};

ExitList level0ExitList = { 1, level0Exits };

LevelOveralLayout level0 = {
    &level0SolidList,
    &level0TilesList,
    &level0EnemyList,
    &level0EntranceList,
    &level0ExitList
};
