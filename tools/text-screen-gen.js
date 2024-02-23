const fs = require("fs");

/*
drawCenteredTextRow("PIXEUS THE EXPLORER", 0, 3);
    drawCenteredTextRow("ABOARD 'THE ARGO'", 0, 4); 
    
    drawCenteredTextRow("LOST IN SPACE", 0, 6);
    drawCenteredTextRow("AND LOW ON ENERGY", 0, 7);

    drawCenteredTextRow("PERHAPS THIS PLANET PROVIDES HOPE?", 0, 9);

    drawCenteredTextRow("PLENTIFUL IN GOLD AND ENERGY", 0, 11);
    drawCenteredTextRow("PIXEUS LANDS IN THIS STRANGE WORLD", 0, 12);

    drawCenteredTextRow("ESCAPE IS THE GOAL", 0, 14);
    drawCenteredTextRow("BUT GOLD IS THE SIREN'S CALL", 0, 15);

    A-Z 193-218
    0-9 48-57
    : 58
    . 46
    ? 63
    - 45
    ! 33
    ' 39
*/

const codes = {
  ' ': 32,
  A: 193,
  B: 194,
  C: 195,
  D: 196,
  E: 197,
  F: 198,
  G: 199,
  H: 200,
  I: 201,
  J: 202,
  K: 203,
  L: 204,
  M: 205,
  N: 206,
  O: 207,
  P: 208,
  Q: 209,
  R: 210,
  S: 211,
  T: 212,
  U: 213,
  V: 214,
  W: 215,
  X: 216,
  Y: 217,
  Z: 218,
  0: 48,
  1: 49,
  2: 50,
  3: 51,
  4: 52,
  5: 53,
  6: 54,
  7: 55,
  8: 56,
  9: 57,
  ":": 58,
  ".": 46,
  "?": 63,
  "-": 45,
  "!": 33,
  "'": 39,
};

const welcomeScreen = [
    { text: "PIXEUS THE EXPLORER", row: 3 },
    { text: "ABOARD 'THE ARGO'", row: 4},
    { text: "LOST IN SPACE", row: 6},
    { text: "AND LOW ON ENERGY", row: 7},
    { text: "PERHAPS THIS PLANET PROVIDES HOPE?", row: 9},
    { text: "PLENTIFUL IN GOLD AND ENERGY", row: 11},
    { text: "PIXEUS LANDS IN THIS STRANGE WORLD", row: 12},
    { text: "ESCAPE IS THE GOAL", row: 14},
    { text: "BUT GOLD IS THE SIREN'S CALL", row: 15}
]

const instructionsScreen = [
    { text: "HOW TO PLAY", row: 1 },
    { text: " MOVE - JOYSTICK LEFT-RIGHT", row: 3, col: 5 },
    { text: " JUMP - BUTTON 1 OR JOYSTICK UP", row: 4, col: 5 },
    { text: "SHOOT - BUTTON 2", row: 5, col: 5 },
    { text: "FIND AT LEAST 10 ENERGY AND", row: 7 },
    { text: "RETURN TO YOUR SHIP TO ESCAPE.", row: 8 },
    { text: "THE MORE GOLD AND ENERGY YOU FIND", row: 9 },
    { text: "THE GREATER YOUR ENDING.", row: 10 },
    { text: "YOUR SHIELD PROVIDES SOME PROTECTION.", row: 12 },
    { text: "FIND ENERGY TO RESTORE IT.", row: 13 },
    { text: "WATER IS:", row: 15 },
    { text: "DEADLY UNTIL YOU FIND SCUBA GEAR", row: 16 },
    { text: "YOUR FRIEND IN THE DESERT", row: 17 },
    { text: "BONE CHILLING IN THE TUNDRA", row: 18 },
    { text: "GOOD LUCK!", row: 20 }
]

const gameOverScreen = [
    { text: "PIXEUS SURRENDERS TO SLEEP AT LAST", row: 7 },
    { text: "NO LONGER KEEPING WATCH", row: 9 },
    { text: "THROUGH THE NIGHT", row: 10 },
    { text: "PERHAPS IN THE NEXT LIFE PIXEUS", row: 12 },
    { text: "YOU'LL COME UP FROM UNDER", row: 13 },
    { text: "ALL YOUR TROUBLES", row: 14 },
    { text: "GAME OVER", row: 17 },
]

const noGoldVictory = [
    { text: "WITH THE ARGO FULLY ENERGIZED", row: 3 },
    { text: "PIXEUS ESCAPES THIS STRANGE PLANET", row: 4 },
    { text: "LEAVING WITH 0 GOLD", row: 6 },
    { text: "PIXEUS DECIDES THAT HEALTH", row: 7 },
    { text: "AND MAKING IT SAFELY HOME", row: 8 },
    { text: "ARE MORE IMPORTANT THAN WEALTH", row: 9 },
    { text: "PERHAPS GREED", row: 11 },
    { text: "FOR A LACK OF A BETTER WORD", row: 12 },
    { text: "IS NOT GOOD?", row: 13 }
]

const col = 3

