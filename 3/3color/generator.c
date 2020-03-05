#include <stdlib.h>
#include <stdio.h> 
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include "circularBuffer.h"

#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))

//#define DEBUG

static returnValue calculateOneSolution(const int *argc, edge *edges ,const int *maxNode);
static void readInput(const int *argc,char** argv,edge *allEdges);
static void handler(const int *max, const int *argc, edge *allEdges);


static void handler(const int *max, const int *argc, edge * allEdges) {
	returnValue my_solution;
	
	int state;
	while ((state = get_state()) > 0) {
	
		
		my_solution = calculateOneSolution(argc, allEdges, max);

		if (my_solution.amount < MAXRETURN) {
#ifdef DEBUG
			printEdge(my_solution.returnEdges, my_solution.amount);
#endif
			if(circ_buf_write(&my_solution)!=0){
				ERROR_EXIT("error writing to shared memory");
			}
		}
		else {
#ifdef DEBUG
			fprintf(stdout, "Solution too Big\n");
#endif
		}
	}
	if(state != 0){
		ERROR_EXIT("couldn't fetch program state");
	}
}


static void readInput(const int *argc,char *argv[],edge * allEdges) {
	int startNode = 0;
	int endNode = 0;
	for (int i = 1; i < *argc; i++) {
		if (sscanf(argv[i], "%d-%d", &startNode, &endNode) != 2) {
			ERROR_MSG("all nodes have to have the form number-number");
			clean_loaded_buffer();
			exit(EXIT_FAILURE);
		}
		edge edge;
		edge.start = MIN(startNode,endNode);
		edge.end = MAX(startNode,endNode);

		allEdges[i - 1] = edge;
	}
}


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



int main(int argc,char *argv[] ) {
    name = argv[0];

    if (1 == argc) {
       ERROR_MSG("no graph given");
       exit(EXIT_FAILURE);
    }

    load_buffer();

	if(increment_state()!=0){
		ERROR_EXIT("couldn't increment system state");
	}
	srand(time(0) + get_state() * 1000);

	edge * allEdges = malloc(sizeof(edge)*(argc-1));
	readInput(&argc, argv, allEdges);
	int max = 0;
	for (int i = 0; i < argc; i++) {
		max = MAX(allEdges[i].start,max);
		max = MAX(allEdges[i].end, max);
	}
	max++;
	handler(&max,&argc,allEdges);
	
	free(allEdges);
	
	sem_post(free_sem);
	
	clean_loaded_buffer();
	SUCCESS_EXIT();
}
