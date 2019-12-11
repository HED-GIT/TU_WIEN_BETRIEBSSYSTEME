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
#include "circularBuffer.h"

#define MAX(a,b) (((a)>(b))?(a):(b))

#define DEBUG

static returnValue calculateOneSolution(const int *argc, edge *edges ,const int *maxNode);
static void readInput(const int *argc,char** argv,edge *allEdges);
static void handler(const int *max, const int *argc, edge *allEdges);

/*
* @brief once called it calculates new solutions till stopped by signal
* @param max maximal value of node
* @param argc input count
* @param allEdges the graph used for the calculation
*/
static void handler(const int *max, const int *argc, edge * allEdges) {
	returnValue my_solution;
	while (get_state() > 0) {
	
		
		my_solution = calculateOneSolution(argc, allEdges, max);

		if (my_solution.amount < MAXRETURN) {
#ifdef DEBUG
			printEdge(my_solution.returnEdges, my_solution.amount);
#endif
			circ_buf_write(my_solution);
		}
		else {
#ifdef DEBUG
			fprintf(stdout, "Solution too Big: Solution with %d edges found\n", my_solution.amount);
#endif
		}
	}
	clean_loaded_buffer();
	exit(EXIT_SUCCESS);
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
			clean_loaded_buffer();
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


/*
* @brief entrypoint, handles creating of shared ressources
*/
int main(int argc,char *argv[] ) {
    name = argv[0];

    if (1 == argc) {
       ERROR_MSG("no graph given");
        exit(EXIT_FAILURE);
    }

    load_buffer();

	increment_state();
	srand(time(0) + get_state() * 1000);

	edge allEdges[MAXGRAPHSIZE];
	readInput(&argc, argv, allEdges);
	int max = 0;
	for (int i = 0; i < argc; i++) {
		max = MAX(allEdges[i].start,max);
		max = MAX(allEdges[i].end, max);
	}
	max++;
	handler(&max,&argc,allEdges);
	decrement_state();
	sem_post(used_sem);
	sem_post(free_sem);
	clean_loaded_buffer();
	exit(EXIT_SUCCESS);
}
