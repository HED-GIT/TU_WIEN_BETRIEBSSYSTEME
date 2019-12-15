#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/wait.h>
#include <fcntl.h> 
#include <string.h> 
#include <math.h>

#define PI 3.141592654
#define MAXLENGTH 100000

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
static ComplexNumber add(ComplexNumber x, ComplexNumber y) {
    ComplexNumber z;
    z.imaginary = x.imaginary + y.imaginary;
    z.real = x.real + y.real;
    return z;
}

/**
*@brief subtracts two complexNumbers
*@param the complexNumbers to subtract
*@return new complexNumber with the subtracted value
*/
static ComplexNumber subtract(ComplexNumber x, ComplexNumber y) {
    ComplexNumber z;
    z.imaginary = x.imaginary - y.imaginary;
    z.real = x.real - y.real;
    return z;
}

/**
*@brief multiplies two complexNumbers
*@param the complexNumbers to multiply
*@return new complexNumber with the multiplied value
*/
static ComplexNumber multiply(ComplexNumber x, ComplexNumber y) {
    ComplexNumber z;
    z.imaginary = ((x.real) * (y.imaginary) + (x.imaginary) * (y.real));
    z.real = (x.real) * (y.real) - (x.imaginary) * (y.imaginary);
    return z;
}

/**
*@brief creates a copy of a complexNumber
*@param the complexNumber to copy
*@return new complexNumber with values of complexNumber
*/
static ComplexNumber copy(ComplexNumber a) {
    ComplexNumber returnNumber;
    returnNumber.real = a.real;
    returnNumber.imaginary = a.imaginary;
    return returnNumber;
}

