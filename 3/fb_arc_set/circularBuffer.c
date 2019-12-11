#include "circularBuffer.h"
#include <fcntl.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/mman.h> 
#include <sys/types.h> 
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h> 
#include <time.h> 


/**
*@brief handles writing operation to the circularbuffer
*@param value that should be saved to the buffer
*/

void circ_buf_write(returnValue val) {

  if (sem_wait(free_sem) != 0) {
    printError("error at sem_wait (writing)");
  }
  buf[write_pos] = val;
  if (sem_post(used_sem) != 0) {
    printError("error at sem_post (writing)");
  }
  write_pos = (write_pos + 1) % BUFFERLENGTH;
}


/**
*@brief handles reading operation to the circularbuffer
*@return value that was read from the circularbuffer
*/
returnValue circ_buf_read() {
  if (sem_wait(used_sem) != 0) {
    printError("error at sem_wait (reading)");
  }
  returnValue val = buf[read_pos];
  if (sem_post(free_sem) != 0) {
    printError("error at sem_post (reading)");
  }
  read_pos = (read_pos + 1) % BUFFERLENGTH;
  return val;
}

/**
*@brief prints a default error message to the console and terminates the program
*@param extra error message that should be printed out
*/
void printError(char * text) {
  fprintf(stderr, "Error occuried in %s\n", name);
  fprintf(stderr, "Message: %s\n", text);
  exit(EXIT_FAILURE);
}

/**
*@brief prints out the edges of an edge array
*@param i_edge: edge array to be printed, length: amount of edges which should be printed out
*/
void printEdge(const edge * i_edge, int length) {
  fprintf(stdout, "Solution with %d edges ", length);
  for (int i = 0; i < length; i++) {
    fprintf(stdout, "%d-%d ", i_edge[i].start, i_edge[i].end);
  }
  fprintf(stdout, "\n");
}
