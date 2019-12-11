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
*@brief cleanup
*@detail handels cleanup
*/
void cleanup(void){

  munmap(buf, sizeof( * buf));
  munmap(state, sizeof(int));
  close(shmfd);
  close(shmstate);
  sem_close(free_sem);
  sem_close(used_sem);
  fprintf(stdout,"cleanup\n");
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
      cleanup();
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
  srand(time(0));
  edge graph[argc - 1];

  int shmfd = shm_open(SHMNAME, O_RDWR | O_CREAT, 0600);
  if (shmfd == -1) {
    cleanup();
    printError("shm_open failed");
  }

  buf = mmap(NULL, sizeof( * buf), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
  if (buf == MAP_FAILED) {
    cleanup();
    printError("mmap failed");
  }
  free_sem = sem_open(SEM_1, BUFFERLENGTH);
  used_sem = sem_open(SEM_2, 0);
  if (free_sem == SEM_FAILED || used_sem == SEM_FAILED) {   
    cleanup();
    printError("sem_open failed");
  }
  if (1 == argc) {
    cleanup();
    printError("graph has to have at least one edge");
  }

  int shmstate = shm_open(STATENAME, O_RDWR | O_CREAT, 0600);
  if (shmstate == -1) {
    cleanup();
    printError("error at opening shared memory");
  }
  state = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shmstate, 0);
  if (state == MAP_FAILED) {
    cleanup();
    printError("mmap failed");
  }
  ( * state) ++;
  srand(time(0) + ( * state) * 1000);
  
  readInput(argc,argv,&graph[0]);
  int counter;
  int amountVertices = getMaxInt(graph, argc - 1) + 1;
  while (1) {
    if (( * state) <= 0) {
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
    if (( * state) <= 0) {
      break;
    }
    if (counter == MAXRETURN+1) {
      fprintf(stdout, "more then %d edges\n",MAXRETURN);
    } else {
    if (( * state) <= 0) {
      break;
    }
      circ_buf_write(returns);
      printEdge(returns.returnEdges, returns.amount);
    }
    /*if (counter == 0) {
      fprintf(stdout, "graph is acyclic\n");
      break;
    }*/

  }
  (*state)--;
  sem_post(used_sem);
  sem_post(free_sem);
  cleanup();
  exit(EXIT_SUCCESS);
}

