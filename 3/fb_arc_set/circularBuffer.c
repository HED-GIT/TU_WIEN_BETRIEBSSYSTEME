#include "circularBuffer.h"


/**
*@brief handles writing operation to the circularbuffer
*@param value that should be saved to the buffer
*/

void circ_buf_write(returnValue val) {


	if (sem_wait(free_sem) != 0) {
		printError("error at sem_wait (writing)");
	}
	
	sem_wait(write_sem);
	buf->values[buf->writePosition] = val;
	buf->writePosition = (buf->writePosition + 1) % BUFFERLENGTH;
	sem_post(write_sem);
	
	if (sem_post(used_sem) != 0) {
		printError("error at sem_post (writing)");
	}
}


/**
*@brief handles reading operation to the circularbuffer
*@return value that was read from the circularbuffer
*/
returnValue circ_buf_read() {
  if (sem_wait(used_sem) != 0) {
    printError("error at sem_wait (reading)");
  }
  
  returnValue val = buf->values[buf->readPosition];
  buf->readPosition = (buf->readPosition + 1) % BUFFERLENGTH;
  
  if (sem_post(free_sem) != 0) {
    printError("error at sem_post (reading)");
  }
  return val;
}

/**
*@brief prints a default error message to the console and terminates the program
*@param extra error message that should be printed out
*/
void printError(char * text) {
  fprintf(stderr, "Error occuried in %s\n", name);
  fprintf(stderr, "Message: %s\n", text);
  exit(EXIT_FAILURE);
}

/**
*@brief prints out the edges of an edge array
*@param i_edge: edge array to be printed, length: amount of edges which should be printed out
*/
void printEdge(const edge * i_edge, int length) {
  fprintf(stdout, "Solution with %d edges ", length);
  for (int i = 0; i < length; i++) {
    fprintf(stdout, "%d-%d ", i_edge[i].start, i_edge[i].end);
  }
  fprintf(stdout, "\n");
}

void load_buffer(){
  shmfd = shm_open(SHMNAME, O_RDWR | O_CREAT, 0600);
  if (shmfd == -1) {
    clean_loaded_buffer();
    printError("shm_open failed");
  }

  buf = mmap(NULL, sizeof( * buf), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
  if (buf == MAP_FAILED) {
    clean_loaded_buffer();
    printError("mmap failed");
  }
  free_sem = sem_open(SEM_1, BUFFERLENGTH);
  used_sem = sem_open(SEM_2, 0);
  write_sem = sem_open(SEM_W, 1);
  if (free_sem == SEM_FAILED || used_sem == SEM_FAILED || write_sem == SEM_FAILED) {   
    clean_loaded_buffer();
    printError("sem_open failed");
  }
}

void clean_loaded_buffer(){
  munmap(buf, sizeof( * buf));
  close(shmfd);
  sem_close(free_sem);
  sem_close(used_sem);
  sem_close(write_sem);
  fprintf(stdout,"cleanup\n");
}

void setup_buffer(){
  shmfd = shm_open(SHMNAME, O_RDWR | O_CREAT, 0600);
  if (shmfd == -1) {
    clean_buffer();
    printError("error at opening shared memory");
  }

  if (ftruncate(shmfd, sizeof( * buf)) < 0) {
    clean_buffer();
    printError("error  at ftruncate");

  }
  buf = mmap(NULL, sizeof( * buf), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
  if (buf == MAP_FAILED) {
    clean_buffer();
    printError("mmap failed");
  }

  free_sem = sem_open(SEM_1, O_CREAT | O_EXCL, 0600, BUFFERLENGTH);
  used_sem = sem_open(SEM_2, O_CREAT | O_EXCL, 0600, 0);
  write_sem = sem_open(SEM_W, O_CREAT | O_EXCL, 0600, 1);

  if (free_sem == SEM_FAILED || used_sem == SEM_FAILED) {
    clean_buffer();
    printError("sem_open failed");
  }
  set_state(0);
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

void increment_state(){
	sem_wait(write_sem); 
	buf->state++;
	sem_post(write_sem); 
}
void decrement_state(){
	sem_wait(write_sem); 
	buf->state--;
	sem_post(write_sem); 
}

void set_state(int i){
	sem_wait(write_sem); 
	buf->state=i;
	sem_post(write_sem); 
}

int get_state(){
	int i;
	sem_wait(write_sem); 
	i=buf->state;
	sem_post(write_sem); 
	return i;
}
