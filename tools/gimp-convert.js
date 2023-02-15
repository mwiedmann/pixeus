/**
 * This reads in a GIMP "Raw image data - data,raw" file and converts it to 
 * a .c file as an array of palette indexes that can be used as image data by sprites and tiles.
 * 
 * Usage: `node gimp-convert.js imagefile.data WIDTH`
 * WIDTH: A number that is the width of the image.
 * 
 * It attempts to match colors to the default palette by chosing the closest.
 * Improvements may be needed in the algorith but it works pretty well.
 * 
 * To create the image data in GIMP, "Export" the image and Select "Raw image data" as the file type
 * Make sure to remove the alpha channel or the data is not in the correct formap
 * Layer -> Transparency -> Remove Alpha Channel
 * 
 * To use the image in your program:
 * Add `extern unsigned char yournameImage[];`
 * In the makefile, add the .c file to the compile list
 */

const fs = require("fs");

const pixelData = [];

const filename = process.argv[2];
const width = parseInt(process.argv[3]);
const fileparts = filename.split(".");

console.log("filename", filename, "wdith", width);

if (!filename) {
  throw new Error("Missing filename");
}
if (!width) {
    throw new Error("Missing width")
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
    return { i: 16, r: 0, g: 0, b: 0 }
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

// Spit out an array
let output = `unsigned char ${fileparts[0]}Image[${pixelData.length}] = {`;

// We attempt to format it nicely by width/height
// When you look at the generated array you can kind of see the image
convertedPixels.forEach((cp, idx) => {
  if (idx % width === 0) {
    output += "\n  ";
  }
  output +=
    cp.i.toString().padStart(3, " ") + (idx < convertedPixels.length - 1
      ? ","
      : "\n");
});

output += "};";

if (conversionError.length > 0) {
  console.error("Conversion Error(s)", conversionError);
}

const outputFilename = `../images/${fileparts[0]}Image.c`
fs.writeFileSync(outputFilename, output);

console.log(`Generated file ${outputFilename} with ${pixelData.length} pixels`);
console.log(`Add the following line of code to use in your program. Also update the makefile with this file.`)
console.log(`extern unsigned char ${fileparts[0]}Image[];`)
