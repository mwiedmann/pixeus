# Commander X16 Graphics Test
This is a program for the [Commander X16](https://cx16forum.com/) system written in C. The purpose is to learn how the sprite hardware and other graphics functions work on the X16. The main goal is to ultimately create a full game.

## What is the Commander X16
In a nutshell, the X16 is a new 8-bit system based on the 65c02 processor (popular in many 80s/90s systems like the C64 and NES). It is a spiritual successor to the C64/C128 and even boots into a very similar BASIC prompt. It is NOT compatible with those systems though as the X16 includes a more advanced graphics/sound chip. The goal is to have a better supported 8-bit system for folks to hack on. There is an emulator for developing on, and actual hardware will be available in the near future.

Their FAQ states: 

`The Commander X16 is David Murray aka The 8-Bit Guy’s dream computer, designed to evoke the same fondness and nostalgia many of us had for 8-Bit computers, whilst retaining closeness to the hardware from a programming perspective, unlike the Raspberry Pi and others. But more than that, it is intended not only as an educational tool but to solve some of the issues of finding an 8-Bit system to tinker with today; namely ever-increasing costs, auction site price gouging/sniping, lack of replacement parts, and unreliability of 30-year old hardware.
The X16 will be made entirely with parts that are still readily available today, ensuring perpetual availability without reliability issues, but in keeping with David's vision, it will house a real CPU rather than using emulation or an FPGA recreation of a processor. Running Commodore BASIC V2 (with some additions), the X16 will be inexpensive enough to allow a critical mass of users to create an expansive software ecosystem, but simple enough that a single person can understand all of the chips and components that allow that software to run.`

## Requirements
- The [cc65](https://cc65.github.io/) cross development package for 65(C)02 systems.
- The Commander X16 emulator. It can be found in the download section on their [main site](https://cx16forum.com/)

## Building
Running
```
make
``` 
will build the `x16test.prg` file which can be run in the emulator with
```
x16emu -prg x16test.prg -run
```
`x16emu` is the emulator so use the appropriate path to that executable.