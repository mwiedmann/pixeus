CC=cl65

make:
	$(CC) -I lib -I zsound -o pixeus.prg -t cx16 \
	game/main.c game/startup.c game/level.c game/levelmgr.c game/enemymgr.c game/entitymgr.c game/soundmgr.c \
	game/fontmgr.c game/levelutils.c game/gametiles.c game/gamesprites.c game/welcome.c \
	lib/x16graphics.c lib/sprites.c lib/imageload.c lib/waitforjiffy.s \
	zsound/zsound.lib

test:
	$(CC) -I lib -o test.prg -t cx16 game/test.c lib/x16graphics.c

testrun:
	../emu/x16emu -prg test.prg -run

clean:
	rm lib/*.o images/*.o game/*.o

run:
	../emu/x16emu -prg pixeus.prg -run

level:
	cd tilebuilding && node ldtk-to-bin.js

pal:
	cd tools && node palette-create-bin.js
	
img:
	cd tools && \
	node gimp-convert-bin.js title.data && \
	node gimp-convert-tiles-bin.js tileset1.data 18 7 16 16 && \
	node gimp-convert-tiles-bin.js tileset2.data 20 7 16 16 && \
	node gimp-convert-tiles-bin.js tileset3.data 20 7 16 16 && \
	node gimp-convert-tiles-bin.js tileset4.data 20 7 16 16 && \
	node gimp-convert-tiles-bin.js fontsimple.data 42 1 16 16 && \
	node gimp-convert-tiles-bin.js extralife.data 1 1 16 16 && \
	node gimp-convert-tiles-bin.js boots.data 1 1 16 16 && \
	node gimp-convert-tiles-bin.js weapon.data 1 1 16 16 && \
	node gimp-convert-tiles-bin.js scuba.data 1 1 16 16 && \
	node gimp-convert-tiles-bin.js gold.data 4 1 16 16 && \
	node gimp-convert-tiles-bin.js energy.data 4 1 16 16 && \
	node gimp-convert-tiles-bin.js guyscuba.data 4 1 16 16 && \
	node gimp-convert-tiles-bin.js guyrun.data 6 1 16 16 && \
	node gimp-convert-tiles-bin.js ship.data 4 1 64 64 && \
	node gimp-convert-bin.js laser.data && \
	node gimp-convert-tiles-bin.js expsmall.data 4 1 16 16 && \
	node gimp-convert-tiles-bin.js bigghost.data 4 1 32 32 && \
	node gimp-convert-tiles-bin.js fish1.data 4 1 16 16 && \
	node gimp-convert-tiles-bin.js wasp.data 4 1 16 16 && \
	node gimp-convert-tiles-bin.js scorpion.data 4 1 16 16 && \
	node gimp-convert-tiles-bin.js ghost.data 4 1 16 16 && \
	node gimp-convert-tiles-bin.js bee.data 4 1 16 16 && \
	node gimp-convert-tiles-bin.js snake.data 4 1 16 16
