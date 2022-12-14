all: binary-digits

compile_flags = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L -g

binary-digits.o: binary-digits.c
	gcc $(compile_flags) -c binary-digits.c

binary-digits: binary-digits.o
	gcc -o binary-digits binary-digits.o

clean:
	rm -f binary-digits binary-digits.o