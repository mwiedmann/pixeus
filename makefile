CC=cl65

make:
	$(CC) -I lib -o x16test.prg -t cx16 \
	game/main.c game/startup.c game/level.c game/gamelevels.c game/gametiles.c game/gamesprites.c \
	lib/x16graphics.c lib/sprites.c lib/waitforjiffy.s \
	images/guyrunImage.c images/greentreeTilesImage.c images/snakeImage.c

clean:
	rm lib/*.o images/*.o game/*.o

