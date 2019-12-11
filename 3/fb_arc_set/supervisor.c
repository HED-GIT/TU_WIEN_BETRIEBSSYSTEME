#include <fcntl.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/mman.h> 
#include <sys/types.h> 
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h> 
#include <limits.h>
#include "circularBuffer.h"
#include <signal.h> 

int gen;

/**
*@brief get called on SIGINT and SIGTERM signals
*@detail starts the cleanup terminates the programm and the ./generator
*/
void signalhandler(int sig_num) {
  set_state(0);
  clean_buffer();
  exit(EXIT_SUCCESS);
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
    printError("no arguments allowed");
  }

  setup_buffer();

  int min = INT_MAX;
  fprintf(stdout, "please start ./generator\n");
  while (1) {
    gen=get_state()-1;
    returnValue read = circ_buf_read();
    if (read.amount == 0) {
      fprintf(stdout, "This graph is already acyclic.\n");
      break;
    }
    if (min > read.amount) {
      printEdge(read.returnEdges, read.amount);
      min = read.amount;
    }

  }
  //gen=0;
  set_state(0);
  while(gen!=get_state()*(-1)){circ_buf_read();  sleep(1);}
  clean_buffer();
  exit(EXIT_SUCCESS);

}
