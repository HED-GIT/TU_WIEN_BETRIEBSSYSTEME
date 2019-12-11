#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <ctype.h>
#include <signal.h>
#include <dirent.h>
#include <time.h>

#include "../requestHandling/sharedTools.h"

#define USAGE() {fprintf(stderr,"USAGE:\t./server [-p PORT] [-i INDEX] DOC_ROOT\n");exit(EXIT_FAILURE);}

static int terminate = 0;

/**
@brief the signal-Handler cor SIGINT and SIGTERM

@param signal the signal that happened

*/
static void signalHandler(int signal) {
	terminate++;
}

/**
@brief starts the signalHandler vor SIGINT and SIGTERM
*/
static void setupSignalHandler() {
	struct sigaction sigint,sigterm;
	memset(&sigint, 0, sizeof sigint);
	memset(&sigterm, 0, sizeof sigterm);

	sigint.sa_handler = signalHandler;
	sigterm.sa_handler = signalHandler;

	if (sigaction(SIGINT, &sigint, NULL) != 0|| sigaction(SIGTERM, &sigterm, NULL) != 0) {
		ERROR_EXIT("sigaction");
	}
}

/**
@brief writes the Date to a Header

@param file file to write to
*/
static void sendDate(FILE* file) {
	char* line = malloc(sizeof(char)*100);

	//get time and timeinfo
	time_t rtime;
	struct tm* timeinfo;

	time(&rtime);
	timeinfo = gmtime(&rtime);

	strftime(line, sizeof(line), "Date: %a, %d %b %y %H:%M:%S GMT\r\n", timeinfo);
	fputs(line, file);
	
	free(line);
}

/**
@brief writes the size of a file to a Header

@param fileToPrint file to write to
@param fileToSend file that the size should come frome
*/
static void sendContentLength(FILE* fileToPrint, FILE* fileToSend) {
	fseek(fileToSend, 0L, SEEK_END);
	int filesize = ftell(fileToSend);
	rewind(fileToSend);
	fprintf(fileToPrint, "Content-Length: %d\r\n", filesize);
}

/**
@brief prints a header to a file

@param socketFile the file to write to
@param command the returnCommand that should be sent
@param requestedFile the file that will be sent after the header
*/
static void printHeader(FILE* socketFile, int command,FILE *requestedFile) {
	switch(command){
		case 501:
			break;
			fputs("HTTP/1.1 501 Not implemented\r\n", socketFile);
		case 404:
			fputs("HTTP/1.1 404 Not found\r\n", socketFile);
			break;
		case 200:
			fputs("HTTP/1.1 200 OK\r\n", socketFile);
			sendDate(socketFile);
			sendContentLength(socketFile, requestedFile);
			break;
		default:
			ERROR_EXIT("invalid return value given");
			break;
	}
	fprintf(socketFile, "Connection: close\r\n");
	fprintf(socketFile, "\r\n");
}

/**
@brief waits for and handles requests
@param socket file descriptor 
@param rootDoc the root document of the server
@param i_index the default file sent
*/
static void startRequestHandling(int sock_fd,char * rootDoc, char * i_index) {
	while (!terminate) {
		int request_fd = accept(sock_fd, NULL, NULL);

		if (request_fd < 0) {
			close(request_fd);
			break;
		}

		FILE *socketFile = fdopen(request_fd, "r+");
		if (socketFile == NULL) {
			ERROR_MSG("could not open socket");
			continue;
		}

		char buf[CHARLENGTH]; buf[0] = '\0';
		char get[CHARLENGTH]; get[0] = '\0';
		char path[CHARLENGTH]; path[0] = '\0';
		char fullPath[CHARLENGTH]; fullPath[0] = '\0';

		fgets(buf, sizeof(buf), socketFile);
		sscanf(buf, "%[^ ] /%[^ ] HTTP/1.1\r\n", get, path);
		strcat(fullPath, rootDoc);
		strcat(fullPath, path);
		if (fullPath[strlen(fullPath) - 1] == '/') {
			strcat(fullPath, i_index);
		}

		fprintf(stderr, "GET: %s PATH: %s\n", get, fullPath);

		goToEndOfHeader(socketFile);

		FILE *requestedFile;
		if (strcmp(get, "GET") != 0) {
			printHeader(socketFile, 501,NULL);
			fflush(socketFile);
			fclose(socketFile);
			close(request_fd);
			continue;
		}

		requestedFile = fopen(fullPath, "r+");
		if (requestedFile == NULL) {
			printHeader(socketFile, 404,NULL);
			fflush(socketFile);
			fclose(socketFile);
			close(request_fd);
			continue;
		}
		printHeader(socketFile,200,requestedFile);

		writeFileToFile(requestedFile,socketFile);

		fflush(socketFile);
		fclose(socketFile);
		close(request_fd);
		fprintf(stderr, "finished\n");

	}
}

/**
@brief main class of the program
@details reads arguments, setup for socket, setup signalhandler

@param argc argument count
@argv array of argument strings

@return 1 on success or an other error if failed

*/
int main(int argc, char *argv[]) {
	fileName = argv[0];
	int p=0;
	int i = 0;
	char * p_port = "8080";
	char * i_index = "index.html";
	char * rootDoc = "";

	int c;
	while ((c = getopt(argc, argv, "p:i:")) != -1) {
		switch (c) {
		case 'p':if (p) { USAGE(); }
				 p++;
				 p_port = strdup(optarg);
				 if (isValidPort(p_port) == 0) {
					 ERROR_EXIT("INVALID PORT");
				 }
				 break;
		case 'i':if (i) { USAGE(); }
				 i++;
				 i_index = strdup(optarg);
				 break;
		default:
			USAGE();
		}
	}

	if ((rootDoc = argv[optind]) == NULL) {
		USAGE();
	}
	if (-1==access(rootDoc, R_OK)) {
		ERROR_EXIT("root doc can't be read from");
	}


	struct addrinfo hints, *ai;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int res = getaddrinfo(NULL, p_port, &hints, &ai);
	if (res != 0) {
		freeaddrinfo(ai);
		ERROR_EXIT("getAddressInfo");
	}

	int sock_fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	if (sock_fd < 0) {
		freeaddrinfo(ai);
		ERROR_EXIT("socket");
	}

	//int optval = 1;
	//setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

	if (bind(sock_fd, ai->ai_addr, ai->ai_addrlen) < 0) {
		freeaddrinfo(ai);
		ERROR_EXIT("bind");
	}

	if (listen(sock_fd, 10) < 0) {
		freeaddrinfo(ai);
		ERROR_EXIT("listen");
	}

	fprintf(stderr,"http://localhost:%s ...\n", p_port);
	setupSignalHandler();

	startRequestHandling(sock_fd,rootDoc,i_index);
	freeaddrinfo(ai);

	return 0;
}