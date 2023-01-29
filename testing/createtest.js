/**
 * Use this to create a .bin file from an array
 * The file can then be loaded with cbm_k_load
 */

const fs = require("fs");

// The first 2 bytes are an optional Address to load into
// We aren't using it BUT the cbm_k_load seems to still skip those bytes, so we add 0,0
const output = new Uint8Array([
  0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
]);

const outputFilename = `../images/sample.bin`.toLowerCase();
fs.writeFileSync(outputFilename, output, "binary");
