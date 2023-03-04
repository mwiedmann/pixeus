// The "waitvsync" function is broken in r41
// People say to use this until fixed
#include "waitforjiffy.h"

// This uses the zsound lib
#include "soundmgr.h"

void loopUpdates() {
    playSoundsThisGameLoop();
    waitforjiffy();
}