all: forkSort

compile_flags = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L -g

forkSort.o: forkSort.c
	gcc $(compile_flags) -c forkSort.c

forkSort: forkSort.o
	gcc -o forkSort forkSort.o

clean:
	rm -f forkSort forkSort.o