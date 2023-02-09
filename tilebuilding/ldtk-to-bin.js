/*
Read the .ldtk file and turn it into a series of .bin files (one per level)
The bin file is formatted for the C data structs so they can be malloc'd and loaded in
*/
const fs = require("fs");

const rawText = fs.readFileSync("pixeus.ldtk");
const d = JSON.parse(rawText);

const EnemyEnum = { Snake: 0, Bee: 1, Ghost: 2, Scorpion: 3, Wasp: 4, Fish1: 5, BigGhost: 6 }
const TilesImageMap = {
  "foresttiles.png": 1,
  "deserttiles.png": 2,
  "wintertiles.png": 3,
  "firetiles.png": 4
}

const shortHi = (s) => s>>8
const shortLo = (s) => s&255

const createLevelCode = (levelNum, level) => {
  let tilesetId;
  const movementTypeBytes = []

  const createMovementTypes = (grid) => {
    let count=0
    for (let y=0; y<30; y++) {
      const row = []
      for (let x=0; x<40; x++,count++) {
        movementTypeBytes.push(grid[count])
      }
    }
    // console.log(masterCode)
  };

  const tilesBytes = [];

  // { "px": [48,32], "src": [80,80], "f": 0, "t": 65, "d": [83] },
  const addTiles = (gridTiles, layer) => {
    gridTiles.forEach((g) => {
      tilesBytes.push(parseInt(g.px[0] / 16));
      tilesBytes.push(parseInt(g.px[1] / 16));
      tilesBytes.push(1)
      tilesBytes.push(g.t + 1); // Add 1 because the game adds Tile:0 as an empty tile
      tilesBytes.push(layer)
      tilesBytes.push(g.f === 2 || g.f === 3 ? 1 : 0);
      tilesBytes.push(g.f === 1 || g.f === 3 ? 1 : 0);

      // allGridTiles.push({ x, y, length: 1, type, layer, vFlip, hFlip });
    });
  };

  const entrancesBytes = []
  const exitsBytes = []
  const enemiesBytes = []

  const addEntities = (entityInstances) => {
    const fiGet = (fi, id) => {
      return fi.find((fi) => fi.__identifier === id)
            .__value
    }

    // Entrances
    const entrances = entityInstances
      .filter((e) => e.__identifier === "LevelEntrance")
      .map((e) => {
        return {
          id: fiGet(e.fieldInstances, 'Id'),
          x: e.__grid[0],
          y: e.__grid[1],
        };
      });
    if (entrances.length === 0) {
      throw new Error("Missing required LevelEntrance in entityInstances");
    }

    entrances.forEach((e) => {
        entrancesBytes.push(e.x)
        entrancesBytes.push(e.y)
        entrancesBytes.push(e.id)
        //return `    { ${e.x}, ${e.y}, ${e.id} }`;
      })

    // Exits
    const exits = entityInstances
      .filter((e) => e.__identifier === "LevelExit")
      .map((e) => {
        return {
          entrance: fiGet(e.fieldInstances, "EntranceId"),
          level: fiGet(e.fieldInstances, "Level"),
          x: e.__grid[0],
          y: e.__grid[1],
        };
      });
    if (exits.length === 0) {
      throw new Error("Missing required LevelEntrance in entityInstances");
    }

    exits.forEach((e) => {
        exitsBytes.push(e.x)
        exitsBytes.push(e.y)
        exitsBytes.push(e.level)
        exitsBytes.push(e.entrance)
        //return `    { ${e.x}, ${e.y}, ${e.level}, ${e.entrance} }`;
      })

    // Enemies
    const enemies = entityInstances
      .filter((e) => e.__identifier === "Enemy")
      .map((e) => {
        return {
          type: EnemyEnum[fiGet(e.fieldInstances, 'type')],
          moveDir: fiGet(e.fieldInstances, 'moveDir'),
          patrolA: fiGet(e.fieldInstances, 'patrolA'),
          patrolB: fiGet(e.fieldInstances, 'patrolB'),
          patrolDir: fiGet(e.fieldInstances, 'patrolDir'),
          x: e.__grid[0],
          y: e.__grid[1],
        };
      });

    enemies.forEach((e) => {
        enemiesBytes.push(e.x)
        enemiesBytes.push(e.y)
        enemiesBytes.push(e.moveDir)
        enemiesBytes.push(e.patrolA)
        enemiesBytes.push(e.patrolB)
        enemiesBytes.push(e.patrolDir)
        enemiesBytes.push(e.type)
        enemiesBytes.push(0)
        //return `    { ${e.x}, ${e.y}, ${e.moveDir}, ${e.patrolA}, ${e.patrolB}, ${e.patrolDir}, ${e.type}, 0 }`;
      })

  };

  level.layerInstances.forEach((li) => {
    switch (li.__identifier) {
      case "MovementTypes":
        createMovementTypes(li.intGridCsv);
        break;

      // Background and ground tiles are both layer 0
      // For ground tiles this allows the player to jump in front of them
      case "Tiles_Background":
        addTiles(li.gridTiles, 0);
        break;
      case "Tiles_Foreground":
      case "Tiles_Ground":
        // We force all layers to use the same tileset as defined in the Tiles_Ground layer
        tilesetId = TilesImageMap[li.__tilesetRelPath];
        addTiles(li.gridTiles, 1);
        break;

      case "Entities":
        addEntities(li.entityInstances);
        break;
    }
  });

  // Added a 0, 0 2 byte header required for cbm_k_load
  // Its an optional address to load into. We don't use it but its required
  /*
  const movementTypeBytes = []
  const tilesBytes = [];
  const entrancesBytes = []
  const exitsBytes = []
  const enemiesBytes = []
  */

  const tilesLength = tilesBytes.length/7

  const output = new Uint8Array([
    0, 0,
    tilesetId,
    shortLo(tilesLength),
    shortHi(tilesLength),
    entrancesBytes.length/3,
    exitsBytes.length/4,
    enemiesBytes.length/8,
    ...movementTypeBytes,
    ...tilesBytes,
    ...entrancesBytes,
    ...exitsBytes,
    ...enemiesBytes
  ]);

  const outputFilename = `../levels/level${levelNum}.bin`.toLowerCase();
  fs.writeFileSync(outputFilename, output, "binary");
};

d.levels.forEach((l) => {
  const levelNum = l.identifier.split("_")[1];
  createLevelCode(levelNum, l);
});
