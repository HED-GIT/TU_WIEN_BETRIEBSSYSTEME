#include "sharedTools.h"
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>

/**
@brief checks if the given port is a valid one
@details  checks if it is not NULL and only contains digits

@param port the port to check
*/
int isValidPort(char * port) {
	return (*port == '\0')?1:(!isdigit(*port))?0:isValidPort(++port);
}

/**
@brief reads through a file till it encounters the end of the header

@param sockfile the file to  search through

*/
void goToEndOfHeader(FILE* sockfile) {
	char * buf = malloc(sizeof(char)*CHARLENGTH);
	
	while (fgets(buf, sizeof(buf), sockfile) != NULL) {
		if (strcmp(buf, "\r\n") == 0) {
			break;
		}
	}
	free(buf);
}

/**
@brief writes the content of one File to another
@details writes from the current position till eof

@param inFile the File read from
@param outFile the File written to

*/
void writeFileToFile(FILE * inFile, FILE * outFile) {
	uint8_t * binary_buffer = malloc(sizeof(uint8_t)*CHARLENGTH);
	
	while (!feof(inFile)) {
		ssize_t n = fread(binary_buffer, sizeof(uint8_t), CHARLENGTH, inFile);
		fwrite(binary_buffer, sizeof(uint8_t), n, outFile);
	}
	
	free(binary_buffer);
}