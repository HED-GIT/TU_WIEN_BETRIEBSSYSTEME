#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>

#define USAGE() {fprintf(stderr,"USAGE:\t%s\n",fileName); exit(EXIT_FAILURE);}
#define ERROR_EXIT(...) { fprintf(stderr, "%s ERROR: " __VA_ARGS__"\n",fileName); exit(EXIT_FAILURE); }
#define SUCCESS_EXIT() {exit(EXIT_SUCCESS);}

#define MAXLENGTH 1024
#define HEXDIGITS "0123456789ABCDEFabcdef"

#define WRITE 1
#define READ 0

#define READ_CHILD_HH 0
#define WRITE_CHILD_HH (READ_CHILD_HH+1)
#define READ_CHILD_LH 2
#define WRITE_CHILD_LH (READ_CHILD_LH+1)
#define READ_CHILD_HL 4
#define WRITE_CHILD_HL (READ_CHILD_HL+1)
#define READ_CHILD_LL 6
#define WRITE_CHILD_LL (READ_CHILD_LL+1)

char * fileName;

static int isHex(char *hexString);
static void readInput(char *firstString, char *secondString);
static void multHexChar(char *a, const char *b);
static void addHexCharOverflow(char *a, const char *b, char *overflow);
static void addHex(char *firstHex, const char *secondHex);
static void addXZeros(char * a, int count);

/**
*@brief checks if a string is a hex-number
*@param pointer to the string to check
*@return 0 if it is a hexstring else 1
*/
static int isHex(char *hexString) {
	return (strspn(hexString, HEXDIGITS) == strlen(hexString));
}

/**
*@brief reads input from stdin
*@param pointer to were the strings should be saved
*/
static void readInput(char *firstString, char *secondString) {
	fgets(firstString, MAXLENGTH, stdin);
	fgets(secondString, MAXLENGTH, stdin);
	firstString[strlen(firstString) - 1] = '\0';
	secondString[strlen(secondString) - 1] = '\0';

	if (!isHex(secondString) || !isHex(firstString)) {
		ERROR_EXIT("The input is not a valid HEX-String");
	}
	else if (strlen(firstString) != strlen(secondString)) {
		ERROR_EXIT("The length of both strings must be equal");
	}
	else if ((((strlen(firstString) - 1) / 2) * 2 == strlen(firstString) - 1) && strlen(firstString) != 1) {
		ERROR_EXIT("The number of digits is not even or 1");
	}
}

/**
*@brief multiplies two hex numbers
*@param the two numbers to add, the first will return the multiplied number
*/
static void multHexChar(char *a, const char *b) {
	int value = (int)strtol(a, NULL, 16)*(int)strtol(b, NULL, 16);
	sprintf(a, "%x", value);
}

/**
*@brief adds two hex-numbers and adds an overflow
*@param the numbers to add together, value returned in first parameter, overflow in last one
*/
static void addHexCharOverflow(char *a, const char *b, char *overflow) {
	int value = (int)strtol(a, NULL, 16) + (int)strtol(b, NULL, 16) + (int)strtol(overflow, NULL, 16);
	sprintf(a, "%x", value % 16);
	sprintf(overflow, "%x", value / 16);
}

/**
*@brief adds two hex-strings together
*@param the numbers to add together, value returned in first parameter
*/
static void addHex(char *firstHex, const char *secondHex) {
	char overflow[2] = "0\0";
	char firstChar[2] = "0\0";
	char secondChar[2] = "0\0";
	int dif = strlen(firstHex) - strlen(secondHex);
	for (int i = strlen(firstHex) - 1; i >= 0; i--)
	{
		firstChar[0] = firstHex[i];
		secondChar[0] = (i - dif < 0) ? '0' : secondHex[i - dif];
		addHexCharOverflow(firstChar, secondChar, overflow);
		firstHex[i] = firstChar[0];
	}

	if (overflow[0] != '0') {
		for (int i = strlen(firstHex); i >= 0; i--)
		{
			firstHex[i + 1] = firstHex[i];
		}
		firstHex[0] = overflow[0];
	}
}

