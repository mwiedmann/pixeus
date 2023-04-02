const fs = require("fs");

const filename = process.argv[2];
const fileparts = filename.split(".");

const sampleRate = parseInt(process.argv[3] ?? 64)

const chopcount = parseInt(process.argv[4] ?? 44)

console.log("filename", filename);

if (!filename) {
  throw new Error("Missing filename");
}

const data = fs.readFileSync(filename);
const size = data.length - chopcount;

const sizeLo = size & 255
const sizeMid = (size & 65280) >> 8
const sizeHi = (size & 16711680) >> 16

console.log(size, sizeLo, sizeMid, sizeHi)

const choppedBytes = [
    0,
    0,
    0,
    sizeLo,
    sizeMid,
    sizeHi,
    0b00101111, // Full volume, stero, 16bit
    sampleRate // Sample rate - Still figuring this one out
]

for (let i=chopcount; i<data.length; i++) {
    choppedBytes.push(data[i])
}


// Added a 0, 0 2 byte header required for cbm_k_load
// Its an optional address to load into. We don't use it but its required
const output = new Uint8Array(choppedBytes);

const outputFilename = `../sounds/${fileparts[0]}.zcm`.toLowerCase();
fs.writeFileSync(outputFilename, output, "binary");

console.log('Created', outputFilename, ' Size', data.length-chopcount)