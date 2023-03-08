#ifndef ENEMIES_H
#define ENEMIES_H

typedef enum
{
    Snake = 0,
    Bee = 1,
    Ghost = 2,
    Scorpion = 3,
    Wasp = 4,
    Fish1 = 5,
    BigGhost = 6,
    Eyeball = 7,
    Mushroom = 8,
    Slime = 9,
    Spider = 10,
    Rat = 11,
    Rockman = 12,
    Eel = 13,
    Iceman = 14,
    Snowball = 15,
    Dragonfly = 16,
    BigBear = 17,
    Clouds = 18,
    Flies = 19,
    Shark = 20,
    Bluehawk = 21,
    Beetle = 22,
    Jellyfish = 23
} EnemyType;

typedef enum
{
    Patrol = 0
} EnemyMoveType;

typedef enum
{
    Empty = 0,
    Ground = 1,
    Water = 2,
    Ice = 3,
    Lava = 4
} MovementTypes;

#endif