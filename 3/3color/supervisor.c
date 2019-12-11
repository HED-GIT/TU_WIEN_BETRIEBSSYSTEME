#include <stdlib.h> 
#include <stdio.h> 
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include "circularBuffer.h"


#define USAGE() {fprintf(stderr,"./supervisor no arguments allowed\n"); exit(EXIT_FAILURE);}

static void signalhandler(int sig_num);



/*
* @brief signal handler for terminating the programm
*/
static void signalhandler(int sig_num) {
	set_state(0);
    clean_buffer();
	exit(EXIT_SUCCESS);
}

/*
* @brief entry point,
* reads from buffer and prints values
*/
int main( int argc, char *argv[] ) {
    name = argv[0];
    signal(SIGINT, signalhandler);
    signal(SIGTERM, signalhandler);

    if (1 != argc) {
		USAGE();
    }

	returnValue bestSolution = {.amount=MAXGRAPHSIZE};

    setup_buffer();

	returnValue newSolution = {.amount=MAXGRAPHSIZE};

    fprintf(stdout, "waiting for solutions from ./generator\n");

	int gen = 0;
    while(1){
		gen=get_state()-1;
        newSolution=circ_buf_read();
        if(newSolution.amount == 0){
		fprintf(stdout, "The graph is 3-colorable \n");
           	break;
        }
        if(newSolution.amount < bestSolution.amount){
            bestSolution = newSolution;

			printEdge(newSolution.returnEdges,newSolution.amount);
        }
	}
  clean_buffer();
  set_state(0);
  while(gen!=get_state()*(-1)){circ_buf_read();  sleep(1);}
  clean_buffer();
  exit(EXIT_SUCCESS);
}
