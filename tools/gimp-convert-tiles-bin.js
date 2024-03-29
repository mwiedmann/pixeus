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
const frameWidth = parseInt(process.argv[5]);
const frameHeight = parseInt(process.argv[6]);
const outputFilename = `../bundle/${(process.argv[7] || fileparts[0]).toUpperCase()}.BIN`; 

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
  pixelData.push({
    r: data[i],
    g: data[i + 1],
    b: data[i + 2],
  });
}

// This is the standard palette for the X16 plus some additions for the game
const palette = JSON.parse(fs.readFileSync("palette.json", { encoding: 'utf-8' }))

// Scale the values down to a 0-15 range to match the PX16 palette
const convertColor = (c) => {
  return c === 0 ? 0 : parseInt(c / 16);
};

// Find the closest color we can
const findPalleteIdx = (cRec) => {
  // We have a special case for 1,1,1
  // 0,0,0 Index 0 is transparent in X16
  // We use a special 1,1,1 color to select index 16 which is also black
  if (cRec.r == 1 && cRec.g == 1 && cRec.b == 1) {
    return { i: 16, r: 0, g: 0, b: 0 };
  }

  let exact = undefined;
  let close = undefined;
  let closeAmt = 9999;
  let closeBiggestSingle = 999;
  const maxSingleDiff = 6;

  for (let i = 0; i < palette.length; i++) {
    const pRec = palette[i];
    if (cRec.r === pRec.r && cRec.g === pRec.g && cRec.b === pRec.b) {
      // exact...we are done
      exact = pRec;
      break;
    } else {
      const sr = Math.abs(cRec.r - pRec.r);
      const sg = Math.abs(cRec.g - pRec.g);
      const sb = Math.abs(cRec.b - pRec.b);
      // Make sure no single value strays too far or this isn't a good candidate
      if (sr <= maxSingleDiff && sg <= maxSingleDiff && sb <= maxSingleDiff) {
        const diff = sr + sg + sb;
        const thisBiggestSingle = Math.max(sr, sg, sb);

        // If this is closer in total diff OR its the same BUT it has a smaller "biggest single diff"
        if (
          diff < closeAmt ||
          (diff === closeAmt && thisBiggestSingle < closeBiggestSingle)
        ) {
          closeAmt = diff;
          closeBiggestSingle = thisBiggestSingle;
          close = pRec;
        }
      }
    }
  }

  if (exact) {
    return exact;
  }
  return close;
};

let conversionError = [];

// Convert to the scaled down colors and find a match in the palette
const convertedPixels = pixelData.map((d, idx) => {
  const sized = {
    r: convertColor(d.r),
    g: convertColor(d.g),
    b: convertColor(d.b),
  };
  const result = findPalleteIdx(sized);

  // We are pretty generous but its possible we don't find a match
  if (!result) {
    conversionError.push({ data: d, sized });
  }

  return result ? result : { i: -9999 };
});

if (conversionError.length > 0) {
  console.error("Conversion Error(s)", conversionError);
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
      for (x = 0; x < frameWidth; x++) {
        pixelIdx = start + x;
        flattenedTiles.push(convertedPixels[pixelIdx].i);
      }
    }
  }
}

const output = new Uint8Array(flattenedTiles);

fs.writeFileSync(outputFilename, output, "binary");

console.log(`Generated file ${outputFilename} with ${pixelData.length} pixels`);
