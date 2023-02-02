const fs = require("fs");

const rawText = fs.readFileSync("x16.ldtk");

const d = JSON.parse(rawText);

// only 1 level to start
const level = d.levels[0];

/*
typedef struct EnemyLayout {
    unsigned char x;
    unsigned char y;
    unsigned char length;
    EnemyType enemyType;
    EnemyMoveType moveType;
} EnemyLayout;

EnemyLayout wasps[1] = {
    { 7, 22, 25, Wasp, 0}
};

EnemyLayoutList testLevelEnemies[5] = {
    { 3, snakes },
    { 2, bees },
    { 2, ghosts },
    { 1, scopions },
    { 1, wasps }
};

*/
let masterCode =
`#include "level.h"

`
let enemyCount = 0

const createEnemies = (intGridCsv) => {
    let i=0
    const enemies = {}
    const lengthHardCoded = 5

    for (let y=0; y<30; y++) {
        for (let x=0; x<40; x++,i++) {
            let enemyId = intGridCsv[i]

            if (enemyId !== 0) {
                const enemyEntries = enemies[enemyId] || []
                enemyEntries.push(`{${x}, ${y}, ${lengthHardCoded}, ${enemyId-1}, 0}`)
                enemies[enemyId] = enemyEntries
            }  
        }
    }

    // console.log(JSON.stringify(enemies, undefined, 2))

    let enemyLayout = ''
    
    Object.keys(enemies).forEach(k => {
        enemies[k].forEach(e => {
            enemyLayout += `${enemyCount > 0 ? ',\n' : ''}    ${e}`
            enemyCount++
        })
    })

    enemyLayout = `EnemyLayout enemies[${enemyCount}] = {\n${enemyLayout}\n};\n\nEnemyLayoutList testLevelEnemies[1] = {\n    { ${enemyCount}, enemies }\n};\n\n`

    masterCode += enemyLayout

    // console.log(masterCode)
};

/*
typedef struct SolidLayout {
    unsigned char x;
    unsigned char y;
    unsigned char length;
    unsigned char type;
} SolidLayout;

SolidLayout testLevelSolid[TEST_LEVEL_SOLID_LENGTH] = {
    { 7, 9, 1, 0}
},

SolidLayoutList testLevelAllSolids[1] = {
    { TEST_LEVEL_SOLID_LENGTH, testLevelSolid }
};

*/

// { "px": [0,80], "src": [0,0], "f": 0, "t": 0, "d": [200] },
let runCount = 0

const createSolidGround = (gridTiles) => {
    let solidCode = ''
    let xStart = undefined
    let xPrev = undefined
    let yStart = undefined
    let length = 0
    

    const printLengthRun = (skipComma) => {
        runCount++
        solidCode += `    { ${xStart}, ${yStart}, ${length}, 0}${!skipComma ? ',' : ''}\n`

        // Reset tracking after printing code
        xStart = undefined
        xPrev = undefined
        yStart = undefined
        length = 0
    }

    const startLengthRun = (x, y) => {
        xStart = x
        yStart = y
        xPrev = x
        length = 0
    }

    gridTiles.forEach(g => {
        const x = parseInt(g.px[0]/16)
        const y = parseInt(g.px[1]/16)

        // See if we are in a length run
        if (xStart !== undefined) {
            // If we moved Y or there is an X gap, then the run is over
            if (y !== yStart || x !== xPrev + 1) {
                printLengthRun();
                startLengthRun(x, y)
            }
        } else {
            startLengthRun(x, y)
        }
        xPrev = x
        length++
    })

    printLengthRun(true);

    solidCode = `SolidLayout testLevelSolid[${runCount}] = {\n${solidCode}};\n\n`
    solidCode = `${solidCode}SolidLayoutList testLevelAllSolids[1] = {\n    { ${runCount}, testLevelSolid }\n};\n\n`

    masterCode += solidCode

    // console.log(masterCode)
}

/*
typedef struct TileLayout {
    unsigned char x;
    unsigned char y;
    unsigned char length;
    unsigned char type;
    unsigned char layer;
    unsigned char vFlip;
    unsigned char hFlip;
} TileLayout;

TileLayout testLevelTiles[TEST_LEVEL_TILES_LENGTH] = {
    { 7, 7, 1, 36, 0}
};

TileLayoutList testLevelAllTiles[3] = {
    { TEST_LEVEL_TILES_LENGTH, testLevelTiles }
};
*/

const allGridTiles = []

// { "px": [48,32], "src": [80,80], "f": 0, "t": 65, "d": [83] },
const addTiles = (gridTiles, layer) => {
    gridTiles.forEach(g => {
        const x = parseInt(g.px[0]/16)
        const y = parseInt(g.px[1]/16)
        const type = g.t + 1 // Add 1 because the game adds Tile:0 as an empty tile
        const vFlip = g.f === 2 || g.f === 3 ? 1 : 0
        const hFlip = g.f === 1 || g.f === 3 ? 1 : 0
        allGridTiles.push({ x, y, length: 1, type, layer, vFlip, hFlip})
    })
}

let playerStartCode

const addEntities = (entityInstances) => {
    const ps = entityInstances.find(e => e.__identifier === "PlayerStart")
    if (!ps) {
        throw new Error("Missing required PlayerStart in entityInstances")
    }

    playerStartCode = `PlayerLayout playerLayout = {${ps.px[0]}, ${ps.px[1]}};\n\n`
}

level.layerInstances.forEach((li) => {
  switch (li.__identifier) {
    case "Enemies":
      createEnemies(li.intGridCsv);
      break;
    case "Solid_Ground":
      createSolidGround(li.gridTiles)
      break
    case "Tiles_Background":
        addTiles(li.gridTiles, 0)
        break;

    // Background and ground tiles are both layer 0
    // For ground tiles this allows the player to jump in front of them
    case "Tiles_Background":
    case "Tiles_Ground":
        addTiles(li.gridTiles, 0)
        break;
    case "Tiles_Foreground":
        addTiles(li.gridTiles, 1)
        break;

    case "Entities":
        addEntities(li.entityInstances)
        break
  }
});

let allTilesCode = `TileLayout testLevelTiles[${allGridTiles.length}] = {\n${allGridTiles
    .map(g => {
        return `    { ${g.x}, ${g.y}, ${g.length}, ${g.type}, ${g.layer}, ${g.vFlip}, ${g.hFlip} }`
    }).join(',\n')}\n};\n\n`

allTilesCode += `TileLayoutList testLevelAllTiles[1] = {\n    { ${allGridTiles.length}, testLevelTiles }\n};\n\n`

masterCode+= allTilesCode

masterCode+= playerStartCode

/*
LevelOveralLayout testLevel = {
    1,
    testLevelAllSolids,
    3,
    testLevelAllTiles,
    5,
    testLevelEnemies
};
*/

masterCode+= `LevelOveralLayout testLevel = { 1, testLevelAllSolids, 1, testLevelAllTiles, 1, testLevelEnemies, &playerLayout };\n`

const outputFilename = `../game/gamelevels.c`
fs.writeFileSync(outputFilename, masterCode);

console.log(`Created ${outputFilename} with enemies:${enemyCount}, solidGround:${runCount}, tiles:${allGridTiles.length}`)