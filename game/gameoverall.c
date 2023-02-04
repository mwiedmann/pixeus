#include "level.h"

extern LevelOveralLayout level0;
extern LevelOveralLayout level1;

LevelOveralLayout* levelGet(unsigned short l) {
    switch(l) {
        case 0: return &level0;
        case 1: return &level1;
    }
}