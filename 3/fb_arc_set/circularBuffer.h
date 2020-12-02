#include <fcntl.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/mman.h> 
#include <sys/types.h> 
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h> 
#include <time.h> 
#include <errno.h> 


char * name;

#define SHMNAME "/your_student_id_SHM2"
#define STATENAME "/your_student_id_state"
#define MAXGRAPHSIZE 100

#define SEM_1 "/your_student_id_sem_1"
#define SEM_2 "/your_student_id_sem_2"
#define SEM_W "/your_student_id_sem_W"
#define BUFFERLENGTH 5

#define MAXRETURN 8

#define ERROR_EXIT(...) do{ fprintf(stderr, "%s ERROR: " __VA_ARGS__"\n",name); exit(EXIT_FAILURE); }while(0)
#define SUCCESS_EXIT() do{exit(EXIT_SUCCESS);}while(0)
#define ERROR_MSG(...) do{fprintf(stderr, "%s ERROR: " __VA_ARGS__"\n",name); }while(0)

sem_t * free_sem;
sem_t * used_sem;
sem_t * write_sem;

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


int circ_buf_write(returnValue * val);

int circ_buf_read(returnValue * value);

void setup_buffer();

void clean_buffer();

void load_buffer();

void clean_loaded_buffer();

int increment_state();

int set_state(int i);

int get_state();

/**
*@brief prints out the edges of an edge array
*@param i_edge: edge array to be printed, length: amount of edges which should be printed out
*/
void printEdge(const edge * i_edge, int length);