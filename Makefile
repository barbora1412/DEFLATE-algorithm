all: program

program: compressor.c main.c
	gcc -Wall -g -std=c99 compressor.c main.c -o program -lm

clean:
	rm program

