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
#define STATENAME "/11775789state"
#define MAXGRAPHSIZE 100

#define SEM_1 "/11775789sem_1"
#define SEM_2 "/11775789sem_2"
#define BUFFERLENGTH 5

#define MAXRETURN 8

char * name;

sem_t * free_sem;
sem_t * used_sem;

int shmstate;
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

int * state;
returnValue * buf;
int write_pos;
int read_pos;


/**
*@brief handles writing operation to the circularbuffer
*@param value that should be saved to the buffer
*/
void circ_buf_write(returnValue val);

/**
*@brief handles reading operation to the circularbuffer
*@return value that was read from the circularbuffer
*/
returnValue circ_buf_read();

/**
*@brief prints a default error message to the console and terminates the program
*@param extra error message that should be printed out
*/
void printError(char * text);

/**
*@brief prints out the edges of an edge array
*@param i_edge: edge array to be printed, length: amount of edges which should be printed out
*/
void printEdge(const edge * i_edge, int length);
