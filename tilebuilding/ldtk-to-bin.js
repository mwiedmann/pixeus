/*
Read the .ldtk file and turn it into a series of .bin files (one per level)
The bin file is formatted for the C data structs so they can be malloc'd and loaded in
*/
const fs = require("fs");

const rawText = fs.readFileSync("pixeus.ldtk");
const d = JSON.parse(rawText);

const EnemyEnum = {
    Snake: 0, Bee: 1, Ghost: 2, Scorpion: 3, Wasp: 4, Fish1: 5, BigGhost: 6, Eyeball: 7,
    Mushroom: 8, Slime: 9, Spider: 10, Rat: 11, Rockman: 12, Eel: 13, Iceman: 14, Snowball: 15,
    Dragonfly: 16, BigBear: 17, Clouds: 18, Flies: 19, Shark: 20, Bluehawk: 21, Beetle: 22,
    Jellyfish: 23, Wolf: 24
  }

const enemyCount = {
  Forest: '-----',
  Mushroom: 0,
  Spider: 0,
  Bee: 0,
  Slime: 0,
  Dragonfly: 0,
  BigGhost: 0,
  Desert: '-----',
  Scorpion: 0,
  Wasp: 0,
  Snake: 0,
  Rat: 0,
  Beetle: 0,
  Flies: 0,
  Tundra: '-----',
  Snowball: 0,
  Iceman: 0,
  Clouds: 0,
  Bluehawk: 0,
  Wolf: 0,
  BigBear: 0,
  Underworld: '-----',
  Eyeball: 0,
  Rockman: 0,
  Ghost: 0,
  Water: '-----',
  Fish1: 0,
  Eel: 0,
  Jellyfish: 0,
  Shark: 0,
}

const resourceCount = {
  Gold: 0,
  Energy: 0,
  Weapon: 0,
  Boots: 0,
  Scuba: 0,
  ExtraLife: 0
}

const enemyMax = {
  level: 0,
  count: 0
}

