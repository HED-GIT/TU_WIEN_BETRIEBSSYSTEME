#include <fcntl.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/mman.h> 
#include <sys/types.h> 
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h> 
#include <time.h> 
#include "circularBuffer.h"

/**
*@brief generates a random permutation of an array
*@param value x: pointer to the array, n: amount of items that should be made to a random permutation
*/

void permutations(int * x, int n) {
  int j;
  int tmp;
  for (int i = 0; i < n; i++) {
    j = rand() % (n - 1);
    tmp = x[j];
    x[j] = x[i];
    x[i] = tmp;
  }

}


/**
*@brief gets the highest vertice of an array
*@param value x: pointer to the array, n: amount of items
*@return max value of the array
*/
int getMaxInt(struct edges * x, int n) {
  int returnInt = 0;
  for (int i = 0; i < n; i++) {
    if (x[i].start > returnInt)
      returnInt = x[i].start;
    if (x[i].end > returnInt)
      returnInt = x[i].end;
  }
  return returnInt;
}

/**
*@brief saves all read edges to an array
*@detail crashes on wrong format
*@param argv: pointer to the array, argc: amount of items, graph pointer to the place to save to
*/
void readInput(const int argc, char * argv[], edge * graph){
  for (int i = 1; i < argc; i++) {
    edge newEdge;
    if (sscanf(argv[i], "%d-%d", & newEdge.start, & newEdge.end) != 2 || newEdge.start < 0 || newEdge.end < 0) {
      clean_loaded_buffer();
      printError("edge format has to be 'node'-'node'\nall nodes have to be a positiv integer value ");
    }
    graph[i - 1] = newEdge;
  }
}


/**
*@brief main function of program
*@detail handels creating of shared buffer, cleanup...
*/
int main(int argc, char * argv[]) {
  name = argv[0];
  edge graph[argc - 1];

  load_buffer();
  
  
  if (1 == argc) {
    clean_loaded_buffer();
    printError("graph has to have at least one edge");
  }

  increment_state();
  srand(time(0) + get_state() * 1000);
  
  readInput(argc,argv,&graph[0]);
  int counter;
  int amountVertices = getMaxInt(graph, argc - 1) + 1;
  while (1) {
    if (get_state() <= 0) {
      break;
    }
    returnValue returns;
 
    int permutation[amountVertices];

    for (int i = 0; i < amountVertices; i++) {
      permutation[i] = i;
    }

    permutations(permutation, amountVertices + 1);

    counter = 0;
    for (int i = 0; i < argc - 1; i++) {
      for (int j = 0; j < amountVertices + 1; j++) {
        if (counter == MAXRETURN+1) {
          break;
        }
        if (graph[i].end == permutation[j]) {
          returns.returnEdges[counter] = graph[i];
          counter++;
        }
        if (graph[i].start == permutation[j]) {
          break;
        }
      }
    }
    returns.amount = counter;
    if (get_state() <= 0) {
      break;
    }
    if (counter == MAXRETURN+1) {
      fprintf(stdout, "more then %d edges\n",MAXRETURN);
    } else {
    if (get_state() <= 0) {
      break;
    }
      circ_buf_write(returns);
      printEdge(returns.returnEdges, returns.amount);
    }

  }
  decrement_state();
  sem_post(used_sem);
  sem_post(free_sem);
  clean_loaded_buffer();
  exit(EXIT_SUCCESS);
}

