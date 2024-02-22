const fs = require("fs");

// This is the standard palette for the X16 plus some additions for the game
const palette = JSON.parse(fs.readFileSync("palette.json", { encoding: 'utf-8' }))

// Added a 0, 0 2 byte header required for cbm_k_load
// Its an optional address to load into. We don't use it but its required
const palBytes = []

palette.forEach(p => {
    const b1 = (p.g<<4) + p.b
    const b2 = p.r
    palBytes.push(b1)
    palBytes.push(b2)
})

const output = new Uint8Array(palBytes);

const outputFilename = '../bundle/PALETTE.BIN'
fs.writeFileSync(outputFilename, output, "binary");
