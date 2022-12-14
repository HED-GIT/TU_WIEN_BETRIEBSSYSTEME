all: mycompress

compile_flags = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L -g

mycompress.o: mycompress.c
	gcc $(compile_flags) -c mycompress.c

mycompress: mycompress.o
	gcc -o mycompress mycompress.o

clean:
	rm -f mycompress mycompress.o