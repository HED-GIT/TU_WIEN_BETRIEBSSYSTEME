build: mydiff

compile_flags = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L -g

mydiff.o: mydiff.c
	gcc $(compile_flags) -c mydiff.c

mydiff: mydiff.o
	gcc -o mydiff mydiff.o

clean:
	rm -f mydiff mydiff.o