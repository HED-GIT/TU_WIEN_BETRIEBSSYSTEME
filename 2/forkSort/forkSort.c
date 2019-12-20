#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/wait.h>
#include <fcntl.h> 
#include <string.h> 
#include <ctype.h>

#define MAXLENGTH 100000
#define MAX_AMOUNT_STRINGS 100

#define USAGE() {fprintf(stderr,"USAGE:\t%s\n",fileName); exit(EXIT_FAILURE);}
#define ERROR_EXIT(...) { fprintf(stderr, "%s ERROR: " __VA_ARGS__"\n",fileName); exit(EXIT_FAILURE); }
#define SUCCESS_EXIT() {exit(EXIT_SUCCESS);}


#define PIPE_1_WRITE 0
#define PIPE_2_WRITE 1
#define PIPE_1_READ 2
#define PIPE_2_READ 3

#define READ 0
#define WRITE 1

char * fileName;



static void dupNeededPipes(int pipeAmount, int pipes[pipeAmount][2], int neededReadPipe, int neededWritePipe){
	for(int i= 0;i<pipeAmount;i++){
	
		if(i==neededReadPipe){
			if(dup2(pipes[i][1], STDOUT_FILENO)==-1){ERROR_EXIT("dup-Error");}
		}
		else if(i==neededWritePipe){
			if(dup2(pipes[i][0], STDIN_FILENO)==-1){ERROR_EXIT("dup-Error");}
		}
		
		close(pipes[i][1]);
		close(pipes[i][0]);
		
	}
}

/**
*@brief Programm entry point
*@detail reads from stdin, splits the values and gives them to two child process
*reads values from pipes and calculates forkFFT, prints endvalues to stdout 
*no arguments are allowed
*amount of lines read from stdin has to be 2^n (n>=0)
*/
int main(int argc, char * argv[]) {
    fileName = argv[0];
	
	char ** readString = malloc(sizeof(char*)*MAX_AMOUNT_STRINGS);	//free
    int counter = 0;

    if (argc != 1) {
        USAGE();
    }

    char * line = NULL;	//read string from stdin
    ssize_t nread;	//errorvalue for getline
    size_t len = 0;	//length is irrelivant because line=null
    while ((nread = getline( & line, & len, stdin)) != -1) {
		readString[counter] = malloc(sizeof(char)*MAXLENGTH);
		strcpy(readString[counter],line);
        counter++;
    }
	

    if (counter == 1) {
		fprintf(stdout, "%s\n", readString[0]);
        SUCCESS_EXIT();
    }



	int pipes[4][2];

    if (pipe(pipes[PIPE_1_WRITE]) == -1 || pipe(pipes[PIPE_1_READ]) == -1 || pipe(pipes[PIPE_2_WRITE]) == -1 || pipe(pipes[PIPE_2_READ]) == -1) {
        ERROR_EXIT("Pipe-Error");
    }

    int p1;					//first child process
    if((p1 = fork())==-1){ERROR_EXIT("fork-Error");}				
    if (p1 == 0) {
		dupNeededPipes(4,pipes,PIPE_1_READ,PIPE_1_WRITE);
        if(execlp("./forkSort", "argv[0]", NULL)==-1){ERROR_EXIT("execlp-Error");}
    }
    int p2;					//second child process
    if((p2 = fork())==-1){ERROR_EXIT("fork-Error");}	
    if (p2 == 0) {
		dupNeededPipes(4, pipes,PIPE_2_READ,PIPE_2_WRITE);
        if(execlp("./forkSort", "argv[0]", NULL)==-1){ERROR_EXIT("execlp-Error");}
    }
    close(pipes[PIPE_1_WRITE][READ]);
    close(pipes[PIPE_2_WRITE][READ]);
    close(pipes[PIPE_1_READ][WRITE]);
    close(pipes[PIPE_2_READ][WRITE]);



	//write to pipes
	int i = 0;
	int writen1 = 0;
	int writen2 = 0;
    for (; i < counter/2; i++) {
        write(pipes[PIPE_1_WRITE][WRITE], readString[i], (strlen(readString[i]))); 
		writen1++;

	}
	for (; i < counter; i++) {
	    write(pipes[PIPE_2_WRITE][WRITE], readString[i], (strlen(readString[i])));      
		writen2++;

	}

    close(pipes[PIPE_1_WRITE][WRITE]);
    close(pipes[PIPE_2_WRITE][WRITE]);

    int state;					//return state of the child process (never checked)
    waitpid(p1, &state, WEXITED);

    waitpid(p2, &state, WEXITED);

    size_t length = 0;				//size of string for getline

    FILE * file1 = fdopen(pipes[PIPE_1_READ][READ], "r");	//opens pipe as file
    FILE * file2 = fdopen(pipes[PIPE_2_READ][READ], "r");	//opens pipe as file

	char ** readLines1 = malloc(sizeof(char*)*writen1);
	//read from first
    for (int k = 0; k < writen1; k++) {
		char * line;
        getline( & line, & length, file1);
		readLines1[k] = malloc(sizeof(char)*MAXLENGTH);

		strcpy(readLines1[k],line);
		readLines1[k][strlen(readLines1[k])-1]='\0';
		
    }
	//read from second
	char ** readLines2 = malloc(sizeof(char*)*writen2);
	for (int k = 0; k < writen2; k++) {
		char * line;
        getline( & line, & length, file2);				
		readLines2[k] = malloc(sizeof(char)*MAXLENGTH);

		strcpy(readLines2[k],line);
		readLines2[k][strlen(readLines2[k])-1]='\0';
    }
    
	//concat
	int first=0;
	int second=0;

	while(first!=writen1 && second!=writen2){
		
		if(strcmp(readLines1[first],readLines2[second])<0){
			fprintf(stdout,"%s\n",readLines1[first]);
			first++;
		} else{
			fprintf(stdout,"%s\n",readLines2[second]);
			second++;			
		}
	}
	while(first!=writen1 || second!=writen2){
		
		if(first!=writen1){
			fprintf(stdout,"%s\n",readLines1[first]);
			first++;
		} else{
			fprintf(stdout,"%s\n",readLines2[second]);
			second++;			
		}
	}
	
    SUCCESS_EXIT()
}
