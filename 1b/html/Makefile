all: server client
 
compile_flags = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L -g

server.o: server.c
	gcc $(compile_flags) -c server.c

server: server.o sharedTools.o
	gcc -o server server.o sharedTools.o

client.o: client.c
	gcc $(compile_flags) -c client.c

client: client.o sharedTools.o
	gcc -o client client.o sharedTools.o

sharedTools.o: sharedTools.c
	gcc $(compile_flags) -c sharedTools.c
	
clean:
	rm -f *.o
	rm -f server
	rm -f client