/**
*@brief adds 0 to the end of a string
*@param pointer to the string and amount of 0 to add
*/
static void addXZeros(char * a, int count) {
	int length = strlen(a);
	for (int i = 0; count > i; i++) {
		a[length] = '0';
		length++;
	}
	a[length] = '\0';
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

/**
*@brief Programm entry point
*@detail reads from stdin, splits the values and gives them to four child process
*reads values from pipes and calculates the value from their response
*no arguments are allowed
*amount of lines read from stdin has to be 2^n (n>=0)
*/
int main(int argc, char *argv[]) {
	int length;
	fileName = argv[0];
	char firstString[MAXLENGTH];
	char secondString[MAXLENGTH];

	if (argc != 1) {
		USAGE();
	}

	readInput(firstString, secondString);

	length = strlen(firstString) / 2;

	if (strlen(firstString) == 1) {
		multHexChar(firstString, secondString);
		fprintf(stdout, "%s", firstString);
		SUCCESS_EXIT();
	}

	char Al[length + 1];
	char Bh[length + 1];
	char Bl[length + 1];
	char Ah[length + 1];


	for (int i = 0; i < length; i++) {
		Ah[i] = firstString[i];
		Bh[i] = secondString[i];
		Al[i] = firstString[length + i];
		Bl[i] = secondString[length + i];
		Ah[i + 1] = '\n';
		Bh[i + 1] = '\n';
		Al[i + 1] = '\n';
		Bl[i + 1] = '\n';
		Ah[i + 2] = '\0';
		Bh[i + 2] = '\0';
		Al[i + 2] = '\0';
		Bl[i + 2] = '\0';
	}
	
	int pipes[8][2];

	int pid[4];


	if (pipe(pipes[READ_CHILD_HH]) == -1 || pipe(pipes[WRITE_CHILD_HH]) == -1
		|| pipe(pipes[READ_CHILD_HL]) == -1 || pipe(pipes[WRITE_CHILD_HL]) == -1
		|| pipe(pipes[READ_CHILD_LH]) == -1 || pipe(pipes[WRITE_CHILD_LH]) == -1
		|| pipe(pipes[READ_CHILD_LL]) == -1 || pipe(pipes[WRITE_CHILD_LL]) == -1) {
		ERROR_EXIT("Error when opening pipes");
	}
	
	//create child processes
	for(int i; i<4;i++){
		pid[i] = fork();
		if(pid[i]<0){
			ERROR_EXIT("Error at forking");
		}
		else if (pid[i] == 0) {
			dupNeededPipes(8,pipes,i*2,i*2+1);

			if (execlp(argv[0], argv[0], NULL) == -1) {
				ERROR_EXIT("Error on execlp");
			}
		}
	}
	

	{

		// close all reading ends of writing pipe
		close(pipes[WRITE_CHILD_HH][READ]);
		close(pipes[WRITE_CHILD_HL][READ]);
		close(pipes[WRITE_CHILD_LH][READ]);
		close(pipes[WRITE_CHILD_LL][READ]);

		//writing

		write(pipes[WRITE_CHILD_HH][WRITE], Ah, strlen(Ah));
		write(pipes[WRITE_CHILD_HH][WRITE], Bh, strlen(Bh));
		close(pipes[WRITE_CHILD_HH][WRITE]);

		write(pipes[WRITE_CHILD_HL][WRITE], Ah, strlen(Ah));
		write(pipes[WRITE_CHILD_HL][WRITE], Bl, strlen(Bl));
		close(pipes[WRITE_CHILD_HL][WRITE]);

		write(pipes[WRITE_CHILD_LH][WRITE], Al, strlen(Al));
		write(pipes[WRITE_CHILD_LH][WRITE], Bh, strlen(Bh));
		close(pipes[WRITE_CHILD_LH][WRITE]);

		write(pipes[WRITE_CHILD_LL][WRITE], Al, strlen(Al));
		write(pipes[WRITE_CHILD_LL][WRITE], Bl, strlen(Bl));
		close(pipes[WRITE_CHILD_LL][WRITE]);

		// Wait for child
		int status[4];
		waitpid(pid[0], &status[0], 0);
		waitpid(pid[1], &status[1], 0);
		waitpid(pid[2], &status[2], 0);
		waitpid(pid[3], &status[3], 0);

		if (WEXITSTATUS(status[0]) == 1 || WEXITSTATUS(status[1]) == 1 || WEXITSTATUS(status[2]) == 1 || WEXITSTATUS(status[3]) == 1) {
			ERROR_EXIT("Error in the childprocess");
			exit(EXIT_FAILURE);
		}

		close(pipes[WRITE_CHILD_HH][WRITE]);
		close(pipes[WRITE_CHILD_HL][WRITE]);
		close(pipes[WRITE_CHILD_LH][WRITE]);
		close(pipes[WRITE_CHILD_LL][WRITE]);
	}

	// Read string from child and close reading end.
	char returnChildHH[2 * length + length * 2 + 2];
	char returnChildHL[2 * length + length + 2];
	char returnChildLH[2 * length + length + 2];
	char returnChildLL[2 * length + 2];

	{
		int rv;

		rv = read(pipes[READ_CHILD_HH][READ], returnChildHH, length *2);
		returnChildHH[rv] = '\0';
		close(pipes[READ_CHILD_HH][READ]);

		rv = read(pipes[READ_CHILD_HL][READ], returnChildHL, length *2);
		returnChildHL[rv] = '\0';
		close(pipes[READ_CHILD_HL][READ]);

		rv = read(pipes[READ_CHILD_LH][READ], returnChildLH, length *2);
		returnChildLH[rv] = '\0';
		close(pipes[READ_CHILD_LH][READ]);

		rv = read(pipes[READ_CHILD_LL][READ], returnChildLL, length *2);
		returnChildLL[rv] = '\0';
		close(pipes[READ_CHILD_LL][READ]);
	}

	//calculation

	addXZeros(returnChildHH, length * 2);
	addXZeros(returnChildHL, length);
	addXZeros(returnChildLH, length);


	addHex(returnChildHH, returnChildHL);
	addHex(returnChildHH, returnChildLH);
	addHex(returnChildHH, returnChildLL);



	for (int i = 0; i < strlen(returnChildHH); i++) {
		fprintf(stdout, "%c", returnChildHH[i]);
	}
	fprintf(stdout, "\n");
	SUCCESS_EXIT();
}