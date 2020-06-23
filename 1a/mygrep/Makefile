all: mygrep

compile_flags = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L -g

mygrep.o: mygrep.c
	gcc $(compile_flags) -c mygrep.c

mygrep: mygrep.o
	gcc -o mygrep mygrep.o

clean:
	rm -f mygrep mygrep.o