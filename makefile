CC=cl65

make:
	$(CC) -O -I lib -I zsound -o pixeus.prg -t cx16 \
	game/main.c \
	game/startup.c game/level.c game/levelmgr.c game/enemymgr.c game/entitymgr.c game/soundmgr.c game/textmgr.c game/loopmgr.c \
	game/fontmgr.c game/levelutils.c game/gametiles.c game/gamesprites.c \
	lib/x16graphics.c lib/sprites.c lib/imageload.c lib/waitforjiffy.s \
	zsound/zsound.lib

test:
	$(CC) -O -I lib -I zsound -o test.prg -t cx16 \
	game/test.c \
	game/startup.c game/level.c game/levelmgr.c game/enemymgr.c game/entitymgr.c game/soundmgr.c game/textmgr.c game/loopmgr.c \
	game/fontmgr.c game/levelutils.c game/gametiles.c game/gamesprites.c \
	lib/x16graphics.c lib/sprites.c lib/imageload.c lib/waitforjiffy.s \
	zsound/zsound.lib

testrun:
	../emu-r41/x16emu -prg test.prg -run

clean:
	rm lib/*.o images/*.o game/*.o

run:
	../emu-r41/x16emu -prg pixeus.prg -run

runlatest:
	../emu-latest/x16emu -prg pixeus.prg -run -randram

level:
	cd tilebuilding && node ldtk-to-bin.js

pal:
	cd tools && node palette-create-bin.js

txt:
	cd tools && node text-screen-gen.js

img:
	cd tools && \
	node gimp-convert-bin.js title.data && \
	node gimp-convert-tiles-bin.js tileset1.data 18 7 16 16 && \
	node gimp-convert-tiles-bin.js tileset2.data 20 7 16 16 && \
	node gimp-convert-tiles-bin.js tileset3.data 20 7 16 16 && \
	node gimp-convert-tiles-bin.js tileset4.data 20 7 16 16 && \
	node gimp-convert-tiles-bin.js fontsimple.data 42 1 16 16 && \
	node gimp-convert-tiles-bin.js guyrun.data 6 1 16 16 sp0 && \
	node gimp-convert-tiles-bin.js guyscuba.data 4 1 16 16 sp1 && \
	node gimp-convert-tiles-bin.js expsmall.data 4 1 16 16 sp2 && \
	node gimp-convert-tiles-bin.js laser.data 1 1 16 16 sp3 && \
	node gimp-convert-tiles-bin.js ship.data 4 1 64 64 sp4 && \
	node gimp-convert-tiles-bin.js energy.data 4 1 16 16 sp5 && \
	node gimp-convert-tiles-bin.js gold.data 4 1 16 16 sp6 && \
	node gimp-convert-tiles-bin.js scuba.data 1 1 16 16 sp7 && \
	node gimp-convert-tiles-bin.js weapon.data 1 1 16 16 sp8 && \
	node gimp-convert-tiles-bin.js boots.data 1 1 16 16 sp9 && \
	node gimp-convert-tiles-bin.js extralife.data 1 1 16 16 sp10 && \
	node gimp-convert-tiles-bin.js snake.data 4 1 16 16 sp11 && \
	node gimp-convert-tiles-bin.js bee.data 4 1 16 16 sp12 && \
	node gimp-convert-tiles-bin.js ghost.data 4 1 16 16 sp13 && \
	node gimp-convert-tiles-bin.js scorpion.data 4 1 16 16 sp14 && \
	node gimp-convert-tiles-bin.js wasp.data 4 1 16 16 sp15 && \
	node gimp-convert-tiles-bin.js fish1.data 4 1 16 16 sp16 && \
	node gimp-convert-tiles-bin.js bigghost.data 4 1 32 32 sp17 && \
	node gimp-convert-tiles-bin.js eyeball.data 4 1 16 16 sp18 && \
	node gimp-convert-tiles-bin.js mushroom.data 4 1 16 16 sp19 && \
	node gimp-convert-tiles-bin.js slime.data 4 1 16 16 sp20 && \
	node gimp-convert-tiles-bin.js spider.data 4 1 16 16 sp21 && \
	node gimp-convert-tiles-bin.js rat.data 4 1 16 16 sp22 && \
	node gimp-convert-tiles-bin.js rockman.data 4 1 16 16 sp23 && \
	node gimp-convert-tiles-bin.js eel.data 4 1 16 16 sp24 && \
	node gimp-convert-tiles-bin.js iceman.data 4 1 16 16 sp25 && \
	node gimp-convert-tiles-bin.js snowball.data 4 1 16 16 sp26 && \
	node gimp-convert-tiles-bin.js dragfly.data 4 1 16 16 sp27 && \
	node gimp-convert-tiles-bin.js bigbear.data 4 1 32 32 sp28 && \
	node gimp-convert-tiles-bin.js clouds.data 4 1 16 16 sp29 && \
	node gimp-convert-tiles-bin.js flies.data 4 1 32 32 sp30 && \
	node gimp-convert-tiles-bin.js shark.data 4 1 32 16 sp31 && \
	node gimp-convert-tiles-bin.js bluehawk.data 4 1 16 16 sp32 && \
	node gimp-convert-tiles-bin.js flame.data 1 1 16 16 && \
	node gimp-convert-tiles-bin.js snowflake.data 1 1 16 16

	
	
	
	
	
	
	
	
	
	
	
	