const EntityTypeEnum = { Empty: 0, Entrance: 1, Exit: 2, Energy: 3, Gold: 4, Scuba: 5, Weapon: 6, Boots: 7, ExtraLife: 8  }
const tilesStartTileId = 49

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
  };

  const tilesBytes = [];

  // { "px": [48,32], "src": [80,80], "f": 0, "t": 65, "d": [83] },
  const addTiles = (gridTiles, layer) => {
    gridTiles.forEach((g) => {
      tilesBytes.push(parseInt(g.px[0] / 16));
      tilesBytes.push(parseInt(g.px[1] / 16));
      tilesBytes.push(1)
      tilesBytes.push(g.t + tilesStartTileId); // Add 1 because the game adds Tile:0 as an empty tile + the font tileset
      tilesBytes.push(layer)
      tilesBytes.push(g.f === 2 || g.f === 3 ? 1 : 0);
      tilesBytes.push(g.f === 1 || g.f === 3 ? 1 : 0);
    });
  };

  const fiGet = (fi, id) => {
    return fi.find((fi) => fi.__identifier === id)
          .__value
  }

  const entityBytes = []
  const enemiesBytes = []

  const addEntities = (entityInstances) => {
    // Energies
    const energies = entityInstances
      .filter((e) => e.__identifier === "Energy")
      .map((e) => {
        const amount = fiGet(e.fieldInstances, 'amount')
        resourceCount.Energy+= amount

        return {
          entityType: EntityTypeEnum['Energy'],
          x: e.__grid[0],
          y: e.__grid[1],
          amount,
          unused1: 0
        };
      });

      energies.forEach((e) => {
        entityBytes.push(...[e.entityType, e.x, e.y, e.amount, e.unused1])
      })

    // Gold
    const goldpiles = entityInstances
      .filter((e) => e.__identifier === "Gold")
      .map((e) => {
        const amount = fiGet(e.fieldInstances, 'amount')
        resourceCount.Gold+= amount

        return {
          entityType: EntityTypeEnum['Gold'],
          x: e.__grid[0],
          y: e.__grid[1],
          amount,
          unused1: 0
        };
      });

      goldpiles.forEach((e) => {
        entityBytes.push(...[e.entityType, e.x, e.y, e.amount, e.unused1])
      })

    // Items
    const items = entityInstances
      .filter((e) => e.__identifier === "Item")
      .map((e) => {
        const entityTypeName = fiGet(e.fieldInstances, 'type')
        resourceCount[entityTypeName]+= 1
        console.log('ITEM', entityTypeName, levelNum)
        return {
          entityType: EntityTypeEnum[entityTypeName],
          x: e.__grid[0],
          y: e.__grid[1],
          unused0: 0,
          unused1: 0
        };
      });

      items.forEach((e) => {
        entityBytes.push(...[e.entityType, e.x, e.y, e.unused0, e.unused1])
      })

    // Entrances
    const entrances = entityInstances
      .filter((e) => e.__identifier === "LevelEntrance")
      .map((e) => {
        return {
          entityType: EntityTypeEnum['Entrance'],
          x: e.__grid[0],
          y: e.__grid[1],
          id: fiGet(e.fieldInstances, 'Id'),
          unused1: 0
        };
      });

    entrances.forEach((e) => {
      entityBytes.push(...[e.entityType, e.x, e.y, e.id, e.unused1])
    })

    // Exits
    const exits = entityInstances
      .filter((e) => e.__identifier === "LevelExit")
      .map((e) => {
        return {
          entityType: EntityTypeEnum['Exit'],
          x: e.__grid[0],
          y: e.__grid[1],
          level: fiGet(e.fieldInstances, "Level"),
          entrance: fiGet(e.fieldInstances, "EntranceId")
        };
      });

    exits.forEach((e) => {
      entityBytes.push(...[e.entityType, e.x, e.y, e.level, e.entrance])
    })

    // Enemies
    const enemies = entityInstances
      .filter((e) => e.__identifier === "Enemy")
      .map((e) => {
        const eType = fiGet(e.fieldInstances, 'type')
        enemyCount[eType] += 1

        return {
          type: EnemyEnum[eType],
          moveDir: fiGet(e.fieldInstances, 'moveDir'),
          patrolA: fiGet(e.fieldInstances, 'patrolA'),
          patrolB: fiGet(e.fieldInstances, 'patrolB'),
          patrolDir: fiGet(e.fieldInstances, 'patrolDir'),
          x: e.__grid[0],
          y: e.__grid[1],
        };
      });

    if (enemies.length > enemyMax.count) {
      enemyMax.count = enemies.length
      enemyMax.level = levelNum
    }

    enemies.forEach((e) => {
        enemiesBytes.push(...[e.x, e.y, e.moveDir, e.patrolA, e.patrolB, e.patrolDir, e.type, 0])
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
        if (li.__identifier === "Tiles_Ground") {
          tilesetId = TilesImageMap[li.__tilesetRelPath];
        }
        addTiles(li.gridTiles, 1);
        break;

      case "Entities":
        addEntities(li.entityInstances);
        break;
    }
  });

  const tilesLength = tilesBytes.length/7

  const leftLevel = fiGet(level.fieldInstances, "LeftLevel")
  const rightLevel = fiGet(level.fieldInstances, "RightLevel")
  const downLevel = fiGet(level.fieldInstances, "DownLevel")
  const upLevel = fiGet(level.fieldInstances, "UpLevel")

  // Added a 0, 0 2 byte header required for cbm_k_load
  // Its an optional address to load into. We don't use it but its required
  const output = new Uint8Array([
    tilesetId,
    leftLevel ?? 255,
    rightLevel ?? 255,
    downLevel ?? 255,
    upLevel ?? 255,
    shortLo(tilesLength),
    shortHi(tilesLength),
    entityBytes.length/5,
    enemiesBytes.length/8,
    ...movementTypeBytes,
    ...tilesBytes,
    ...entityBytes,
    ...enemiesBytes
  ]);

  const outputFilename = `../bundle/LEVEL${levelNum}.BIN`
  fs.writeFileSync(outputFilename, output, "binary");
};

d.levels.forEach((l) => {
  const levelNum = l.identifier.split("_")[1];
  createLevelCode(levelNum, l);
});

console.log(enemyCount)
console.log(resourceCount)
console.log(enemyMax)

if (resourceCount.Gold !== 9999 || resourceCount.Energy !== 99 || resourceCount.Weapon !== 1 || 
  resourceCount.Boots !== 1 || resourceCount.Scuba !== 1 || resourceCount.ExtraLife !== 6) {
    console.error("*****ERROR***** resourceCount error. Missing required count for 1 or more items.")
}
