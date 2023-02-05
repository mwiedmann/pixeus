#include "level.h"

EnemyLayout level1Enemies[6] = {
    {27, 22, 5, 0, 0},
    {6, 4, 5, 1, 0},
    {16, 13, 5, 2, 0},
    {6, 28, 5, 3, 0},
    {26, 28, 5, 3, 0},
    {28, 12, 5, 4, 0}
};

EnemyLayoutList level1EnemyList = { 6, level1Enemies };

SolidLayout level1Solids[17] = {
    { 12, 2, 2, 0},
    { 10, 3, 2, 0},
    { 0, 5, 25, 0},
    { 27, 13, 13, 0},
    { 16, 14, 11, 0},
    { 29, 16, 1, 0},
    { 29, 17, 1, 0},
    { 30, 18, 7, 0},
    { 0, 19, 7, 0},
    { 38, 19, 2, 0},
    { 38, 21, 2, 0},
    { 23, 22, 2, 0},
    { 10, 23, 13, 0},
    { 25, 23, 15, 0},
    { 3, 27, 4, 0},
    { 21, 27, 3, 0},
    { 0, 29, 40, 0}
};

SolidLayoutList level1SolidList = { 17, level1Solids };

TileLayout level1Tiles[389] = {
    { 6, 1, 1, 13, 0, 0, 0 },
    { 17, 1, 1, 160, 0, 0, 0 },
    { 30, 1, 1, 162, 0, 0, 0 },
    { 1, 2, 1, 13, 0, 0, 0 },
    { 22, 2, 1, 171, 0, 0, 0 },
    { 35, 2, 1, 121, 0, 0, 0 },
    { 36, 2, 1, 122, 0, 0, 0 },
    { 37, 2, 1, 123, 0, 0, 0 },
    { 26, 3, 1, 16, 0, 0, 0 },
    { 27, 3, 1, 17, 0, 0, 0 },
    { 35, 3, 1, 133, 0, 0, 0 },
    { 36, 3, 1, 134, 0, 0, 0 },
    { 37, 3, 1, 135, 0, 0, 0 },
    { 1, 4, 1, 59, 0, 0, 0 },
    { 4, 4, 1, 102, 0, 0, 0 },
    { 11, 4, 1, 95, 0, 0, 0 },
    { 12, 4, 1, 59, 0, 0, 0 },
    { 15, 4, 1, 96, 0, 0, 0 },
    { 19, 4, 1, 180, 0, 0, 0 },
    { 22, 4, 1, 179, 0, 0, 0 },
    { 32, 4, 1, 172, 0, 0, 0 },
    { 35, 4, 1, 145, 0, 0, 0 },
    { 36, 4, 1, 146, 0, 0, 0 },
    { 37, 4, 1, 147, 0, 0, 0 },
    { 28, 6, 1, 13, 0, 0, 0 },
    { 35, 7, 1, 18, 0, 0, 0 },
    { 36, 7, 1, 25, 0, 0, 0 },
    { 28, 9, 1, 61, 0, 0, 0 },
    { 29, 9, 1, 62, 0, 0, 0 },
    { 30, 9, 1, 63, 0, 0, 0 },
    { 28, 10, 1, 73, 0, 0, 0 },
    { 29, 10, 1, 74, 0, 0, 0 },
    { 30, 10, 1, 75, 0, 0, 0 },
    { 10, 11, 1, 170, 0, 0, 0 },
    { 28, 11, 1, 85, 0, 0, 0 },
    { 29, 11, 1, 86, 0, 0, 0 },
    { 30, 11, 1, 87, 0, 0, 0 },
    { 29, 12, 1, 91, 0, 0, 0 },
    { 34, 12, 1, 57, 0, 0, 0 },
    { 35, 12, 1, 58, 0, 0, 0 },
    { 36, 12, 1, 59, 0, 0, 0 },
    { 20, 13, 1, 69, 0, 0, 0 },
    { 21, 13, 1, 70, 0, 0, 0 },
    { 22, 13, 1, 71, 0, 0, 0 },
    { 23, 13, 1, 110, 0, 0, 0 },
    { 25, 13, 1, 97, 0, 0, 0 },
    { 4, 14, 1, 26, 0, 0, 0 },
    { 5, 14, 1, 27, 0, 0, 0 },
    { 6, 14, 1, 28, 0, 0, 0 },
    { 13, 14, 1, 162, 0, 0, 0 },
    { 4, 15, 1, 38, 0, 0, 0 },
    { 5, 15, 1, 39, 0, 0, 0 },
    { 6, 15, 1, 40, 0, 0, 0 },
    { 4, 16, 1, 50, 0, 0, 0 },
    { 5, 16, 1, 51, 0, 0, 0 },
    { 6, 16, 1, 52, 0, 0, 0 },
    { 32, 16, 1, 127, 0, 0, 0 },
    { 33, 16, 1, 128, 0, 0, 0 },
    { 5, 17, 1, 78, 0, 0, 0 },
    { 32, 17, 1, 139, 0, 0, 0 },
    { 33, 17, 1, 140, 0, 0, 0 },
    { 1, 18, 1, 112, 0, 0, 0 },
    { 4, 18, 1, 98, 0, 0, 0 },
    { 5, 18, 1, 90, 0, 0, 0 },
    { 10, 20, 1, 143, 0, 0, 0 },
    { 11, 20, 1, 144, 0, 0, 0 },
    { 10, 21, 1, 155, 0, 0, 0 },
    { 11, 21, 1, 156, 0, 0, 0 },
    { 10, 22, 1, 167, 0, 0, 0 },
    { 11, 22, 1, 168, 0, 0, 0 },
    { 18, 22, 1, 57, 0, 0, 0 },
    { 19, 22, 1, 58, 0, 0, 0 },
    { 20, 22, 1, 59, 0, 0, 0 },
    { 22, 22, 1, 113, 0, 0, 0 },
    { 25, 22, 1, 115, 0, 0, 0 },
    { 28, 22, 1, 81, 0, 0, 0 },
    { 29, 22, 1, 82, 0, 0, 0 },
    { 30, 22, 1, 83, 0, 0, 0 },
    { 35, 22, 1, 93, 0, 0, 0 },
    { 36, 22, 1, 94, 0, 0, 0 },
    { 4, 26, 1, 42, 0, 0, 0 },
    { 21, 26, 1, 154, 0, 0, 0 },
    { 23, 26, 1, 154, 0, 0, 0 },
    { 0, 28, 1, 53, 0, 0, 0 },
    { 1, 28, 1, 54, 0, 0, 0 },
    { 9, 28, 1, 101, 0, 0, 0 },
    { 12, 28, 1, 57, 0, 0, 0 },
    { 13, 28, 1, 58, 0, 0, 0 },
    { 14, 28, 1, 59, 0, 0, 0 },
    { 30, 28, 1, 37, 0, 0, 0 },
    { 32, 28, 1, 49, 0, 0, 0 },
    { 12, 2, 1, 1, 0, 0, 0 },
    { 13, 2, 1, 2, 0, 0, 0 },
    { 10, 3, 1, 2, 0, 0, 0 },
    { 11, 3, 1, 3, 0, 0, 0 },
    { 12, 3, 1, 169, 0, 1, 1 },
    { 0, 5, 1, 2, 0, 0, 0 },
    { 1, 5, 1, 3, 0, 0, 0 },
    { 2, 5, 1, 4, 0, 0, 0 },
    { 3, 5, 1, 2, 0, 0, 0 },
    { 4, 5, 1, 3, 0, 0, 0 },
    { 5, 5, 1, 4, 0, 0, 0 },
    { 6, 5, 1, 2, 0, 0, 0 },
    { 7, 5, 1, 3, 0, 0, 0 },
    { 8, 5, 1, 4, 0, 0, 0 },
    { 9, 5, 1, 2, 0, 0, 0 },
    { 10, 5, 1, 3, 0, 0, 0 },
    { 11, 5, 1, 4, 0, 0, 0 },
    { 12, 5, 1, 2, 0, 0, 0 },
    { 13, 5, 1, 3, 0, 0, 0 },
    { 14, 5, 1, 4, 0, 0, 0 },
    { 15, 5, 1, 2, 0, 0, 0 },
    { 16, 5, 1, 3, 0, 0, 0 },
    { 17, 5, 1, 4, 0, 0, 0 },
    { 18, 5, 1, 2, 0, 0, 0 },
    { 19, 5, 1, 3, 0, 0, 0 },
    { 20, 5, 1, 4, 0, 0, 0 },
    { 21, 5, 1, 2, 0, 0, 0 },
    { 22, 5, 1, 3, 0, 0, 0 },
    { 23, 5, 1, 4, 0, 0, 0 },
    { 24, 5, 1, 5, 0, 0, 0 },
    { 24, 6, 1, 157, 0, 0, 0 },
    { 24, 7, 1, 158, 0, 0, 0 },
    { 24, 8, 1, 159, 0, 0, 0 },
    { 8, 9, 1, 169, 0, 0, 0 },
    { 9, 9, 1, 2, 0, 1, 0 },
    { 10, 9, 1, 3, 0, 1, 0 },
    { 11, 9, 1, 4, 0, 1, 0 },
    { 12, 9, 1, 2, 0, 1, 0 },
    { 13, 9, 1, 3, 0, 1, 0 },
    { 14, 9, 1, 4, 0, 1, 0 },
    { 15, 9, 1, 2, 0, 1, 0 },
    { 16, 9, 1, 3, 0, 1, 0 },
    { 17, 9, 1, 4, 0, 1, 0 },
    { 18, 9, 1, 2, 0, 1, 0 },
    { 19, 9, 1, 3, 0, 1, 0 },
    { 20, 9, 1, 4, 0, 1, 0 },
    { 21, 9, 1, 2, 0, 1, 0 },
    { 22, 9, 1, 3, 0, 1, 0 },
    { 23, 9, 1, 4, 0, 1, 0 },
    { 24, 9, 1, 5, 0, 1, 0 },
    { 8, 10, 1, 157, 0, 0, 0 },
    { 8, 11, 1, 157, 0, 0, 0 },
    { 1, 12, 1, 169, 0, 0, 0 },
    { 2, 12, 1, 2, 0, 1, 0 },
    { 3, 12, 1, 3, 0, 1, 0 },
    { 4, 12, 1, 4, 0, 1, 0 },
    { 5, 12, 1, 2, 0, 1, 0 },
    { 6, 12, 1, 3, 0, 1, 0 },
    { 7, 12, 1, 4, 0, 1, 0 },
    { 8, 12, 1, 5, 0, 1, 0 },
    { 1, 13, 1, 157, 0, 0, 0 },
    { 27, 13, 1, 1, 0, 0, 0 },
    { 28, 13, 1, 2, 0, 0, 0 },
    { 29, 13, 1, 3, 0, 0, 0 },
    { 30, 13, 1, 4, 0, 0, 0 },
    { 31, 13, 1, 2, 0, 0, 0 },
    { 32, 13, 1, 3, 0, 0, 0 },
    { 33, 13, 1, 4, 0, 0, 0 },
    { 34, 13, 1, 2, 0, 0, 0 },
    { 35, 13, 1, 3, 0, 0, 0 },
    { 36, 13, 1, 4, 0, 0, 0 },
    { 37, 13, 1, 2, 0, 0, 0 },
    { 38, 13, 1, 3, 0, 0, 0 },
    { 39, 13, 1, 4, 0, 0, 0 },
    { 1, 14, 1, 158, 0, 0, 0 },
    { 16, 14, 1, 1, 0, 0, 0 },
    { 17, 14, 1, 4, 0, 0, 0 },
    { 18, 14, 1, 2, 0, 0, 0 },
    { 19, 14, 1, 3, 0, 0, 0 },
    { 20, 14, 1, 4, 0, 0, 0 },
    { 21, 14, 1, 2, 0, 0, 0 },
    { 22, 14, 1, 3, 0, 0, 0 },
    { 23, 14, 1, 4, 0, 0, 0 },
    { 24, 14, 1, 2, 0, 0, 0 },
    { 25, 14, 1, 3, 0, 0, 0 },
    { 26, 14, 1, 4, 0, 0, 0 },
    { 27, 14, 1, 169, 0, 1, 1 },
    { 0, 15, 1, 4, 0, 1, 0 },
    { 1, 15, 1, 5, 0, 1, 0 },
    { 16, 15, 1, 157, 0, 0, 1 },
    { 29, 15, 1, 169, 0, 0, 0 },
    { 30, 15, 1, 4, 0, 1, 0 },
    { 31, 15, 1, 2, 0, 1, 0 },
    { 32, 15, 1, 3, 0, 1, 0 },
    { 33, 15, 1, 4, 0, 1, 0 },
    { 34, 15, 1, 2, 0, 1, 0 },
    { 35, 15, 1, 3, 0, 1, 0 },
    { 36, 15, 1, 4, 0, 1, 0 },
    { 37, 15, 1, 2, 0, 1, 0 },
    { 38, 15, 1, 3, 0, 1, 0 },
    { 39, 15, 1, 4, 0, 1, 0 },
    { 16, 16, 1, 1, 0, 1, 0 },
    { 17, 16, 1, 2, 0, 1, 0 },
    { 18, 16, 1, 3, 0, 1, 0 },
    { 19, 16, 1, 4, 0, 1, 0 },
    { 20, 16, 1, 2, 0, 1, 0 },
    { 21, 16, 1, 3, 0, 1, 0 },
    { 22, 16, 1, 4, 0, 1, 0 },
    { 23, 16, 1, 2, 0, 1, 0 },
    { 24, 16, 1, 3, 0, 1, 0 },
    { 25, 16, 1, 4, 0, 1, 0 },
    { 26, 16, 1, 169, 0, 0, 1 },
    { 29, 16, 1, 157, 0, 0, 0 },
    { 26, 17, 1, 157, 0, 0, 1 },
    { 29, 17, 1, 158, 0, 0, 0 },
    { 26, 18, 1, 158, 0, 0, 1 },
    { 29, 18, 1, 169, 0, 1, 0 },
    { 30, 18, 1, 2, 0, 0, 0 },
    { 31, 18, 1, 3, 0, 0, 0 },
    { 32, 18, 1, 4, 0, 0, 0 },
    { 33, 18, 1, 2, 0, 0, 0 },
    { 34, 18, 1, 3, 0, 0, 0 },
    { 35, 18, 1, 4, 0, 0, 0 },
    { 36, 18, 1, 5, 0, 0, 0 },
    { 0, 19, 1, 2, 0, 0, 0 },
    { 1, 19, 1, 3, 0, 0, 0 },
    { 2, 19, 1, 4, 0, 0, 0 },
    { 3, 19, 1, 2, 0, 0, 0 },
    { 4, 19, 1, 3, 0, 0, 0 },
    { 5, 19, 1, 4, 0, 0, 0 },
    { 6, 19, 1, 5, 0, 0, 0 },
    { 26, 19, 1, 1, 0, 1, 0 },
    { 27, 19, 1, 2, 0, 1, 0 },
    { 28, 19, 1, 3, 0, 1, 0 },
    { 29, 19, 1, 4, 0, 1, 0 },
    { 30, 19, 1, 2, 0, 1, 0 },
    { 31, 19, 1, 3, 0, 1, 0 },
    { 32, 19, 1, 4, 0, 1, 0 },
    { 33, 19, 1, 2, 0, 1, 0 },
    { 34, 19, 1, 3, 0, 1, 0 },
    { 35, 19, 1, 4, 0, 1, 0 },
    { 36, 19, 1, 5, 0, 1, 0 },
    { 38, 19, 1, 1, 0, 0, 0 },
    { 39, 19, 1, 2, 0, 0, 0 },
    { 6, 20, 1, 158, 0, 0, 0 },
    { 0, 21, 1, 2, 0, 1, 0 },
    { 1, 21, 1, 3, 0, 1, 0 },
    { 2, 21, 1, 4, 0, 1, 0 },
    { 3, 21, 1, 2, 0, 1, 0 },
    { 4, 21, 1, 3, 0, 1, 0 },
    { 5, 21, 1, 4, 0, 1, 0 },
    { 6, 21, 1, 5, 0, 1, 0 },
    { 38, 21, 1, 1, 0, 0, 0 },
    { 39, 21, 1, 2, 0, 0, 0 },
    { 23, 22, 1, 1, 0, 0, 0 },
    { 24, 22, 1, 5, 0, 0, 0 },
    { 10, 23, 1, 1, 0, 0, 0 },
    { 11, 23, 1, 2, 0, 0, 0 },
    { 12, 23, 1, 3, 0, 0, 0 },
    { 13, 23, 1, 4, 0, 0, 0 },
    { 14, 23, 1, 2, 0, 0, 0 },
    { 15, 23, 1, 3, 0, 0, 0 },
    { 16, 23, 1, 4, 0, 0, 0 },
    { 17, 23, 1, 2, 0, 0, 0 },
    { 18, 23, 1, 3, 0, 0, 0 },
    { 19, 23, 1, 4, 0, 0, 0 },
    { 20, 23, 1, 2, 0, 0, 0 },
    { 21, 23, 1, 3, 0, 0, 0 },
    { 22, 23, 1, 4, 0, 0, 0 },
    { 23, 23, 1, 169, 0, 1, 1 },
    { 24, 23, 1, 169, 0, 1, 0 },
    { 25, 23, 1, 2, 0, 0, 0 },
    { 26, 23, 1, 3, 0, 0, 0 },
    { 27, 23, 1, 4, 0, 0, 0 },
    { 28, 23, 1, 2, 0, 0, 0 },
    { 29, 23, 1, 3, 0, 0, 0 },
    { 30, 23, 1, 4, 0, 0, 0 },
    { 31, 23, 1, 2, 0, 0, 0 },
    { 32, 23, 1, 3, 0, 0, 0 },
    { 33, 23, 1, 4, 0, 0, 0 },
    { 34, 23, 1, 2, 0, 0, 0 },
    { 35, 23, 1, 3, 0, 0, 0 },
    { 36, 23, 1, 4, 0, 0, 0 },
    { 37, 23, 1, 2, 0, 0, 0 },
    { 38, 23, 1, 3, 0, 0, 0 },
    { 39, 23, 1, 4, 0, 0, 0 },
    { 3, 27, 1, 1, 0, 0, 0 },
    { 4, 27, 1, 2, 0, 0, 0 },
    { 5, 27, 1, 3, 0, 0, 0 },
    { 6, 27, 1, 5, 0, 0, 0 },
    { 21, 27, 1, 1, 0, 0, 0 },
    { 22, 27, 1, 2, 0, 0, 0 },
    { 23, 27, 1, 5, 0, 0, 0 },
    { 0, 29, 1, 2, 0, 0, 0 },
    { 1, 29, 1, 3, 0, 0, 0 },
    { 2, 29, 1, 4, 0, 0, 0 },
    { 3, 29, 1, 2, 0, 0, 0 },
    { 4, 29, 1, 3, 0, 0, 0 },
    { 5, 29, 1, 4, 0, 0, 0 },
    { 6, 29, 1, 2, 0, 0, 0 },
    { 7, 29, 1, 3, 0, 0, 0 },
    { 8, 29, 1, 4, 0, 0, 0 },
    { 9, 29, 1, 2, 0, 0, 0 },
    { 10, 29, 1, 3, 0, 0, 0 },
    { 11, 29, 1, 4, 0, 0, 0 },
    { 12, 29, 1, 2, 0, 0, 0 },
    { 13, 29, 1, 3, 0, 0, 0 },
    { 14, 29, 1, 4, 0, 0, 0 },
    { 15, 29, 1, 2, 0, 0, 0 },
    { 16, 29, 1, 3, 0, 0, 0 },
    { 17, 29, 1, 4, 0, 0, 0 },
    { 18, 29, 1, 2, 0, 0, 0 },
    { 19, 29, 1, 3, 0, 0, 0 },
    { 20, 29, 1, 4, 0, 0, 0 },
    { 21, 29, 1, 2, 0, 0, 0 },
    { 22, 29, 1, 3, 0, 0, 0 },
    { 23, 29, 1, 4, 0, 0, 0 },
    { 24, 29, 1, 2, 0, 0, 0 },
    { 25, 29, 1, 3, 0, 0, 0 },
    { 26, 29, 1, 4, 0, 0, 0 },
    { 27, 29, 1, 2, 0, 0, 0 },
    { 28, 29, 1, 3, 0, 0, 0 },
    { 29, 29, 1, 4, 0, 0, 0 },
    { 30, 29, 1, 2, 0, 0, 0 },
    { 31, 29, 1, 3, 0, 0, 0 },
    { 32, 29, 1, 4, 0, 0, 0 },
    { 33, 29, 1, 2, 0, 0, 0 },
    { 34, 29, 1, 3, 0, 0, 0 },
    { 35, 29, 1, 4, 0, 0, 0 },
    { 36, 29, 1, 2, 0, 0, 0 },
    { 37, 29, 1, 3, 0, 0, 0 },
    { 38, 29, 1, 4, 0, 0, 0 },
    { 39, 29, 1, 2, 0, 0, 0 },
    { 3, 2, 1, 66, 1, 0, 0 },
    { 3, 3, 1, 78, 1, 0, 0 },
    { 2, 4, 1, 30, 1, 0, 0 },
    { 3, 4, 1, 90, 1, 0, 0 },
    { 5, 4, 1, 53, 1, 0, 0 },
    { 13, 4, 1, 101, 1, 0, 0 },
    { 14, 4, 1, 81, 1, 0, 0 },
    { 20, 4, 1, 114, 1, 0, 0 },
    { 21, 4, 1, 115, 1, 0, 0 },
    { 30, 6, 1, 10, 1, 0, 0 },
    { 31, 6, 1, 11, 1, 0, 0 },
    { 32, 6, 1, 12, 1, 0, 0 },
    { 30, 7, 1, 22, 1, 0, 0 },
    { 31, 7, 1, 23, 1, 0, 0 },
    { 32, 7, 1, 24, 1, 0, 0 },
    { 30, 8, 1, 34, 1, 0, 0 },
    { 31, 8, 1, 35, 1, 0, 0 },
    { 32, 8, 1, 36, 1, 0, 0 },
    { 31, 9, 1, 55, 1, 0, 0 },
    { 31, 10, 1, 67, 1, 0, 0 },
    { 31, 11, 1, 79, 1, 0, 0 },
    { 31, 12, 1, 91, 1, 0, 0 },
    { 33, 12, 1, 49, 1, 0, 0 },
    { 18, 13, 1, 53, 1, 0, 0 },
    { 19, 13, 1, 41, 1, 0, 0 },
    { 24, 13, 1, 98, 1, 0, 0 },
    { 34, 17, 1, 81, 1, 0, 0 },
    { 35, 17, 1, 59, 1, 0, 0 },
    { 2, 18, 1, 112, 1, 0, 0 },
    { 3, 18, 1, 98, 1, 0, 0 },
    { 38, 18, 1, 54, 1, 0, 0 },
    { 39, 18, 1, 30, 1, 0, 0 },
    { 15, 20, 1, 107, 1, 0, 0 },
    { 16, 20, 1, 108, 1, 0, 0 },
    { 34, 20, 1, 64, 1, 0, 0 },
    { 38, 20, 1, 6, 1, 0, 0 },
    { 39, 20, 1, 46, 1, 0, 0 },
    { 15, 21, 1, 119, 1, 0, 0 },
    { 16, 21, 1, 120, 1, 0, 0 },
    { 23, 21, 1, 180, 1, 0, 0 },
    { 24, 21, 1, 179, 1, 0, 0 },
    { 34, 21, 1, 76, 1, 0, 0 },
    { 4, 22, 1, 7, 1, 0, 0 },
    { 5, 22, 1, 8, 1, 0, 0 },
    { 6, 22, 1, 9, 1, 0, 0 },
    { 12, 22, 1, 37, 1, 0, 0 },
    { 15, 22, 1, 131, 1, 0, 0 },
    { 16, 22, 1, 132, 1, 0, 0 },
    { 17, 22, 1, 102, 1, 0, 0 },
    { 33, 22, 1, 114, 1, 0, 0 },
    { 34, 22, 1, 88, 1, 0, 0 },
    { 4, 23, 1, 19, 1, 0, 0 },
    { 5, 23, 1, 20, 1, 0, 0 },
    { 6, 23, 1, 21, 1, 0, 0 },
    { 4, 24, 1, 31, 1, 0, 0 },
    { 5, 24, 1, 32, 1, 0, 0 },
    { 6, 24, 1, 33, 1, 0, 0 },
    { 5, 25, 1, 78, 1, 0, 0 },
    { 5, 26, 1, 90, 1, 0, 0 },
    { 22, 26, 1, 166, 1, 0, 0 },
    { 31, 26, 1, 65, 1, 0, 0 },
    { 31, 27, 1, 77, 1, 0, 0 },
    { 10, 28, 1, 93, 1, 0, 0 },
    { 11, 28, 1, 94, 1, 0, 0 },
    { 31, 28, 1, 89, 1, 0, 0 }
};

TileLayoutList level1TilesList = { 389, level1Tiles };

Entrance level1Entrances[1] = {
    { 1, 4, "TopLeft" }
};

EntranceList level1EntranceList = { 1, level1Entrances };

Exit level1Exits[1] = {
    { 0, 4, 0, "Right" }
};

ExitList level1ExitList = { 1, level1Exits };

LevelOveralLayout level1 = {
    &level1SolidList,
    &level1TilesList,
    &level1EnemyList,
    &level1EntranceList,
    &level1ExitList
};
