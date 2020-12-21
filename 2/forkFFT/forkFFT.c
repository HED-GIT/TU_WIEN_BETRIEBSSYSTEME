#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/wait.h>
#include <sys/types.h>
#include <regex.h>
#include <fcntl.h> 
#include <string.h> 
#include <math.h>
#include <ctype.h>

#define PI 3.141592654
#define MAXLENGTH 100000

#define USAGE() do{fprintf(stderr,"USAGE:\t%s\n",fileName); exit(EXIT_FAILURE);}while(0)
#define ERROR_EXIT(...) do{ fprintf(stderr, "%s ERROR: " __VA_ARGS__"\n",fileName); exit(EXIT_FAILURE); }while(0)
#define SUCCESS_EXIT() do{exit(EXIT_SUCCESS);}while(0)

#define REGEX "\\d*(.\\d*)?(\\s\\d*(.\\d*)?\\s\\*i)?\\n?"

#define PIPE_E_WRITE 0
#define PIPE_O_WRITE 1
#define PIPE_E_READ 2
#define PIPE_O_READ 3

#define READ 0
#define WRITE 1

static char * fileName;

/**
*@brief saves the real and the imaginary part of a complex number
*/
typedef struct ComplexNumber {
    float imaginary;
    float real;

}ComplexNumber;

/**
*@brief adds two complexNumbers
*@param the complexNumbers to add
*@return new complexNumber with the added value
*/
static ComplexNumber add(const ComplexNumber * x, const ComplexNumber * y) {
    ComplexNumber ret = { 
        x->imaginary + y->imaginary,
        x->real + y->real
    };
    return ret;
}

/**
*@brief subtracts two complexNumbers
*@param the complexNumbers to subtract 
*@return new complexNumber with the subtracted value
*/
static ComplexNumber subtract(const ComplexNumber * x, const ComplexNumber * y) {
    ComplexNumber ret = { 
        x->imaginary - y->imaginary,
        x->real - y->real
    };
    return ret;
}

/**
*@brief multiplies two complexNumbers
*@param the complexNumbers to multiply
*@return new complexNumber with the multiplied value
*/
static ComplexNumber multiply(const ComplexNumber * x, const ComplexNumber * y) {
    ComplexNumber ret = {
        ((x->real) * (y->imaginary) + (x->imaginary) * (y->real)),
        (x->real) * (y->real) - (x->imaginary) * (y->imaginary)
    };
    return ret;
}

/**
*@brief checks if the given string is a valid input, if not ends the programm
*@param the string to be checked
*@detail string is not allowed to have a ' ' at the beginning
*everything after "*i" or '\n' is ignored
*string has to include a real number but doesn`t have to include an imaginary part
*all numbers have to be valid numbers (- only allowed once and only on the beginning, only one '.' per number)
*string is only allowed to contain {{'0'..'9'}, '.' , '-' , '*' , 'i' , '\n' , ' '}
*/

regex_t preg;
static void checkForNumber(char * text) {
    if(regexec(&preg, text,0,NULL,0)==REG_NOMATCH){
		ERROR_EXIT("invalid complexNumber");
	}
}

static void regexInit(){
	regcomp(&preg, REGEX,REG_EXTENDED);
}

static void freeRegex(){
	regfree(&preg);
}

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

static int readInput(ComplexNumber * numbers){
    char * line = NULL;	    //read string from stdin
    ssize_t nread;	        //errorvalue for getline
    size_t len = 0;	        //length is irrelivant because line=null
    char * end;		        //pointer to position after strtof
    int counter = 0;
    while ((nread = getline( & line, & len, stdin)) != -1) {

        numbers[counter].real = strtof(line, & end);
        checkForNumber(line);
        if (!( * end != '\n' || * end != ' ')) {
            ERROR_EXIT("invalid number");
        }
        numbers[counter].imaginary = strtof(end, & end);

        if (!( * end != '\n' || * end != ' ')) {
            ERROR_EXIT("invalid number");
        }
        counter++;
    }
    freeRegex();
    return counter;
}

static void calculateNewNumbers(int numberAmount, FILE *fileE, FILE *fileO, ComplexNumber *retNumbers){
    char * oline = NULL;			//read string with o values
    char * eline = NULL;			//read string with e values
    size_t length = 0;				//size of string for getline
    
    for (int k = 0; k < numberAmount / 2; k++) {
        ComplexNumber e; 		//saves read E-value
        ComplexNumber o;		//saves read O-value

        ComplexNumber new1;		//saves new calculated value
        ComplexNumber new2;		//saves new calculated value

        getline( & eline, & length, fileE);
        e.real = strtof(eline, & eline);
        e.imaginary = strtof(eline, & eline);

        getline( & oline, & length, fileO);
        o.real = strtof(oline, & oline);
        o.imaginary = strtof(oline, & oline);

        new1.real = cos((-((2 * PI) / numberAmount)) * k);
        new1.imaginary = sin((-((2 * PI) / numberAmount)) * k);
        new1 = multiply(&new1, &o);
		new1 = add(&new1,&e);
        retNumbers[k] = new1;

        new2.real = cos((-((2 * PI) / numberAmount)) * (k));
        new2.imaginary = sin((-((2 * PI) / numberAmount)) * (k));
        new2 = multiply(&new2, &o);
        new2 = subtract(&e, &new2);
        retNumbers[k + (numberAmount / 2)] = new2;

    }
}