/**
*@brief prints out errormessage to stderr and ends the programm
*@param the error message and the name of the program
*/
static void standardError(char text[MAXLENGTH],char argv[]) {
    fprintf(stderr, "%s:  %s\n", argv ,text);
    fprintf(stderr, "break\n\n");
    exit(EXIT_FAILURE);

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
static void checkForNumber(char text[]) {
    int i = 0;			//position on string
    int dots = 0;		//amount of dots already found
    int dotThisNumber = 0;	//amount of dots in current number
    int readingNumber = 1;	//1 if currently reading a number else 0
    int numbers = 1;		//counts amount of numbers already read
    int firstChar = 0;		//says if the first char was already read
    while (text[i] != '\n') {
        if (text[i] == '0' || text[i] == '1' || text[i] == '2' || text[i] == '3' || text[i] == '4' || text[i] == '5' || text[i] == '6' || text[i] == '7' || text[i] == '8' || text[i] == '9') {
            readingNumber = 1;
            firstChar = 1;
        } else if (text[i] == '-') {
            if (readingNumber == 1 && firstChar != 0) {
                standardError("wrong - position","./forkFFT");
            } else {
                readingNumber = 1;
            }
        } else if (text[i] == '*') {
            if (text[i + 1] == 'i' && numbers == 2)
                return;
            else {
                standardError("no real Number(only imaginary is optional)!\nOr it doesn't terminate with \"*i\"","./forkFFT");
            }
        } else if (text[i] == '.' && (dotThisNumber == 1 || dots == 2 || readingNumber == 0)) {
            standardError("input Error","./forkFFT");
        } else if (text[i] == '.') {
            dotThisNumber++;
            dots++;
        } else if (text[i] == ' ') {
            dotThisNumber = 0;
            if (firstChar == 0) {
                standardError("space as first char is not allowed","./forkFFT");
            }
            if (readingNumber == 1)
                numbers++;
            readingNumber = 0;
        } else {
            standardError("invalid Value in imputstring","./forkFFT");
        }
        i++;
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
    ComplexNumber readNumbers[MAXLENGTH];	//saves all numbers read from stdin
    int counter = 0;					//saves amount of numbers read from stdin

    if (argc != 1) {
        standardError("no arguments allowed",argv[0]);
    }

    char * line = NULL;	//read string from stdin
    ssize_t nread;	//errorvalue for getline
    size_t len = 0;	//length is irrelivant because line=null
    char * end;		//pointer to position after strtof
    while ((nread = getline( & line, & len, stdin)) != -1) {

        readNumbers[counter].real = strtof(line, & end);
        checkForNumber(line);
        if (!( * end != '\n' || * end != ' ')) {
            standardError("test",argv[0]);
        }
        readNumbers[counter].imaginary = strtof(end, & end);

        if (!( * end != '\n' || * end != ' ' || * end != '*')) {
            standardError("test",argv[0]);
        }
        counter++;
    }

    if (counter == 1) {
        fprintf(stdout, "%f %f *i\n", readNumbers[0].real, readNumbers[0].imaginary);
        exit(EXIT_SUCCESS);
    }
    if (counter % 2 != 0) {
        standardError("amount of input must be 2^n!",argv[0]);
    }

    ComplexNumber newNumbers[counter];	//saves new calculated numbers

    int pipeEWrite[2];				//pipes for writing/reading to to/from child process
    int pipeERead[2];
    int pipeOWrite[2];
    int pipeORead[2];
    if (pipe(pipeEWrite) == -1 || pipe(pipeERead) == -1 || pipe(pipeOWrite) == -1 || pipe(pipeORead) == -1) {
        standardError("Pipe-Error",argv[0]);
    }

    int p1;					//first child process
    if((p1 = fork())==-1){standardError("fork-Error",argv[0]);}				
    if (p1 == 0) {
        close(pipeEWrite[1]);
        close(pipeERead[0]);
        close(pipeOWrite[1]);
        close(pipeOWrite[0]);
        close(pipeORead[0]);
        close(pipeORead[1]);
        if(dup2(pipeEWrite[0], STDIN_FILENO)==-1){standardError("dup-Error",argv[0]);}
        if(dup2(pipeERead[1], STDOUT_FILENO)==-1){standardError("dup-Error",argv[0]);}
        close(pipeEWrite[0]);
        close(pipeERead[1]);
        if(execlp("./forkFFT", "argv[0]", NULL)==-1){standardError("execlp-Error",argv[0]);}
    }
    int p2;					//second child process
    if((p2 = fork())==-1){standardError("fork-Error",argv[0]);}	
    if (p2 == 0) {
        close(pipeOWrite[1]);
        close(pipeORead[0]);
        close(pipeEWrite[1]);
        close(pipeEWrite[0]);
        close(pipeERead[0]);
        close(pipeERead[1]);
        if(dup2(pipeOWrite[0], STDIN_FILENO)==-1){standardError("dup-Error",argv[0]);}
        if(dup2(pipeORead[1], STDOUT_FILENO)==-1){standardError("dup-Error",argv[0]);}
        close(pipeOWrite[0]);
        close(pipeORead[1]);
        if(execlp("./forkFFT", "argv[0]", NULL)==-1){standardError("execlp-Error",argv[0]);}
    }
    close(pipeEWrite[0]);
    close(pipeOWrite[0]);
    close(pipeERead[1]);
    close(pipeORead[1]);
    char floatToString[MAXLENGTH];		//string that will be writen to the pipe

    for (int i = 0; i < counter; i++) {

        snprintf(floatToString, sizeof(floatToString), "%.5f %.5f*i\n", readNumbers[i].real, readNumbers[i].imaginary);
        if (i % 2 == 1)
            write(pipeEWrite[1], floatToString, (strlen(floatToString)));
        else
            write(pipeOWrite[1], floatToString, (strlen(floatToString)));
    }
    close(pipeEWrite[1]);
    close(pipeOWrite[1]);

    int state;					//return state of the child process (never checked)
    waitpid(p1, &state, WEXITED);

    waitpid(p2, &state, WEXITED);

    char * oline = NULL;			//read string with o values
    char * eline = NULL;			//read string with e values
    size_t length = 0;				//size of string for getline

    FILE * fileE = fdopen(pipeERead[0], "r");	//opens pipe as file
    FILE * fileO = fdopen(pipeORead[0], "r");	//opens pipe as file

    for (int k = 0; k < counter / 2; k++) {
        ComplexNumber e; 		//saves read E-value
        ComplexNumber o;			//saves read O-value
	//E is actually O and O is actually E (writes wrong values to wrong pipe)
        ComplexNumber new1;		//saves new calculated value
        ComplexNumber new2;		//saves new calculated value
        getline( & eline, & length, fileE);
        e.real = strtof(eline, & eline);
        e.imaginary = strtof(eline, & eline);

        getline( & oline, & length, fileO);
        o.real = strtof(oline, & oline);
        o.imaginary = strtof(oline, & oline);

        new1.real = cos((-((2 * PI) / counter)) * k);
        new1.imaginary = sin((-((2 * PI) / counter)) * k);
        new1 = multiply(new1, e);
        new1 = add(o, new1);
        newNumbers[k] = copy(new1);

        new2.real = cos((-((2 * PI) / counter)) * (k));
        new2.imaginary = sin((-((2 * PI) / counter)) * (k));
        new2 = multiply(new2, e);
        new2 = subtract(o, new2);
        newNumbers[k + (counter / 2)] = copy(new2);
    }
    for (int i = 0; i < counter; i++) {
        fprintf(stdout, "%.6f %.6f *i\n", newNumbers[i].real, newNumbers[i].imaginary);
    }
    exit(EXIT_SUCCESS);
}
