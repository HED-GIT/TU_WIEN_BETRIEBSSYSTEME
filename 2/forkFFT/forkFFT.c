#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <regex.h>
#include <string.h>
#include <math.h>
#include <complex.h>
#include <errno.h>

#define PI 3.141592654
#define MAXLENGTH 100000    // refers to both, the max length of a single line read and the max amount of lines

#define USAGE()                                    \
    do                                             \
    {                                              \
        fprintf(stderr, "USAGE:\t%s\n", fileName); \
        exit(EXIT_FAILURE);                        \
    } while (0)

#define ERROR_EXIT(...)                                                 \
    do                                                                  \
    {                                                                   \
        fprintf(stderr, "%s ERROR: ", fileName);                        \
        fprintf(stderr, __VA_ARGS__);                                   \
        fprintf(stderr, "\n");                                          \
        exit(EXIT_FAILURE);                                             \
    } while (0)

#define SUCCESS_EXIT()                                  \
        exit(EXIT_SUCCESS)                              \

#define REGEX "^-?\\d*(\\.\\d*)?([[:blank:]]+-?\\d*(\\.\\d*)?\\s?\\*i)?"

typedef enum Pipe{
    PIPE_E_WRITE = 0,
    PIPE_O_WRITE = 1,
    PIPE_E_READ  = 2,
    PIPE_O_READ  = 3
} Pipe;

#define READ 0
#define WRITE 1

static const char *fileName;

/**
* @brief checks if the given string is a valid input, if not ends the programm
* @param the string to be checked
* @details string is not allowed to have a ' ' at the beginning
* everything after "*i" or '\n' is ignored
* string has to include a real number but doesn`t have to include an imaginary part
* all numbers have to be valid numbers (- only allowed once and only on the beginning, only one '.' per number)
* string is only allowed to contain {{'0'..'9'}, '.' , '-' , '*' , 'i' , '\n' , ' '}
*/

regex_t preg;
static void check_for_number(const char *text)
{
    if (regexec(&preg, text, 0, NULL, 0) == REG_NOMATCH)
    {
        ERROR_EXIT("invalid complex number");
    }
}

/**
 * @brief dups the needed pipes and closes the rest
 * @param pipeAmount: the amount of pipes given
 * @param pipes: a list of pipes
 * @param neededReadPipe: the readpipe that should redirect to stdout
 * @param neededWritePipe: the writepipe that should redirect to stdin
 * @details closes all pipes except the needed read and write pipes which are redirected
 * you do not necessary need this function, you can simply close all pipes for every child by hand and dup if you see this is seen as more readable
 */
static void dup_needed_pipes(int pipeAmount, int pipes[pipeAmount][2], Pipe neededReadPipe, Pipe neededWritePipe)
{
    for (int i = 0; i < pipeAmount; i++)
    {

        if (i == neededReadPipe)
        {
            if (dup2(pipes[i][1], STDOUT_FILENO) == -1)
            {
                ERROR_EXIT("dup-Error");
            }
        }
        else if (i == neededWritePipe)
        {
            if (dup2(pipes[i][0], STDIN_FILENO) == -1)
            {
                ERROR_EXIT("dup-Error");
            }
        }

        close(pipes[i][1]);
        close(pipes[i][0]);
    }
}

static double complex line_to_number(char* line){
    double complex ret = 0;

    check_for_number(line);

    errno = 0;
    ret = strtof(line, &line);
    if (!(*line != '\n' || *line != ' ') || errno != 0)
    {
        ERROR_EXIT("invalid number");
    }
    ret += strtof(line, &line) * I;

    if (!(*line != '\n' || *line != ' ') || errno != 0)
    {
        ERROR_EXIT("invalid number");
    }

    return ret;
}

/**
 * @brief reads the input from stdin
 * @param numbers place to save the read numbers
 * 
 * @returns amount of numbers read
 * 
 * @note in theory you could read a line and directly write it to the child process
 * the problem with that you do not know if you have to fork the program before you read twice since you do not fork programs if you only get one line of input
 * so the best way would be to read twice, check if you got results both times
 * if yes: fork the programs, pass the input to them and then continue reading and passing them on till you run out of lines
 * if no: print that one line (or error message if none is given) and exit
 * this would need the least amount of memory and would be faster because it would not require iterating over it twice
 * but this example should be as easy to understand as possible so i instead save the input and fork later
 */
static int read_input(double complex *numbers)
{
    char *line = NULL; //read string from stdin
    size_t len = 0;    //length is irrelivant because line=null
    
    int counter = 0;
    for (; getline(&line, &len, stdin) != -1; counter++)
    {
        numbers[counter] = line_to_number(line);        
    }
    free(line);
    return counter;
}

/**
 * @brief calculates the new numbers that will be and writes them to retNumbers
 * @param numberAmount amount of numbers for the calculation
 * @param fileE file the e values are written to
 * @param fileO file the o values are written to
 * @param retNumbers memory to save the numbers to
 */
