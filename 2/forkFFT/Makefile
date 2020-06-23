build: forkFFT

compile_flags = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L -g
link_libraries = -lm

forkFFT.o: forkFFT.c
	gcc $(compile_flags) -c forkFFT.c

forkFFT: forkFFT.o
	gcc -o forkFFT forkFFT.o $(link_libraries)

clean:
	rm -f forkFFT forkFFT.o