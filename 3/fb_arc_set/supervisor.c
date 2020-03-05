#include <limits.h>
#include <signal.h>

#include "circularBuffer.h"

int terminate = 0;

/**
*@brief get called on SIGINT and SIGTERM signals
*@detail starts the cleanup terminates the programm and the ./generator
*/
void signalhandler(int sig_num) {
  terminate ++;
}

/**
*@brief main function of program
*@detail handels creating of shared buffer, cleanup...
*/
int main(int argc, char * argv[]) {

  signal(SIGINT, signalhandler);
  signal(SIGTERM, signalhandler);
  name = argv[0];
  if (argc > 1) {
    ERROR_EXIT("no arguments allowed");
  }

  setup_buffer();

  returnValue newSolution = {.amount=INT_MAX};
  returnValue bestSolution = {.amount=INT_MAX};
  
  fprintf(stdout, "please start ./generator\n");
  while (!terminate) {

	int error_value;
	if((error_value = circ_buf_read(&newSolution))!=0){
		if(error_value==1){
				break;
			}
		ERROR_EXIT("error while reading buffer");
	}
	
    if (newSolution.amount == 0) {
      fprintf(stdout, "This graph is already acyclic.\n");
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
  exit(EXIT_SUCCESS);

}