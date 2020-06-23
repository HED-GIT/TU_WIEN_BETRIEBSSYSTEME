all: generator supervisor
 
compile_flags = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SCID_SOURCE -D_POSIX_C_SOURCE=200809L -g
link_libaries = -lpthread -lrt

generator.o: generator.c
	gcc $(compile_flags) -c generator.c

generator: generator.o circularBuffer.o
	gcc -o generator generator.o circularBuffer.o $(link_libaries)

supervisor.o: supervisor.c
	gcc $(compile_flags) -c supervisor.c

supervisor: supervisor.o circularBuffer.o
	gcc -o supervisor supervisor.o circularBuffer.o $(link_libaries)

circularBuffer.o: circularBuffer.c
	gcc $(compile_flags) -c circularBuffer.c
	
clean:
	rm -f *.o
	rm -f generator
	rm -f supervisor
