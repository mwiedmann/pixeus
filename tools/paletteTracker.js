
const desiredColors = [
    // ************************************
    // Shared colors
    // ************************************
    [27, 27, 27], // Non-transparent black (use palette 16-black)
    [40, 173, 255], // Water waves, bubbles
    [0, 247, 58], // Green - desert cactus, forest ground
    [255, 83, 186], // Pink - cactus/forest flowers
    [197, 215, 219], // Grey - moon, stones
    [255, 240, 232], // White - stars, moon
    [255, 121, 24], // Orange - sun, fire ground
    [190, 110, 43], // Brown - dead tree, fire dirt
    [207, 179, 223], // White/Pinkish - winter cloud, fire moon
    [212, 215, 182], // Grey - clouds
    [255, 207, 41], // yellow/orange - Desert, fire flowers

    // ************************************
    // Environment specific
    // ************************************

    // Forest
    [171, 83, 54], // Brown
    
    // Desert
    [255, 124, 0], // orange
    [255, 255, 0], // sun yellow
    
    // Winter
    [91, 227, 227], // Icy ground, icicles
    [110, 70, 39], // Brown
    [255, 255, 0], // Sun yellow

    // Fire
    [255, 66, 66], // lava
]

const convertColor = (c) => {
    return c === 0 ? 0 : parseInt(c / 16);
};

let idx = 17

const convertedColors = desiredColors.map(c => ({
    i: idx++,
    r: convertColor(c[0]),
    g: convertColor(c[1]),
    b: convertColor(c[2]),
}))

console.log(JSON.stringify(convertedColors, undefined, 2));