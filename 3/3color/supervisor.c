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

int terminate = 0;

static void signalhandler(int sig_num);


static void signalhandler(int sig_num) {
	terminate++;
}


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

    while(!terminate){
		
		int error_value;
        if((error_value = circ_buf_read(&newSolution))!=0){
			ERROR_EXIT("error while reading buffer");
		}
		
		
        if(newSolution.amount == 0){
		fprintf(stdout, "The graph is 3-colorable \n");
           	break;
        }
        if(newSolution.amount < bestSolution.amount){
            bestSolution = newSolution;

			printEdge(newSolution.returnEdges,newSolution.amount);
        }
	}
	
	set_state(0);
	sem_post(free_sem);
	
	clean_buffer();
	SUCCESS_EXIT();
}
