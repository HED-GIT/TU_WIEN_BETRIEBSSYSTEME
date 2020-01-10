#include <fcntl.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/mman.h> 
#include <sys/types.h> 
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h> 
#include <time.h> 

#define SHMNAME "/11775789SHM"
#define MAXGRAPHSIZE 100

#define SEM_1 "/11775789sem_1"
#define SEM_2 "/11775789sem_2"
#define SEM_W "/11775789sem_W"
#define BUFFERLENGTH 5

#define MAXRETURN 8

char * name;

sem_t * free_sem;
sem_t * used_sem;
sem_t * write_sem;

#define ERROR_EXIT(...) { fprintf(stderr, "%s ERROR: " __VA_ARGS__"\n",name); exit(EXIT_FAILURE); }
#define SUCCESS_EXIT() {exit(EXIT_SUCCESS);}
#define ERROR_MSG(...) {fprintf(stderr, "%s ERROR: " __VA_ARGS__"\n",name); }


int shmfd;

/**
*@brief saves start and end vertice of an edge
*/
typedef struct edges {
  int start;
  int end;

}
edge;

/**
*@brief saves the amount of edges and the edges that should be removed (max 8)
*/
typedef struct returnValue {
  edge returnEdges[MAXRETURN];
  int amount;
}
returnValue;

typedef struct buffer{
	int state;
	returnValue values[BUFFERLENGTH];
	int writePosition;
	int readPosition;
} buffer;

buffer * buf;

/**
*@brief prints out the edges of an edge array
*@param i_edge: edge array to be printed, length: amount of edges which should be printed out
*/
void printEdge(const edge * i_edge, int length);
