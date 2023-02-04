const fs = require("fs");

const rawText = fs.readFileSync("pixeus.ldtk");

const d = JSON.parse(rawText);

const createLevelCode = (levelNum, level) => {
  let levelName = `level${levelNum}`;
  let masterCode = `#include "level.h"

`;
  let enemyCount = 0;

  const createEnemies = (intGridCsv) => {
    let i = 0;
    const enemies = {};
    const lengthHardCoded = 5;

    for (let y = 0; y < 30; y++) {
      for (let x = 0; x < 40; x++, i++) {
        let enemyId = intGridCsv[i];

        if (enemyId !== 0) {
          const enemyEntries = enemies[enemyId] || [];
          enemyEntries.push(
            `{${x}, ${y}, ${lengthHardCoded}, ${enemyId - 1}, 0}`
          );
          enemies[enemyId] = enemyEntries;
        }
      }
    }

    // console.log(JSON.stringify(enemies, undefined, 2))

    let enemyLayout = "";

    const enemyKeys = Object.keys(enemies);
    // Can't have an empty array in C, so we just add a dummy record
    // the "length" field will be 0 so its ok
    if (enemyKeys.length === 0) {
      enemyLayout += `    {0, 0, 0, 0, 0}`;
      enemyCount = 0;
    } else {
      enemyKeys.forEach((k) => {
        enemies[k].forEach((e) => {
          enemyLayout += `${enemyCount > 0 ? ",\n" : ""}    ${e}`;
          enemyCount++;
        });
      });
    }

    enemyLayout = `EnemyLayout ${levelName}Enemies[${enemyCount}] = {\n${enemyLayout}\n};\n\nEnemyLayoutList ${levelName}EnemyList = { ${enemyCount}, ${levelName}Enemies };\n\n`;

    masterCode += enemyLayout;

    // console.log(masterCode)s
  };

  // { "px": [0,80], "src": [0,0], "f": 0, "t": 0, "d": [200] },
  let runCount = 0;

  const createSolidGround = (gridTiles) => {
    let solidCode = "";
    let xStart = undefined;
    let xPrev = undefined;
    let yStart = undefined;
    let length = 0;

    const printLengthRun = (skipComma) => {
      runCount++;
      solidCode += `    { ${xStart}, ${yStart}, ${length}, 0}${
        !skipComma ? "," : ""
      }\n`;

      // Reset tracking after printing code
      xStart = undefined;
      xPrev = undefined;
      yStart = undefined;
      length = 0;
    };

    const startLengthRun = (x, y) => {
      xStart = x;
      yStart = y;
      xPrev = x;
      length = 0;
    };

    gridTiles.forEach((g) => {
      const x = parseInt(g.px[0] / 16);
      const y = parseInt(g.px[1] / 16);

      // See if we are in a length run
      if (xStart !== undefined) {
        // If we moved Y or there is an X gap, then the run is over
        if (y !== yStart || x !== xPrev + 1) {
          printLengthRun();
          startLengthRun(x, y);
        }
      } else {
        startLengthRun(x, y);
      }
      xPrev = x;
      length++;
    });

    printLengthRun(true);

    solidCode = `SolidLayout ${levelName}Solids[${runCount}] = {\n${solidCode}};\n\n`;
    solidCode = `${solidCode}SolidLayoutList ${levelName}SolidList = { ${runCount}, ${levelName}Solids };\n\n`;

    masterCode += solidCode;

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

  let entrancesCodeAll;
  let exitsCodeAll;

  const addEntities = (entityInstances) => {
    // Entrances
    const entrances = entityInstances
      .filter((e) => e.__identifier === "LevelEntrance")
      .map((e) => {
        return {
          name: e.fieldInstances.find((fi) => fi.__identifier === "Name")
            .__value,
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
          entrance: e.fieldInstances.find(
            (fi) => fi.__identifier === "Entrance"
          ).__value,
          level: e.fieldInstances.find((fi) => fi.__identifier === "Level")
            .__value,
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
  };

  level.layerInstances.forEach((li) => {
    switch (li.__identifier) {
      case "Enemies":
        createEnemies(li.intGridCsv);
        break;
      case "Solid_Ground":
        createSolidGround(li.gridTiles);
        break;
      case "Tiles_Background":
        addTiles(li.gridTiles, 0);
        break;

      // Background and ground tiles are both layer 0
      // For ground tiles this allows the player to jump in front of them
      case "Tiles_Background":
      case "Tiles_Ground":
        addTiles(li.gridTiles, 0);
        break;
      case "Tiles_Foreground":
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

  masterCode += `LevelOveralLayout ${levelName} = {\n    &${levelName}SolidList,\n    &${levelName}TilesList,\n    &${levelName}EnemyList,\n    &${levelName}EntranceList,\n    &${levelName}ExitList\n};\n`;

  const outputFilename = `../game/game${levelName}.c`;
  fs.writeFileSync(outputFilename, masterCode);

  console.log(
    `Created ${outputFilename} with enemies:${enemyCount}, solidGround:${runCount}, tiles:${allGridTiles.length}`
  );
};

d.levels.forEach((l) => {
  const levelNum = l.identifier.split("_")[1];
  createLevelCode(levelNum, l);
});
