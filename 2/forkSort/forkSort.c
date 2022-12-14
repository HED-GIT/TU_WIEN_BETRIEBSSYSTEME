#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

#define MAXLENGTH 100000
#define MAX_AMOUNT_STRINGS 100

#define USAGE()                                    \
	do                                             \
	{                                              \
		fprintf(stderr, "USAGE:\t%s\n", fileName); \
		exit(EXIT_FAILURE);                        \
	} while (0)
	
#define ERROR_EXIT(...)                                           \
	do                                                            \
	{                                                             \
		fprintf(stderr, "%s ERROR: " __VA_ARGS__ "\n", fileName); \
		exit(EXIT_FAILURE);                                       \
	} while (0)

#define SUCCESS_EXIT()      							\
	do                      							\
	{                       							\
	    fprintf(stderr, "%s Success\n", fileName);      \
		exit(EXIT_SUCCESS); 							\
	} while (0)

#define PIPE_1_WRITE 0
#define PIPE_2_WRITE 1
#define PIPE_1_READ 2
#define PIPE_2_READ 3

#define READ 0
#define WRITE 1

static const char *fileName;

/**
 * @brief dups the needed pipes and closes the rest
 * @param pipeAmount: the amount of pipes given
 * @param pipes: a list of pipes
 * @param neededReadPipe: the readpipe that should redirect to stdout
 * @param neededWritePipe: the writepipe that should redirect to stdin
 * @details closes all pipes except the needed read and write pipes which are redirected
 * you do not necessary need this function, you can simply close all pipes for every child by hand and dup if you see this is seen as more readable
 */
static void dup_needed_pipes(int pipeAmount, int pipes[pipeAmount][2], int neededReadPipe, int neededWritePipe)
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

/**
 * @brief reads the input from stdin
 * @param amount of strings read
 * 
 * @returns pointer to memory of read strings
 * 
 *
 * @note in theory you could read a line and directly write it to the child process
 * the problem with that you do not know if you have to fork the program before you read twice since you do not fork programs if you only get one line of input
 * so the best way would be to read twice, check if you got results both times
 * if yes: fork the programs, pass the input to them and then continue reading and passing them on till you run out of lines
 * if no: print that one line (or error message if none is given) and exit
 * this would need the least amount of memory and would be faster because it would not require iterating over it twice
 * but this example should be as easy to understand as possible so i instead save the input and fork later
 */
static char **get_input(int* counter){
	char **readString = malloc(sizeof(char *) * MAX_AMOUNT_STRINGS);

	char *line = NULL; //read string from stdin
	ssize_t nread;	   //errorvalue for getline
	size_t len = 0;	   //length is irrelivant because line=null
	while ((nread = getline(&line, &len, stdin)) != -1)
	{
		readString[*counter] = line;
		(*counter)++;
		line = NULL;
		len = 0;
	}

	free(line);	// gerline demands that line is freed even if an error (in this case end of file) occurs
	return readString;
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
        dup_needed_pipes(4, pipes, PIPE_1_READ, PIPE_1_WRITE);
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
        dup_needed_pipes(4, pipes, PIPE_2_READ, PIPE_2_WRITE);
        if (execlp(fileName, fileName, NULL) == -1)
        {
            ERROR_EXIT("execlp-Error");
        }
    }
}


/**
 * @brief writes the lines to the child process
 */
static void write_to_child_process(int pipes[4][2], int count, char** lines){
	int i = 0;
	for (; i < count / 2; i++)
	{
		write(pipes[PIPE_1_WRITE][WRITE], lines[i], (strlen(lines[i])));
		free(lines[i]);
	}
	for (; i < count; i++)
	{
		write(pipes[PIPE_2_WRITE][WRITE], lines[i], (strlen(lines[i])));
		free(lines[i]);
	}
	free(lines);
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
*amount of lines read from stdin has to be 2^n (n>=0)
*/
int main(int argc, char *argv[])
{
	fileName = argv[0];

	int counter = 0;

	if (argc != 1)
	{
		USAGE();
	}

	char **readString = get_input(&counter);

	if (counter == 1)
	{
		fprintf(stdout, "%s", readString[0]);

		free(readString[0]);
		free(readString);

		SUCCESS_EXIT();
	}

	int pipes[4][2];

	if (pipe(pipes[PIPE_1_WRITE]) == -1 || pipe(pipes[PIPE_1_READ]) == -1 || pipe(pipes[PIPE_2_WRITE]) == -1 || pipe(pipes[PIPE_2_READ]) == -1)
	{
		ERROR_EXIT("Pipe-Error");
	}

	int p1, p2; //first child process

	create_child_process(pipes, &p1, &p2);

	close(pipes[PIPE_1_WRITE][READ]);
	close(pipes[PIPE_2_WRITE][READ]);
	close(pipes[PIPE_1_READ][WRITE]);
	close(pipes[PIPE_2_READ][WRITE]);

	write_to_child_process(pipes,counter, readString);

	close(pipes[PIPE_1_WRITE][WRITE]);
	close(pipes[PIPE_2_WRITE][WRITE]);

    wait_for_child_process(p1, p2);

	FILE *file1 = fdopen(pipes[PIPE_1_READ][READ], "r"); //opens pipe as file
	FILE *file2 = fdopen(pipes[PIPE_2_READ][READ], "r"); //opens pipe as file

	
	char *line1 = NULL;
	char *line2 = NULL;
	size_t length1 = 0;
	size_t length2 = 0;
	ssize_t ret1 = getline(&line1, &length1, file1);
	ssize_t ret2 = getline(&line2, &length2, file2);

	// fetch one line from both
	// compare
	// print smaller and fetch a new one from that one
	// repeat till out of lines
	do
	{
		if (strcmp(line1, line2) < 0)
		{
			fprintf(stdout, "%s", line1);
			ret1 = getline(&line1, &length1, file1);
		}
		else{
			fprintf(stdout, "%s", line2);
			ret2 = getline(&line2, &length2, file2);
		}

	} while (ret1 != -1 && ret2 != -1);

	// if lines remain print them all
	while(ret1 != -1){
		fprintf(stdout,"%s", line1);
				
		ret1 = getline(&line1, &length1, file1);
	}

	while(ret2 != -1){
		fprintf(stdout,"%s", line2);
				
		ret2 = getline(&line2, &length2, file2);
	}

	free(line1);
	free(line2);

	fclose(file1);
	fclose(file2);


	SUCCESS_EXIT();
}
