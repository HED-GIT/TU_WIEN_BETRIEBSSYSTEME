all: intmul

compile_flags = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L -g

intmul.o: intmul.c
	gcc $(compile_flags) -c intmul.c

intmul: intmul.o
	gcc -o intmul intmul.o

clean:
	rm -f intmul intmul.o