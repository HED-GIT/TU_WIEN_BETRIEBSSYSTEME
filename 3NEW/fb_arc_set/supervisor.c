#include <limits.h>
#include <signal.h> 

#include "circularBuffer.h"

int terminates = 0;

/**
*@brief get called on SIGINT and SIGTERM signals
*@detail starts the cleanup terminates the programm and the ./generator
*/
void signalhandler(int sig_num) {
  terminates++;
}

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

  if (free_sem == SEM_FAILED || used_sem == SEM_FAILED) {
    clean_buffer();
    ERROR_EXIT("sem_open failed");
  }
  buf->state = 1;
}

returnValue circ_buf_read() {
  if (sem_wait(used_sem) != 0) {
    ERROR_EXIT("error at sem_wait (reading)");
  }
  
  returnValue val = buf->values[buf->readPosition];
  buf->readPosition = (buf->readPosition + 1) % BUFFERLENGTH;
  
  if (sem_post(free_sem) != 0) {
    ERROR_EXIT("error at sem_post (reading)");
  }
  return val;
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
    ERROR_EXIT("no arguments allowed");
  }

  setup_buffer();
  
  int min = INT_MAX;
  fprintf(stdout, "please start ./generator\n");
  while (!terminates) {
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
  buf->state=0;
  clean_buffer();
  exit(EXIT_SUCCESS);

}
