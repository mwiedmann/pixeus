CC=cl65

make:
	$(CC) -I lib -o x16test.prg -t cx16 main.c lib/x16graphics.c lib/sprites.c lib/waitforjiffy.s images/guyrunImage.c

clean:
	rm *.o
	rm lib/*.o 
	rm images/*.o