const goldScreen = [
    { text: "WITH THE ARGO ENERGIZED", row: 3 },
    { text: "PIXEUS ESCAPES THIS STRANGE PLANET", row: 4 },
    { text: "- LATE FEES FOR TROPIC OF CANCER", row: 8, col },
    { text: "- THE WHITE ALBUM - AGAIN", row: 9, col, arg: 1000 },
    { text: "- TOPPS CARD 482", row: 10, col, arg: 2000 },
    { text: "- A BAG OF ZENNY COINS", row: 11, col, arg: 3000 },
    { text: "- A BANKSY - BUZZ. RIP. NO!", row: 12, col, arg: 4000 },
    { text: "- A TOWEL", row: 13, col, arg: 5000 },
    { text: "- SHARES OF ENRON", row: 14, col, arg: 6000 },
    { text: "- CROCS. HEY THEY'RE COMFY!", row: 15, col, arg: 7000 },
    { text: "- A HARRIER JET", row: 16, col, arg: 8000 },
    { text: "- LOVE. PAUL: WHAT DID I SAY?!?", row: 17, col, arg: 9000 },
    { text: "- A RING 'ONE RING TO...' SHIT!", row: 18, col, arg: 9500 },
    { text: "- 30 SLIDERS 5 FRECH FRIES AND", row: 19, col, arg: 9999 },
    { text: "  4 LARGE CHERRY COKES", row: 20, col, arg: 9999 },
    { text: "CONGRATS YOU FOUND ALL THE GOLD!", row: 22, col, arg: 9999 }
]

const energyScreen = [
    { text: "WITH THE EXTRA ENERGY IN THE ARGO", row: 3 },
    { text: "PIXEUS EXPLORES THE GALAXY", row: 4 },
    { text: "- THE ASTROPOLOOZA FESTIVAL", row: 8, col, arg: 5 },
    { text: "- KLINGONS AROUND URANUS", row: 9, col, arg: 10 },
    { text: "- THE FORD GALAXY", row: 10, col, arg: 20 },
    { text: "- FAIRIES AT THE BOTTOM OF THE GARDEN", row: 11, col, arg: 30 },
    { text: "- THE CANTINA AND SHOOTS FIRST", row: 12, col, arg: 40 },
    { text: "- A RELAXING SPA ON GIEDI PRIME", row: 13, col, arg: 50 },
    { text: "- KAMINO TO CHECK ON A PROJECT", row: 14, col, arg: 60 },
    { text: "- THE VINEYARDS OF LA BARRE FRANCE", row: 15, col, arg: 70 },
    { text: "- GALLIFREY TO SEE THE POLICE", row: 16, col, arg: 80 },
    { text: "- MELMAC TO PICK UP CAT FROM KENNEL", row: 17, col, arg: 90 },
    { text: "- A COMEDY SHOW ON ORK", row: 18, col, arg: 95 },
    { text: "- SINIBON AT THE MALL - I HUNGER!", row: 19, col, arg: 99 },
    { text: "CONGRATS YOU FOUND ALL THE ENERGY!", row: 21, col, arg: 99 }
]

const getTextBytes = (text) => {
    const textBytes = []
    for (let i=0; i<40; i++) {
        textBytes.push(text[i] === undefined ? 0 : codes[text[i]])
    }

    return textBytes;
}

// Added a 0, 0 2 byte header required for cbm_k_load
// Its an optional address to load into. We don't use it but its required
const saveFile = (outputFilename, bytes) => {
    const output = new Uint8Array(bytes);
    fs.writeFileSync(outputFilename, output, "binary");
}

const shortHi = (s) => s>>8
const shortLo = (s) => s&255

/*
1 byte - row
1 byte - col (255 = center)
2 bytes - arg (used for gold amount compare)
41 bytes ending with 0 - text
*/
const createTextRecord = (rec) => {
    return [
        rec.row,
        rec.col === undefined ? 255 : rec.col,
        shortLo(rec.arg ?? 0),
        shortHi(rec.arg ?? 0),
        ...getTextBytes(rec.text),
        0 // null terminator
    ]
}

// header record 1 byte, number of records
const welcomeBytes = [
    welcomeScreen.length
]

welcomeScreen.forEach(rec => {
    welcomeBytes.push(...createTextRecord(rec))
})

saveFile('../bundle/WELCOME.BIN', welcomeBytes);

// ***********************************************
// Instructions Screen
// ***********************************************
const instructionBytes = [
    instructionsScreen.length
]

instructionsScreen.forEach(rec => {
    instructionBytes.push(...createTextRecord(rec))
})

saveFile('../bundle/INSTR.BIN', instructionBytes);

// ***********************************************
// Game Over Screen
// ***********************************************
const gameOverBytes = [
    gameOverScreen.length
]

gameOverScreen.forEach(rec => {
    gameOverBytes.push(...createTextRecord(rec))
})

saveFile('../bundle/GAMEOVER.BIN', gameOverBytes);

// ***********************************************
// No Gold Victory Screen
// ***********************************************
const noGoldBytes = [
    noGoldVictory.length
]

noGoldVictory.forEach(rec => {
    noGoldBytes.push(...createTextRecord(rec))
})

saveFile('../bundle/NOGOLD.BIN', noGoldBytes);

// ***********************************************
// Gold Screen
// ***********************************************
const goldBytes = [
    goldScreen.length
]

goldScreen.forEach(rec => {
    goldBytes.push(...createTextRecord(rec))
})

saveFile('../bundle/GOLD.BIN', goldBytes);

// ***********************************************
// Energy Screen
// ***********************************************
const energyBytes = [
    energyScreen.length
]

energyScreen.forEach(rec => {
    energyBytes.push(...createTextRecord(rec))
})

saveFile('../bundle/ENERGY.BIN', energyBytes);