static void createChildProcess(int pipes[4][2], int * pid_child_1, int * pid_child_2){
    
    if((*pid_child_1 = fork())==-1){
        ERROR_EXIT("fork-Error");
    }		

    if (*pid_child_1 == 0) {
		dupNeededPipes(4,pipes,PIPE_E_READ,PIPE_E_WRITE);
        if(execlp(fileName, fileName, NULL)==-1){ERROR_EXIT("execlp-Error");}
    }

    if((*pid_child_2 = fork())==-1){
        ERROR_EXIT("fork-Error");
    }

    if (*pid_child_2 == 0) {
		dupNeededPipes(4, pipes,PIPE_O_READ,PIPE_O_WRITE);
        if(execlp(fileName, fileName, NULL)==-1){ERROR_EXIT("execlp-Error");}
    }
    
}

static void writeToChildProcess(int pipes[4][2],int numberAmount, ComplexNumber * numbers){
    char floatToString[MAXLENGTH];		//string that will be writen to the pipe

    for (int i = 0; i < numberAmount; i++) {

        snprintf(floatToString, sizeof(floatToString), "%.5f %.5f*i\n", numbers[i].real, numbers[i].imaginary);
        if (i % 2 == 1)
            write(pipes[PIPE_O_WRITE][WRITE], floatToString, (strlen(floatToString)));
        else
            write(pipes[PIPE_E_WRITE][WRITE], floatToString, (strlen(floatToString)));
    }
}

static void waitForChildProcess(int pid_child_1, int pid_child_2){
    int state;
    waitpid(pid_child_1, &state,0);
	if(WEXITSTATUS(state)){
		ERROR_EXIT("error in child");
	}


    waitpid(pid_child_2, &state, 0);
	if(WEXITSTATUS(state)){
		ERROR_EXIT("error in child");
	}
}

/**
*@brief Programm entry point
*@detail reads from stdin, splits the values and gives them to two child process
*reads values from pipes and calculates forkFFT, prints endvalues to stdout 
*no arguments are allowed
*amount of lines read from stdin has to be 2^n (n>0)
*/
int main(int argc, char ** argv) {
    fileName = argv[0];
	
	regexInit();
	
	ComplexNumber readNumbers[MAXLENGTH];	//saves all numbers read from stdin
					//saves amount of numbers read from stdin

    if (argc != 1) {
        USAGE();
    }

    int numberAmount = readInput(readNumbers);

    if (numberAmount == 1) {
        fprintf(stdout, "%f %f *i\n", readNumbers[0].real, readNumbers[0].imaginary);
        SUCCESS_EXIT();
    }

    if (numberAmount % 2 != 0 || numberAmount == 0) {
        ERROR_EXIT("amount of input must be 2^n (n > 0)!");
    }

	int pipes[4][2];

    if (pipe(pipes[PIPE_E_WRITE]) == -1 || pipe(pipes[PIPE_E_READ]) == -1 || pipe(pipes[PIPE_O_WRITE]) == -1 || pipe(pipes[PIPE_O_READ]) == -1) {
        ERROR_EXIT("Pipe-Error");
    }

    int pid_1;
    int pid_2;
    createChildProcess(pipes,&pid_1, &pid_2);

    close(pipes[PIPE_E_WRITE][READ]);
    close(pipes[PIPE_O_WRITE][READ]);
    close(pipes[PIPE_E_READ][WRITE]);
    close(pipes[PIPE_O_READ][WRITE]);

    writeToChildProcess(pipes,numberAmount,readNumbers);

    close(pipes[PIPE_E_WRITE][WRITE]);
    close(pipes[PIPE_O_WRITE][WRITE]);

    waitForChildProcess(pid_1,pid_2);

    FILE * fileE = fdopen(pipes[PIPE_E_READ][READ], "r");	//opens pipe as file
    FILE * fileO = fdopen(pipes[PIPE_O_READ][READ], "r");	//opens pipe as file

    ComplexNumber * newNumbers = malloc(sizeof(ComplexNumber)*numberAmount);	//saves new calculated numbers
    calculateNewNumbers(numberAmount,fileE,fileO,newNumbers);

    for (int i = 0; i < numberAmount; i++) {
        fprintf(stdout, "%.6f %.6f *i\n", newNumbers[i].real, newNumbers[i].imaginary);
	}

	fclose(fileE);
    fclose(fileO);
	free(newNumbers);
    SUCCESS_EXIT();
}