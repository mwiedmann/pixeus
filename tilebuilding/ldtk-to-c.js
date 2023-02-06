const fs = require("fs");

const rawText = fs.readFileSync("pixeus.ldtk");

const d = JSON.parse(rawText);

const createLevelCode = (levelNum, level) => {
  let levelName = `level${levelNum}`;
  let masterCode = `#include "level.h"\n\n`;

  let movementTypesCode = `unsigned char ${levelName}MovementTypes[30][40] = {\n`

  const createMovementTypes = (grid) => {
    let count=0
    for (let y=0; y<30; y++) {
      const row = []
      for (let x=0; x<40; x++,count++) {
        row.push(grid[count])
      }
      movementTypesCode += `    {${row.join(',')}}${y<29 ? ',' : ''}\n`
    }

    movementTypesCode+= '};\n\n'

    masterCode += movementTypesCode;

    // console.log(masterCode)
  };

  const allGridTiles = [];

  // { "px": [48,32], "src": [80,80], "f": 0, "t": 65, "d": [83] },
  const addTiles = (gridTiles, layer) => {
    gridTiles.forEach((g) => {
      const x = parseInt(g.px[0] / 16);
      const y = parseInt(g.px[1] / 16);
      const type = g.t + 1; // Add 1 because the game adds Tile:0 as an empty tile
      const vFlip = g.f === 2 || g.f === 3 ? 1 : 0;
      const hFlip = g.f === 1 || g.f === 3 ? 1 : 0;
      allGridTiles.push({ x, y, length: 1, type, layer, vFlip, hFlip });
    });
  };

  let entrancesCodeAll = '';
  let exitsCodeAll = '';
  let enemiesCodeAll = '';

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
          name: fiGet(e.fieldInstances, 'Name'),
          x: e.__grid[0],
          y: e.__grid[1],
        };
      });
    if (entrances.length === 0) {
      throw new Error("Missing required LevelEntrance in entityInstances");
    }

    let entranceCode = entrances
      .map((e) => {
        return `    { ${e.x}, ${e.y}, "${e.name}" }`;
      })
      .join(",\n");

    entrancesCodeAll = `Entrance ${levelName}Entrances[${entrances.length}] = {\n${entranceCode}\n};\n\n`;
    entrancesCodeAll = `${entrancesCodeAll}EntranceList ${levelName}EntranceList = { ${entrances.length}, ${levelName}Entrances };\n\n`;

    // Exits
    const exits = entityInstances
      .filter((e) => e.__identifier === "LevelExit")
      .map((e) => {
        return {
          entrance: fiGet(e.fieldInstances, "Entrance"),
          level: fiGet(e.fieldInstances, "Level"),
          x: e.__grid[0],
          y: e.__grid[1],
        };
      });
    if (exits.length === 0) {
      throw new Error("Missing required LevelEntrance in entityInstances");
    }

    let exitsCode = exits
      .map((e) => {
        return `    { ${e.x}, ${e.y}, ${e.level}, "${e.entrance}" }`;
      })
      .join(",\n");

    exitsCodeAll = `Exit ${levelName}Exits[${exits.length}] = {\n${exitsCode}\n};\n\n`;
    exitsCodeAll = `${exitsCodeAll}ExitList ${levelName}ExitList = { ${exits.length}, ${levelName}Exits };\n\n`;

    // Enemies
    const enemies = entityInstances
      .filter((e) => e.__identifier === "Enemy")
      .map((e) => {
        return {
          type: fiGet(e.fieldInstances, 'type'),
          moveDir: fiGet(e.fieldInstances, 'moveDir'),
          patrolA: fiGet(e.fieldInstances, 'patrolA'),
          patrolB: fiGet(e.fieldInstances, 'patrolB'),
          patrolDir: fiGet(e.fieldInstances, 'patrolDir'),
          x: e.__grid[0],
          y: e.__grid[1],
        };
      });

    let enemiesCode = enemies
      .map((e) => {
        return `    { ${e.x}, ${e.y}, ${e.moveDir}, ${e.patrolA}, ${e.patrolB}, ${e.patrolDir}, ${e.type}, 0 }`;
      })
      .join(",\n");

    if (enemies.length > 0) {
      enemiesCodeAll = `EnemyLayout ${levelName}Enemies[${enemies.length}] = {\n${enemiesCode}\n};\n\n`;
    }

    enemiesCodeAll = `${enemiesCodeAll}EnemyLayoutList ${levelName}EnemyList = { ${enemies.length}, ${enemies.length > 0 ? `${levelName}Enemies` : '0'} };\n\n`;
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
        addTiles(li.gridTiles, 1);
        break;

      case "Entities":
        addEntities(li.entityInstances);
        break;
    }
  });

  let allTilesCode = `TileLayout ${levelName}Tiles[${
    allGridTiles.length
  }] = {\n${allGridTiles
    .map((g) => {
      return `    { ${g.x}, ${g.y}, ${g.length}, ${g.type}, ${g.layer}, ${g.vFlip}, ${g.hFlip} }`;
    })
    .join(",\n")}\n};\n\n`;

  allTilesCode += `TileLayoutList ${levelName}TilesList = { ${allGridTiles.length}, ${levelName}Tiles };\n\n`;

  masterCode += allTilesCode;

  masterCode += entrancesCodeAll;
  masterCode += exitsCodeAll;
  masterCode += enemiesCodeAll;

  masterCode += `LevelOveralLayout ${levelName} = {\n    &${levelName}TilesList,\n    &${levelName}EnemyList,\n    &${levelName}EntranceList,\n    &${levelName}ExitList,\n    (unsigned char *)${levelName}MovementTypes,\n};\n`;

  const outputFilename = `../game/game${levelName}.c`;
  fs.writeFileSync(outputFilename, masterCode);

  console.log(
    `Created ${outputFilename} with tiles:${allGridTiles.length}`
  );
};

d.levels.forEach((l) => {
  const levelNum = l.identifier.split("_")[1];
  createLevelCode(levelNum, l);
});
