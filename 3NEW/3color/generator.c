#include <time.h>
#include <stdlib.h>
#include <stdio.h> 
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>
#include <signal.h>
#include "circularBuffer.h"

#define MAX(a,b) (((a)>(b))?(a):(b))

#define DEBUG

static returnValue calculateOneSolution(const int *argc, edge *edges ,const int *maxNode);
static void readInput(const int *argc,char** argv,edge *allEdges);
static void handler(const int *max, const int *argc, edge *allEdges);

void clean_buffer(){
  munmap(buf, sizeof( * buf));
  close(shmfd);
  sem_close(free_sem);
  sem_close(used_sem);
  sem_close(write_sem);
  fprintf(stdout,"cleanup\n");
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
*@brief handles writing operation to the circularbuffer
*@param value that should be saved to the buffer
*/

void circ_buf_write(returnValue val) {


	if (sem_wait(free_sem) != 0) {
		clean_buffer();
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
		clean_buffer();
		ERROR_EXIT("error at sem_post (writing)");
	}
}

/*
* @brief once called it calculates new solutions till stopped by signal
* @param max maximal value of node
* @param argc input count
* @param allEdges the graph used for the calculation
*/
static void handler(const int *max, const int *argc, edge * allEdges) {
	returnValue my_solution;
	while (1) {
	
		
		my_solution = calculateOneSolution(argc, allEdges, max);

		if (my_solution.amount < MAXRETURN) {
#ifdef DEBUG
			printEdge(my_solution.returnEdges, my_solution.amount);
#endif
			circ_buf_write(my_solution);
		}
#ifdef DEBUG
		else {

			fprintf(stdout, "Solution too Big: Solution with %d edges found\n", my_solution.amount);
		}
#endif
	}
}

/*
* @brief reads the graph from program arguments
* @param argc: argument count
* @param argv: arguments
* @param allEdges: pointer were the graph will be saved to
*/
static void readInput(const int *argc,char *argv[],edge * allEdges) {
	int startNode = 0;
	int endNode = 0;
	for (int i = 1; i < *argc; i++) {
		if (sscanf(argv[i], "%d-%d", &startNode, &endNode) != 2) {
			ERROR_MSG("all nodes have to have the form number-number");
			clean_buffer();
			exit(EXIT_FAILURE);
		}
		edge edge;
		edge.start = startNode;
		edge.end = endNode;

		allEdges[i - 1] = edge;
	}
}

/*
* @brief calculates a solution to 3color
* @param argc: argument count
* @param edges: graph to use
* @param maxNode: amount of different nodes
*/
static returnValue calculateOneSolution(const int *argc, edge *edges, const int *maxNode) {
	returnValue newSolution;

    int node_color [*maxNode];
    for (int i = 0; i < *maxNode; i++) {
        int num = (rand() %(3)) + 1;
        node_color[i] = num;
    }

	edge defaultEdge = { .start = -1,.end = -1 };
    for (int i = 0; i < MAXRETURN; i++) {
        newSolution.returnEdges[i] = defaultEdge;
    }

    int NumberColorConflicts = 0; 
    for (int i = 0;  i<(*argc-1);  i++) {
        if(node_color[edges[i].start] == node_color[edges[i].end]){
			if (NumberColorConflicts < MAXRETURN) {
				newSolution.returnEdges[NumberColorConflicts] = edges[i];
			}
            NumberColorConflicts++;
        }
    }
    newSolution.amount = NumberColorConflicts;
    return  newSolution;
}

void * terminator(void * args){
	pthread_t tid = *(pthread_t *) args;
	while(buf->state!=0){}
	pthread_kill(tid,SIGKILL);
	return (void *)NULL;
}

/*
* @brief entrypoint, handles creating of shared ressources
*/
int main(int argc,char **argv ) {
    name = argv[0];

    if (1 == argc) {
       ERROR_MSG("no graph given");
        exit(EXIT_FAILURE);
    }

    load_buffer();

	srand(time(0));

	edge allEdges[MAXGRAPHSIZE];
	readInput(&argc, argv, allEdges);
	int max = 0;
	for (int i = 0; i < argc; i++) {
		max = MAX(allEdges[i].start,max);
		max = MAX(allEdges[i].end, max);
	}
	max++;	
	
	pthread_t tid;
	pthread_t killtid = pthread_self();
	
	pthread_create(&tid,NULL,terminator,(void *)&killtid);
	handler(&max,&argc,allEdges);
	
	clean_buffer();
	exit(EXIT_SUCCESS);
}
