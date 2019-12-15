#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>

#define USAGE() {fprintf(stderr,"USAGE:\t./intmul\n"); exit(EXIT_FAILURE);}
#define ERROR_EXIT(...) { fprintf(stderr, "%s ERROR: " __VA_ARGS__"\n",fileName); exit(EXIT_FAILURE); }
#define SUCCESS_EXIT() {exit(EXIT_SUCCESS);}

#define MAXLENGTH 1024
#define HEXDIGITS "0123456789ABCDEFabcdef"

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
	if (strspn(hexString, HEXDIGITS) == strlen(hexString)) {
		return 0;
	}
	else {
		return 1;
	}
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

	if (isHex(secondString) != 0 || isHex(firstString) != 0) {
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
	int fd1ChildHH[2];
	int fd2ChildHH[2];

	int fd1ChildHL[2];
	int fd2ChildHL[2];

	int fd1ChildLH[2];
	int fd2ChildLH[2];

	int fd1ChildLL[2];
	int fd2ChildLL[2];

	int pid0, pid1, pid2, pid3;

	if (pipe(fd1ChildHH) == -1 || pipe(fd2ChildHH) == -1
		|| pipe(fd1ChildHL) == -1 || pipe(fd2ChildHL) == -1
		|| pipe(fd1ChildLH) == -1 || pipe(fd2ChildLH) == -1
		|| pipe(fd1ChildLL) == -1 || pipe(fd2ChildLL) == -1) {
		ERROR_EXIT("Error when opening pipes");
	}

	{
		pid0 = fork();
		if (pid0 < 0) {
			ERROR_EXIT("Error at forking");
		}

		else if (pid0 == 0) {
			close(fd1ChildHL[0]);
			close(fd1ChildLH[0]);
			close(fd1ChildLL[0]);
			close(fd2ChildHL[0]);
			close(fd2ChildLH[0]);
			close(fd2ChildLL[0]);
			close(fd1ChildHL[1]);
			close(fd1ChildLH[1]);
			close(fd1ChildLL[1]);
			close(fd2ChildHL[1]);
			close(fd2ChildLH[1]);
			close(fd2ChildLL[1]);

			dup2(fd1ChildHH[0], STDIN_FILENO);
			close(fd1ChildHH[1]);

			close(fd1ChildHH[0]);
			close(fd2ChildHH[0]);

			if (dup2(fd2ChildHH[1], STDOUT_FILENO) == -1) {
				ERROR_EXIT("Error on dup2");
			}

			close(fd2ChildHH[1]);

			if (execlp(argv[0], argv[0], NULL) == -1) {
				ERROR_EXIT("Error on execlp");
			}
		}
		//End fork1


		pid1 = fork();
		if (pid1 < 0) {
			ERROR_EXIT("Error at forking");
		}
		else if (pid1 == 0) {
			close(fd1ChildHH[0]);
			close(fd1ChildLH[0]);
			close(fd1ChildLL[0]);
			close(fd2ChildHH[0]);
			close(fd2ChildLH[0]);
			close(fd2ChildLL[0]);
			close(fd1ChildHH[1]);
			close(fd1ChildLH[1]);
			close(fd1ChildLL[1]);
			close(fd2ChildHH[1]);
			close(fd2ChildLH[1]);
			close(fd2ChildLL[1]);

			dup2(fd1ChildHL[0], STDIN_FILENO);
			close(fd1ChildHL[1]);

			close(fd1ChildHL[0]);
			close(fd2ChildHL[0]);


			if (dup2(fd2ChildHL[1], STDOUT_FILENO) == -1) {
				ERROR_EXIT("Error on dup2");
			}

			close(fd2ChildHL[1]);

			if (execlp(argv[0], argv[0], NULL) == -1) {
				ERROR_EXIT("Error on execlp");
			}

		}

		//END FORK 2

		pid2 = fork();
		if (pid2 < 0) {
			ERROR_EXIT("Error on fork");
		}
		else if (pid2 == 0) { 
			close(fd1ChildHL[0]);
			close(fd1ChildHH[0]);
			close(fd1ChildLL[0]);
			close(fd2ChildHL[0]);
			close(fd2ChildHH[0]);
			close(fd2ChildLL[0]);
			close(fd1ChildHL[1]);
			close(fd1ChildHH[1]);
			close(fd1ChildLL[1]);
			close(fd2ChildHL[1]);
			close(fd2ChildHH[1]);
			close(fd2ChildLL[1]);

			dup2(fd1ChildLH[0], STDIN_FILENO);
			close(fd1ChildLH[1]);

			close(fd1ChildLH[0]);
			close(fd2ChildLH[0]);

			if (dup2(fd2ChildLH[1], STDOUT_FILENO) == -1) {
				ERROR_EXIT("Error on dup2");
			}

			close(fd2ChildLH[1]);

			if (execlp(argv[0], argv[0], NULL) == -1) {
				ERROR_EXIT("Error on execlp");
			}

		}

		//END FORK 3

		pid3 = fork();
		if (pid3 < 0) {
			ERROR_EXIT("Error on forking");
		}
		else if (pid3 == 0) { 

			close(fd1ChildHL[0]);
			close(fd1ChildLH[0]);
			close(fd1ChildHH[0]);
			close(fd2ChildHL[0]);
			close(fd2ChildLH[0]);
			close(fd2ChildHH[0]);
			close(fd1ChildHL[1]);
			close(fd1ChildLH[1]);
			close(fd1ChildHH[1]);
			close(fd2ChildHL[1]);
			close(fd2ChildLH[1]);
			close(fd2ChildHH[1]);

			dup2(fd1ChildLL[0], STDIN_FILENO);
			close(fd1ChildLL[1]);

			close(fd1ChildLL[0]);
			close(fd2ChildLL[0]);

			if (dup2(fd2ChildLL[1], STDOUT_FILENO) == -1) {
				ERROR_EXIT("Error on dup2");
			}

			close(fd2ChildLL[1]);

			if (execlp(argv[0], argv[0], NULL) == -1) {
				ERROR_EXIT("Error on execlp");
			}
		}
		//END FORK 4
	}

	{

		// close all reading ends of writing pipe
		close(fd1ChildHH[0]);
		close(fd1ChildHL[0]);
		close(fd1ChildLH[0]);
		close(fd1ChildLL[0]);

		//writing

		write(fd1ChildHH[1], Ah, strlen(Ah));
		write(fd1ChildHH[1], Bh, strlen(Bh));
		close(fd1ChildHH[1]);

		write(fd1ChildHL[1], Ah, strlen(Ah));
		write(fd1ChildHL[1], Bl, strlen(Bl));
		close(fd1ChildHL[1]);

		write(fd1ChildLH[1], Al, strlen(Al));
		write(fd1ChildLH[1], Bh, strlen(Bh));
		close(fd1ChildLH[1]);

		write(fd1ChildLL[1], Al, strlen(Al));
		write(fd1ChildLL[1], Bl, strlen(Bl));
		close(fd1ChildLL[1]);

		// Wait for child
		int status0, status1, status2, status3;
		waitpid(pid0, &status0, 0);
		waitpid(pid1, &status1, 0);
		waitpid(pid2, &status2, 0);
		waitpid(pid3, &status3, 0);

		if (WEXITSTATUS(status0) == 1 || WEXITSTATUS(status1) == 1 || WEXITSTATUS(status2) == 1 || WEXITSTATUS(status3) == 1) {
			ERROR_EXIT("Error in the childprocess");
			exit(EXIT_FAILURE);
		}

		close(fd2ChildHH[1]);
		close(fd2ChildHL[1]);
		close(fd2ChildLH[1]);
		close(fd2ChildLL[1]);
	}

	// Read string from child and close reading end.
	char returnChildHH[2 * length + length * 2 + 2];
	char returnChildHL[2 * length + length + 2];
	char returnChildLH[2 * length + length + 2];
	char returnChildLL[2 * length + 2];

	{
		int rv;

		rv = read(fd2ChildHH[0], returnChildHH, length *2);
		returnChildHH[rv] = '\0';
		close(fd2ChildHH[0]);

		rv = read(fd2ChildHL[0], returnChildHL, length *2);
		returnChildHL[rv] = '\0';
		close(fd2ChildHL[0]);

		rv = read(fd2ChildLH[0], returnChildLH, length *2);
		returnChildLH[rv] = '\0';
		close(fd2ChildLH[0]);

		rv = read(fd2ChildLL[0], returnChildLL, length *2);
		returnChildLL[rv] = '\0';
		close(fd2ChildLL[0]);
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