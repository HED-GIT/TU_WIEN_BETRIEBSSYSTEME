#include "circularBuffer.h"

int circ_buf_write(returnValue * val) {

	while (sem_wait(free_sem) != 0) {
		if(errno == EINTR)
			continue;
		else
			return -1;
	}
	
	while (sem_wait(write_sem) != 0) {
		if(errno == EINTR)
			continue;
		else
			return -1;
	}
	
	
	buf->values[buf->writePosition] = *val;
	buf->writePosition = (buf->writePosition + 1) % BUFFERLENGTH;
	
	if(sem_post(write_sem) != 0){
		return -1;
	}
	
	if (sem_post(used_sem) != 0) {
		return -1;
	}
	return 0;
}

int circ_buf_read(returnValue * val) {
	
	if (sem_wait(used_sem) != 0) {
		if(errno == EINTR)
			return 1;
		else
			return -1;
	}
  
  *val = buf->values[buf->readPosition];
  buf->readPosition = (buf->readPosition + 1) % BUFFERLENGTH;
  
  if (sem_post(free_sem) != 0) {
    return -1;
  }
  return 0;
}

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
    ERROR_EXIT("shm_open failed");
  }

  buf = mmap(NULL, sizeof( * buf), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
  if (buf == MAP_FAILED) {
    clean_loaded_buffer();
    ERROR_EXIT("mmap failed");
  }
  free_sem = sem_open(SEM_1, BUFFERLENGTH);
  used_sem = sem_open(SEM_2, 0);
  write_sem = sem_open(SEM_W, 1);
  if (free_sem == SEM_FAILED || used_sem == SEM_FAILED || write_sem == SEM_FAILED) {   
    clean_loaded_buffer();
    ERROR_EXIT("sem_open failed");
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

int increment_state(){
	while (sem_wait(write_sem) != 0) {
		if(errno == EINTR)
			continue;
		else
			return -1;
	}
	buf->state++;
	if (sem_post(write_sem) != 0) {
		return -1;
	}
	return 0;
}

int set_state(int i){
	while (sem_wait(write_sem) != 0) {
		if(errno == EINTR)
			continue;
		else
			return -1;
	}
	buf->state=i;
	if (sem_post(write_sem) != 0) {
		return -1;
	}
	return 0;
}

int get_state(){
	int i;
	while (sem_wait(write_sem) != 0) {
		if(errno == EINTR)
			continue;
		else
			return -1;
	}
	i=buf->state;
	if (sem_post(write_sem) != 0) {
		return -1;
	}
	return i;
}
