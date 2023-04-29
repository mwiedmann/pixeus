/**
 * This reads in a GIMP "Raw image data - data,raw" file and converts it to
 * a .bin file that can be loaded with cbm_k_load.
 * This will use the tile width/height to flatted the tiles so they can be used directly in code.
 *
 * Usage: `node gimp-convert-tiles-bin.js imagefile.data xTiles yTiles frameWidth frameHeight`
 *
 * It attempts to match colors to the default palette by chosing the closest.
 * Improvements may be needed in the algorith but it works pretty well.
 *
 * To create the image data in GIMP, "Export" the image and Select "Raw image data" as the file type
 * Make sure to remove the alpha channel or the data is not in the correct formap
 * Layer -> Transparency -> Remove Alpha Channel
 */

const fs = require("fs");

const pixelData = [];

const filename = process.argv[2];
const fileparts = filename.split(".");
const xTiles = parseInt(process.argv[3]);
const yTiles = parseInt(process.argv[4]);
const frameWidth = 8;
const frameHeight = 8;

console.log(
  `filename: ${filename} xTiles:${xTiles} yTiles:${yTiles} frameWidth:${frameWidth} frameHeight:${frameHeight}`
);

if (!filename || !xTiles || !yTiles || !frameWidth || !frameHeight) {
  throw new Error("Missing required argument");
}

const data = fs.readFileSync(filename);

// The raw data is just a long array of bytes
// Group them by 3 (r,g,b) into an array
for (let i = 0; i < data.length; i += 3) {
  pixelData.push(data[i] === 0 ? 0 : 1);
}

const flattenedTiles = [];

let ty, tx, y, x, start, pixelIdx;

for (ty = 0; ty < yTiles; ty++) {
  for (tx = 0; tx < xTiles; tx++) {
    for (y = 0; y < frameHeight; y++) {
      start =
        ty * xTiles * frameWidth * frameHeight +
        tx * frameWidth +
        y * xTiles * frameWidth;

      let byte = 0
      
      for (x = 0; x < frameWidth; x++) {
        let val = pixelData[start+x]
        byte+= val << (7 - x)
      }

      flattenedTiles.push(byte)
    }
  }
}

// console.log(JSON.stringify(pixelData))

// Added a 0, 0 2 byte header required for cbm_k_load
// Its an optional address to load into. We don't use it but its required
const output = new Uint8Array([0, 0, ...flattenedTiles]);

const outputFilename = `../bundle/${fileparts[0].toUpperCase()}.BIN`;
fs.writeFileSync(outputFilename, output, "binary");

console.log(`Generated file ${outputFilename} with ${pixelData.length} pixels`);
