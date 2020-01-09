#include <stdlib.h> 
#include <stdio.h> 
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include "circularBuffer.h"


#define USAGE() {fprintf(stderr,"./supervisor no arguments allowed\n"); exit(EXIT_FAILURE);}

int terminate = 0;

static void signalhandler(int sig_num);

void clean_buffer(){
  munmap(buf, sizeof( * buf));
  close(shmfd);
  shm_unlink(SHMNAME);
  sem_close(free_sem);
  sem_close(used_sem);
  sem_close(write_sem);
  sem_unlink(SEM_1);
  sem_unlink(SEM_2);
  sem_unlink(SEM_W);
  fprintf(stdout, "close\n");
}

void setup_buffer(){
  shmfd = shm_open(SHMNAME, O_RDWR | O_CREAT, 0600);
  if (shmfd == -1) {
    clean_buffer();
    ERROR_EXIT("error at opening shared memory");
  }

  if (ftruncate(shmfd, sizeof( * buf)) < 0) {
    clean_buffer();
    ERROR_EXIT("error  at ftruncate");

  }
  buf = mmap(NULL, sizeof( * buf), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
  if (buf == MAP_FAILED) {
    clean_buffer();
    ERROR_EXIT("mmap failed");
  }

  free_sem = sem_open(SEM_1, O_CREAT | O_EXCL, 0600, BUFFERLENGTH);
  used_sem = sem_open(SEM_2, O_CREAT | O_EXCL, 0600, 0);
  write_sem = sem_open(SEM_W, O_CREAT | O_EXCL, 0600, 1);

  if (free_sem == SEM_FAILED || used_sem == SEM_FAILED || write_sem == SEM_FAILED) {
    clean_buffer();
    ERROR_EXIT("sem_open failed");
  }
}

/**
*@brief handles reading operation to the circularbuffer
*@return value that was read from the circularbuffer
*/
returnValue circ_buf_read() {
  if (sem_wait(used_sem) != 0) {
	clean_buffer();
    ERROR_EXIT("error at sem_wait (reading)");
  }

  returnValue val = buf->values[buf->readPosition];
  buf->readPosition = (buf->readPosition + 1) % BUFFERLENGTH;

  if (sem_post(free_sem) != 0) {
	clean_buffer();
    ERROR_EXIT("error at sem_post (reading)");
  }
  return val;
}


/*
* @brief signal handler for terminating the programm
*/
static void signalhandler(int sig_num) {
	buf->state = 0;
    terminate++;
}

/*
* @brief entry point,
* reads from buffer and prints values
*/
int main( int argc, char *argv[] ) {
    name = argv[0];
    signal(SIGINT, signalhandler);
    signal(SIGTERM, signalhandler);

    if (1 != argc) {
		USAGE();
    }

	returnValue bestSolution = {.amount=MAXGRAPHSIZE};
	returnValue newSolution = {.amount=MAXGRAPHSIZE};
	
    setup_buffer();
	buf->state = 1;
	


    fprintf(stdout, "waiting for solutions from ./generator\n");

    while(!terminate){
        newSolution=circ_buf_read();
        if(newSolution.amount == 0){
		fprintf(stdout, "The graph is 3-colorable \n");
           	break;
        }
        if(newSolution.amount < bestSolution.amount){
            bestSolution = newSolution;

			printEdge(newSolution.returnEdges,newSolution.amount);
        }
	}
	
	
  buf->state = 0;
  clean_buffer();
  exit(EXIT_SUCCESS);
}
