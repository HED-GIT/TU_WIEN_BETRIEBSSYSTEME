#include <fcntl.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/mman.h> 
#include <sys/types.h> 
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h> 
#include <limits.h>
#include "circularBuffer.h"
#include <signal.h> 

int gen;


/**
*@brief cleanup function
*@detail handels the cleanup of the programm
*/
void cleanup(void) {
  
  munmap(buf, sizeof( * buf));
  close(shmfd);
  shm_unlink(SHMNAME);
  munmap(state, sizeof(int));
  close(shmstate);
  shm_unlink(STATENAME);
  sem_close(free_sem);
  sem_close(used_sem);
  sem_unlink(SEM_1);
  sem_unlink(SEM_2);
  fprintf(stdout, "close\n");
}

/**
*@brief get called on SIGINT and SIGTERM signals
*@detail starts the cleanup terminates the programm and the ./generator
*/
void signalhandler(int sig_num) {
  ( * state) = 0;
  //while(gen!=(* state)*(-1)){circ_buf_read();  sleep(1);}
  //sleep(1);
  cleanup();
  exit(EXIT_SUCCESS);
}

/**
*@brief main function of program
*@detail handels creating of shared buffer, cleanup...
*/
int main(int argc, char * argv[]) {

  signal(SIGINT, signalhandler);
  signal(SIGTERM, signalhandler);
  name = argv[0];
  if (argc > 1) {
    printError("no arguments allowed");
  }

  shmfd = shm_open(SHMNAME, O_RDWR | O_CREAT, 0600);
  if (shmfd == -1) {
    cleanup();
    printError("error at opening shared memory");
  }

  if (ftruncate(shmfd, sizeof( * buf)) < 0) {
    cleanup();
    printError("error  at ftruncate");

  }
  buf = mmap(NULL, sizeof( * buf), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
  if (buf == MAP_FAILED) {
    cleanup();
    printError("mmap failed");
  }

  free_sem = sem_open(SEM_1, O_CREAT | O_EXCL, 0600, BUFFERLENGTH);
  used_sem = sem_open(SEM_2, O_CREAT | O_EXCL, 0600, 0);

  if (free_sem == SEM_FAILED || used_sem == SEM_FAILED) {
    cleanup();
    printError("sem_open failed");
  }

  shmstate = shm_open(STATENAME, O_RDWR | O_CREAT, 0600);
  if (shmstate == -1) {
    cleanup();
    printError("error at opening shared memory");
  }
  if (ftruncate(shmstate, sizeof(int)) < 0) {
    cleanup();
    printError("error  at ftruncate");

  }
  state = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shmstate, 0);
  if (state == MAP_FAILED) {
    cleanup();
    printError("mmap failed");
  }
  ( * state) = 1;

  int min = INT_MAX;
  fprintf(stdout, "please start ./generator\n");
  while (1) {
    gen=(*state)-1;
    returnValue read = circ_buf_read();
    if (read.amount == 0) {
      fprintf(stdout, "This graph is already acyclic.\n");
      break;
    }
    if (min > read.amount) {
      printEdge(read.returnEdges, read.amount);
      min = read.amount;
    }

  }
  //gen=0;
  ( * state) = 0;
  while(gen!=(* state)*(-1)){circ_buf_read();  sleep(1);}
  cleanup();
  exit(EXIT_SUCCESS);

}
