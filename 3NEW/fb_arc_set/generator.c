#include "circularBuffer.h"

#include <pthread.h>
#include <signal.h>

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

void clean_buffer(){
  munmap(buf, sizeof( * buf));
  close(shmfd);
  sem_close(free_sem);
  sem_close(used_sem);
  sem_close(write_sem);
  fprintf(stdout,"cleanup\n");
}

void circ_buf_write(returnValue val) {


	if (sem_wait(free_sem) != 0) {
		ERROR_EXIT("error at sem_wait (writing)");
	}
	
	if (sem_wait(write_sem) != 0) {
		clean_buffer();
		ERROR_EXIT("error at sem_wait (sem_write)");
	}
	buf->values[buf->writePosition] = val;
	buf->writePosition = (buf->writePosition + 1) % BUFFERLENGTH;
	if (sem_post(write_sem) != 0) {
		clean_buffer();
		ERROR_EXIT("error at sem_post (sem_write)");
	}
	
	if (sem_post(used_sem) != 0) {
		ERROR_EXIT("error at sem_post (writing)");
	}
}

void handler(int argc, char ** argv, edge * graph){
  int counter;
  int amountVertices = getMaxInt(graph, argc - 1) + 1;
  while (1) {
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
    if (counter == MAXRETURN+1) {
      fprintf(stdout, "more then %d edges\n",MAXRETURN);
    } else {
      circ_buf_write(returns);
      printEdge(returns.returnEdges, returns.amount);
    }

  }
}

void * terminator(void * args){
	pthread_t tid = *(pthread_t *) args;
	while(buf->state!=0){}
	pthread_kill(tid,SIGKILL);
	return (void *)NULL;
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
      clean_buffer();
      ERROR_EXIT("edge format has to be 'node'-'node'\nall nodes have to be a positiv integer value ");
    }
    graph[i - 1] = newEdge;
  }
}

void load_buffer(){
  shmfd = shm_open(SHMNAME, O_RDWR | O_CREAT, 0600);
  if (shmfd == -1) {
    clean_buffer();
    ERROR_EXIT("shm_open failed");
  }

  buf = mmap(NULL, sizeof( * buf), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
  if (buf == MAP_FAILED) {
    clean_buffer();
    ERROR_EXIT("mmap failed");
  }
  free_sem = sem_open(SEM_1, BUFFERLENGTH);
  used_sem = sem_open(SEM_2, 0);
  write_sem = sem_open(SEM_W, 1);
  if (free_sem == SEM_FAILED || used_sem == SEM_FAILED || write_sem == SEM_FAILED) {   
    clean_buffer();
    ERROR_EXIT("sem_open failed");
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
    clean_buffer();
    ERROR_EXIT("graph has to have at least one edge");
  }

  srand(time(0));
  
  readInput(argc,argv,&graph[0]);
  
  pthread_t tid;
  pthread_t killtid = pthread_self();
  pthread_create(&tid,NULL,terminator,(void *)&killtid);
  
  handler(argc,argv,graph);
  
  clean_buffer();
  exit(EXIT_SUCCESS);
}