static void handle_response(int numberAmount, FILE *fileE, FILE *fileO)
{
    double complex *newNumbers = malloc(sizeof(double complex) * (numberAmount/2)); //saves new calculated numbers to print later

    char *oline = NULL;       //read string with o values
    char *eline = NULL;       //read string with e values

    size_t olength = 0;       //size of string for getline
    size_t elength = 0;       //size of string for getline

    for (int k = 0; k < numberAmount / 2; k++)
    {
        double complex new1; //saves new calculated value
        double complex new2; //saves new calculated value

        getline(&eline, &elength, fileE);   // todo handle possible error
        
        double complex e = line_to_number(eline);

        getline(&oline, &olength, fileO);   // todo handle possible error
        
        double complex o = line_to_number(oline); 

        new1 = cos((-(2 * PI) / numberAmount) * k);
        new1 += sin((-(2 * PI) / numberAmount) * k) * I;
        new1 *= o;
        new1 += e;
        
        fprintf(stdout, "%.6f %.6f *i\n", creal(new1), cimag(new1));    // we can print this number immediately since those should be displayed in the same order as they are calcualted

        new2 = cos((-(2 * PI) / numberAmount) * k);
        new2 += sin((-(2 * PI) / numberAmount) * k) * I;
        new2 *= o;
        new2 = e - new2;
        newNumbers[k] = new2;                                           // we have to save this number since those are printed only AFTER all new1 are printed
    }
    free(oline);
    free(eline);


    for(int i = 0; i < numberAmount/2; i++){ // now print all the new2 numbers
        fprintf(stdout, "%.6f %.6f *i\n", creal(newNumbers[i]), cimag(newNumbers[i]));
    }

    free(newNumbers);
    regfree(&preg); // free regex, no longer needed

}

/**
 * @brief creates the child processes needed
 * @param pipes the createt pipes needed
 * @param pid_child_1 used to return the process id of the first child
 * @param pid_child_2 used to return the process id of the second child
 */
static void create_child_process(int pipes[4][2], int *pid_child_1, int *pid_child_2)
{

    if ((*pid_child_1 = fork()) == -1)
    {
        ERROR_EXIT("fork-Error");
    }

    if (*pid_child_1 == 0)
    {
        dup_needed_pipes(4, pipes, PIPE_E_READ, PIPE_E_WRITE);
        if (execlp(fileName, fileName, NULL) == -1)
        {
            ERROR_EXIT("execlp-Error");
        }
    }

    if ((*pid_child_2 = fork()) == -1)
    {
        ERROR_EXIT("fork-Error");
    }

    if (*pid_child_2 == 0)
    {
        dup_needed_pipes(4, pipes, PIPE_O_READ, PIPE_O_WRITE);
        if (execlp(fileName, fileName, NULL) == -1)
        {
            ERROR_EXIT("execlp-Error");
        }
    }
}

/**
 * @brief writes the calculated numbers to the child process
 */
static void write_to_child_process(int pipes[4][2], int numberAmount, const double complex *numbers)
{
    char floatToString[MAXLENGTH]; //string that will be written to the pipe

    for (int i = 0; i < numberAmount; i++)
    {

        snprintf(floatToString, sizeof(floatToString), "%.5f %.5f*i\n", creal(numbers[i]), cimag(numbers[i]));
        if (i % 2 == 1)
            write(pipes[PIPE_O_WRITE][WRITE], floatToString, (strlen(floatToString)));
        else
            write(pipes[PIPE_E_WRITE][WRITE], floatToString, (strlen(floatToString)));
    }
}

/**
 * @brief wait for the child processes to terminate
 * 
 * @param pid_child_1 id of child 1
 * @param pid_child_2 id of child 2
 */
static void wait_for_child_process(int pid_child_1, int pid_child_2)
{
    int state;
    waitpid(pid_child_1, &state, 0);
    if (WEXITSTATUS(state))
    {
        ERROR_EXIT("error in child");
    }

    waitpid(pid_child_2, &state, 0);
    if (WEXITSTATUS(state))
    {
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
int main(int argc, char **argv)
{
    fileName = argv[0];

    regcomp(&preg, REGEX, REG_EXTENDED);

    double complex *readNumbers = malloc(sizeof(double complex) * MAXLENGTH); //saves all numbers read from stdin

    if (argc != 1)
    {
        USAGE();
    }

    int numberAmount = read_input(readNumbers);

    if (numberAmount == 1)
    {
        fprintf(stdout, "%f %f *i\n", creal(readNumbers[0]), cimag(readNumbers[0]));

        free(readNumbers);

        SUCCESS_EXIT();
    }

    if (numberAmount % 2 != 0 || numberAmount == 0)
    {
        ERROR_EXIT("amount of input must be 2^n (n > 0)!");
    }

    int pipes[4][2];

    if (pipe(pipes[PIPE_E_WRITE]) == -1 || pipe(pipes[PIPE_E_READ]) == -1 || pipe(pipes[PIPE_O_WRITE]) == -1 || pipe(pipes[PIPE_O_READ]) == -1)
    {
        ERROR_EXIT("Pipe-Error");
    }

    int pid_1;
    int pid_2;
    create_child_process(pipes, &pid_1, &pid_2);

    close(pipes[PIPE_E_WRITE][READ]);
    close(pipes[PIPE_O_WRITE][READ]);
    close(pipes[PIPE_E_READ][WRITE]);
    close(pipes[PIPE_O_READ][WRITE]);

    write_to_child_process(pipes, numberAmount, readNumbers);

    free(readNumbers);

    close(pipes[PIPE_E_WRITE][WRITE]);
    close(pipes[PIPE_O_WRITE][WRITE]);

    wait_for_child_process(pid_1, pid_2);

    FILE *fileE = fdopen(pipes[PIPE_E_READ][READ], "r"); //opens pipe as file
    FILE *fileO = fdopen(pipes[PIPE_O_READ][READ], "r"); //opens pipe as file

    handle_response(numberAmount, fileE, fileO);

    fclose(fileE);
    fclose(fileO);
    SUCCESS_EXIT();
}