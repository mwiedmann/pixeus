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

// This is the standard palette for the X16
const palette = [
  { i: 0, r: 0, g: 0, b: 0 },
  { i: 1, r: 15, g: 15, b: 15 },
  { i: 2, r: 8, g: 0, b: 0 },
  { i: 3, r: 10, g: 15, b: 14 },
  { i: 4, r: 12, g: 4, b: 12 },
  { i: 5, r: 0, g: 12, b: 5 },
  { i: 6, r: 0, g: 0, b: 10 },
  { i: 7, r: 14, g: 14, b: 7 },
  { i: 8, r: 13, g: 8, b: 5 },
  { i: 9, r: 6, g: 4, b: 0 },
  { i: 10, r: 15, g: 7, b: 7 },
  { i: 11, r: 3, g: 3, b: 3 },
  { i: 12, r: 7, g: 7, b: 7 },
  { i: 13, r: 10, g: 15, b: 6 },
  { i: 14, r: 0, g: 8, b: 15 },
  { i: 15, r: 11, g: 11, b: 11 },
  { i: 16, r: 0, g: 0, b: 0 },
  { i: 17, r: 1, g: 1, b: 1 },
  { i: 18, r: 2, g: 2, b: 2 },
  { i: 19, r: 3, g: 3, b: 3 },
  { i: 20, r: 4, g: 4, b: 4 },
  { i: 21, r: 5, g: 5, b: 5 },
  { i: 22, r: 6, g: 6, b: 6 },
  { i: 23, r: 7, g: 7, b: 7 },
  { i: 24, r: 8, g: 8, b: 8 },
  { i: 25, r: 9, g: 9, b: 9 },
  { i: 26, r: 10, g: 10, b: 10 },
  { i: 27, r: 11, g: 11, b: 11 },
  { i: 28, r: 12, g: 12, b: 12 },
  { i: 29, r: 13, g: 13, b: 13 },
  { i: 30, r: 14, g: 14, b: 14 },
  { i: 31, r: 15, g: 15, b: 15 },
  { i: 32, r: 2, g: 1, b: 1 },
  { i: 33, r: 4, g: 3, b: 3 },
  { i: 34, r: 6, g: 4, b: 4 },
  { i: 35, r: 8, g: 6, b: 6 },
  { i: 36, r: 10, g: 8, b: 8 },
  { i: 37, r: 12, g: 9, b: 9 },
  { i: 38, r: 15, g: 11, b: 11 },
  { i: 39, r: 2, g: 1, b: 1 },
  { i: 40, r: 4, g: 2, b: 2 },
  { i: 41, r: 6, g: 3, b: 3 },
  { i: 42, r: 8, g: 4, b: 4 },
  { i: 43, r: 10, g: 5, b: 5 },
  { i: 44, r: 12, g: 6, b: 6 },
  { i: 45, r: 15, g: 7, b: 7 },
  { i: 46, r: 2, g: 0, b: 0 },
  { i: 47, r: 4, g: 1, b: 1 },
  { i: 48, r: 6, g: 1, b: 1 },
  { i: 49, r: 8, g: 2, b: 2 },
  { i: 50, r: 10, g: 2, b: 2 },
  { i: 51, r: 12, g: 3, b: 3 },
  { i: 52, r: 15, g: 3, b: 3 },
  { i: 53, r: 2, g: 0, b: 0 },
  { i: 54, r: 4, g: 0, b: 0 },
  { i: 55, r: 6, g: 0, b: 0 },
  { i: 56, r: 8, g: 0, b: 0 },
  { i: 57, r: 10, g: 0, b: 0 },
  { i: 58, r: 12, g: 0, b: 0 },
  { i: 59, r: 15, g: 0, b: 0 },
  { i: 60, r: 2, g: 2, b: 1 },
  { i: 61, r: 4, g: 4, b: 3 },
  { i: 62, r: 6, g: 6, b: 4 },
  { i: 63, r: 8, g: 8, b: 6 },
  { i: 64, r: 10, g: 10, b: 8 },
  { i: 65, r: 12, g: 12, b: 9 },
  { i: 66, r: 15, g: 14, b: 11 },
  { i: 67, r: 2, g: 1, b: 1 },
  { i: 68, r: 4, g: 3, b: 2 },
  { i: 69, r: 6, g: 5, b: 3 },
  { i: 70, r: 8, g: 7, b: 4 },
  { i: 71, r: 10, g: 9, b: 5 },
  { i: 72, r: 12, g: 11, b: 6 },
  { i: 73, r: 15, g: 13, b: 7 },
  { i: 74, r: 2, g: 1, b: 0 },
  { i: 75, r: 4, g: 3, b: 1 },
  { i: 76, r: 6, g: 5, b: 1 },
  { i: 77, r: 8, g: 6, b: 2 },
  { i: 78, r: 10, g: 8, b: 2 },
  { i: 79, r: 12, g: 10, b: 3 },
  { i: 80, r: 15, g: 12, b: 3 },
  { i: 81, r: 2, g: 1, b: 0 },
  { i: 82, r: 4, g: 3, b: 0 },
  { i: 83, r: 6, g: 4, b: 0 },
  { i: 84, r: 8, g: 6, b: 0 },
  { i: 85, r: 10, g: 8, b: 0 },
  { i: 86, r: 12, g: 9, b: 0 },
  { i: 87, r: 15, g: 11, b: 0 },
  { i: 88, r: 1, g: 2, b: 1 },
  { i: 89, r: 3, g: 4, b: 3 },
  { i: 90, r: 5, g: 6, b: 4 },
  { i: 91, r: 7, g: 8, b: 6 },
  { i: 92, r: 9, g: 10, b: 8 },
  { i: 93, r: 11, g: 12, b: 9 },
  { i: 94, r: 13, g: 15, b: 11 },
  { i: 95, r: 1, g: 2, b: 1 },
  { i: 96, r: 3, g: 4, b: 2 },
  { i: 97, r: 4, g: 6, b: 3 },
  { i: 98, r: 6, g: 8, b: 4 },
  { i: 99, r: 8, g: 10, b: 5 },
  { i: 100, r: 9, g: 12, b: 6 },
  { i: 101, r: 11, g: 15, b: 7 },
  { i: 102, r: 1, g: 2, b: 0 },
  { i: 103, r: 2, g: 4, b: 1 },
  { i: 104, r: 4, g: 6, b: 1 },
  { i: 105, r: 5, g: 8, b: 2 },
  { i: 106, r: 6, g: 10, b: 2 },
  { i: 107, r: 8, g: 12, b: 3 },
  { i: 108, r: 9, g: 15, b: 3 },
  { i: 109, r: 1, g: 2, b: 0 },
  { i: 110, r: 2, g: 4, b: 0 },
  { i: 111, r: 3, g: 6, b: 0 },
  { i: 112, r: 4, g: 8, b: 0 },
  { i: 113, r: 5, g: 10, b: 0 },
  { i: 114, r: 6, g: 12, b: 0 },
  { i: 115, r: 7, g: 15, b: 0 },
  { i: 116, r: 1, g: 2, b: 1 },
  { i: 117, r: 3, g: 4, b: 3 },
  { i: 118, r: 4, g: 6, b: 5 },
  { i: 119, r: 6, g: 8, b: 6 },
  { i: 120, r: 8, g: 10, b: 8 },
  { i: 121, r: 9, g: 12, b: 10 },
  { i: 122, r: 11, g: 15, b: 12 },
  { i: 123, r: 1, g: 2, b: 1 },
  { i: 124, r: 2, g: 4, b: 2 },
  { i: 125, r: 3, g: 6, b: 4 },
  { i: 126, r: 4, g: 8, b: 5 },
  { i: 127, r: 5, g: 10, b: 6 },
  { i: 128, r: 6, g: 12, b: 8 },
  { i: 129, r: 7, g: 15, b: 9 },
  { i: 130, r: 0, g: 2, b: 0 },
  { i: 131, r: 1, g: 4, b: 1 },
  { i: 132, r: 1, g: 6, b: 2 },
  { i: 133, r: 2, g: 8, b: 3 },
  { i: 134, r: 2, g: 10, b: 4 },
  { i: 135, r: 3, g: 12, b: 5 },
  { i: 136, r: 3, g: 15, b: 6 },
  { i: 137, r: 0, g: 2, b: 0 },
  { i: 138, r: 0, g: 4, b: 1 },
  { i: 139, r: 0, g: 6, b: 1 },
  { i: 140, r: 0, g: 8, b: 2 },
  { i: 141, r: 0, g: 10, b: 2 },
  { i: 142, r: 0, g: 12, b: 3 },
  { i: 143, r: 0, g: 15, b: 3 },
  { i: 144, r: 1, g: 2, b: 2 },
  { i: 145, r: 3, g: 4, b: 4 },
  { i: 146, r: 4, g: 6, b: 6 },
  { i: 147, r: 6, g: 8, b: 8 },
  { i: 148, r: 8, g: 10, b: 10 },
  { i: 149, r: 9, g: 12, b: 12 },
  { i: 150, r: 11, g: 15, b: 15 },
  { i: 151, r: 1, g: 2, b: 2 },
  { i: 152, r: 2, g: 4, b: 4 },
  { i: 153, r: 3, g: 6, b: 6 },
  { i: 154, r: 4, g: 8, b: 8 },
  { i: 155, r: 5, g: 10, b: 10 },
  { i: 156, r: 6, g: 12, b: 12 },
  { i: 157, r: 7, g: 15, b: 15 },
  { i: 158, r: 0, g: 2, b: 2 },
  { i: 159, r: 1, g: 4, b: 4 },
  { i: 160, r: 1, g: 6, b: 6 },
  { i: 161, r: 2, g: 8, b: 8 },
  { i: 162, r: 2, g: 10, b: 10 },
  { i: 163, r: 3, g: 12, b: 12 },
  { i: 164, r: 3, g: 15, b: 15 },
  { i: 165, r: 0, g: 2, b: 2 },
  { i: 166, r: 0, g: 4, b: 4 },
  { i: 167, r: 0, g: 6, b: 6 },
  { i: 168, r: 0, g: 8, b: 8 },
  { i: 169, r: 0, g: 10, b: 10 },
  { i: 170, r: 0, g: 12, b: 12 },
  { i: 171, r: 0, g: 15, b: 15 },
  { i: 172, r: 1, g: 1, b: 2 },
  { i: 173, r: 3, g: 3, b: 4 },
  { i: 174, r: 4, g: 5, b: 6 },
  { i: 175, r: 6, g: 6, b: 8 },
  { i: 176, r: 8, g: 8, b: 10 },
  { i: 177, r: 9, g: 10, b: 12 },
  { i: 178, r: 11, g: 12, b: 15 },
  { i: 179, r: 1, g: 1, b: 2 },
  { i: 180, r: 2, g: 2, b: 4 },
  { i: 181, r: 3, g: 4, b: 6 },
  { i: 182, r: 4, g: 5, b: 8 },
  { i: 183, r: 5, g: 6, b: 10 },
  { i: 184, r: 6, g: 8, b: 12 },
  { i: 185, r: 7, g: 9, b: 15 },
  { i: 186, r: 0, g: 0, b: 2 },
  { i: 187, r: 1, g: 1, b: 4 },
  { i: 188, r: 1, g: 2, b: 6 },
  { i: 189, r: 2, g: 3, b: 8 },
  { i: 190, r: 2, g: 4, b: 10 },
  { i: 191, r: 3, g: 5, b: 12 },
  { i: 192, r: 3, g: 6, b: 15 },
  { i: 193, r: 0, g: 0, b: 2 },
  { i: 194, r: 0, g: 1, b: 4 },
  { i: 195, r: 0, g: 1, b: 6 },
  { i: 196, r: 0, g: 2, b: 8 },
  { i: 197, r: 0, g: 2, b: 10 },
  { i: 198, r: 0, g: 3, b: 12 },
  { i: 199, r: 0, g: 3, b: 15 },
  { i: 200, r: 1, g: 1, b: 2 },
  { i: 201, r: 3, g: 3, b: 4 },
  { i: 202, r: 5, g: 4, b: 6 },
  { i: 203, r: 7, g: 6, b: 8 },
  { i: 204, r: 9, g: 8, b: 10 },
  { i: 205, r: 11, g: 9, b: 12 },
  { i: 206, r: 13, g: 11, b: 15 },
  { i: 207, r: 1, g: 1, b: 2 },
  { i: 208, r: 3, g: 2, b: 4 },
  { i: 209, r: 4, g: 3, b: 6 },
  { i: 210, r: 6, g: 4, b: 8 },
  { i: 211, r: 8, g: 5, b: 10 },
  { i: 212, r: 9, g: 6, b: 12 },
  { i: 213, r: 11, g: 7, b: 15 },
  { i: 214, r: 1, g: 0, b: 2 },
  { i: 215, r: 2, g: 1, b: 4 },
  { i: 216, r: 4, g: 1, b: 6 },
  { i: 217, r: 5, g: 2, b: 8 },
  { i: 218, r: 6, g: 2, b: 10 },
  { i: 219, r: 8, g: 3, b: 12 },
  { i: 220, r: 9, g: 3, b: 15 },
  { i: 221, r: 1, g: 0, b: 2 },
  { i: 222, r: 2, g: 0, b: 4 },
  { i: 223, r: 3, g: 0, b: 6 },
  { i: 224, r: 4, g: 0, b: 8 },
  { i: 225, r: 5, g: 0, b: 10 },
  { i: 226, r: 6, g: 0, b: 12 },
  { i: 227, r: 7, g: 0, b: 15 },
  { i: 228, r: 2, g: 1, b: 2 },
  { i: 229, r: 4, g: 3, b: 4 },
  { i: 230, r: 6, g: 4, b: 6 },
  { i: 231, r: 8, g: 6, b: 8 },
  { i: 232, r: 10, g: 8, b: 10 },
  { i: 233, r: 12, g: 9, b: 12 },
  { i: 234, r: 15, g: 11, b: 14 },
  { i: 235, r: 2, g: 1, b: 1 },
  { i: 236, r: 4, g: 2, b: 3 },
  { i: 237, r: 6, g: 3, b: 5 },
  { i: 238, r: 8, g: 4, b: 7 },
  { i: 239, r: 10, g: 5, b: 9 },
  { i: 240, r: 12, g: 6, b: 11 },
  { i: 241, r: 15, g: 7, b: 13 },
  { i: 242, r: 2, g: 0, b: 1 },
  { i: 243, r: 4, g: 1, b: 3 },
  { i: 244, r: 6, g: 1, b: 5 },
  { i: 245, r: 8, g: 2, b: 6 },
  { i: 246, r: 10, g: 2, b: 8 },
  { i: 247, r: 12, g: 3, b: 10 },
  { i: 248, r: 15, g: 3, b: 12 },
  { i: 249, r: 2, g: 0, b: 1 },
  { i: 250, r: 4, g: 0, b: 3 },
  { i: 251, r: 6, g: 0, b: 4 },
  { i: 252, r: 8, g: 0, b: 6 },
  { i: 253, r: 10, g: 0, b: 8 },
  { i: 254, r: 12, g: 0, b: 9 },
  { i: 255, r: 15, g: 0, b: 11 },
];

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
