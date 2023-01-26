CC=cl65

make:
	$(CC) -o x16test.prg -t cx16 x16graphics.c main.c waitforjiffy.s guyrunImage.c

clean:
	rm *.o
