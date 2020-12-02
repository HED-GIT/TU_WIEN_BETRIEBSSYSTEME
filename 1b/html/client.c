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

#include "sharedTools.h"


#define USAGE() 																	\
	do{																				\
		fprintf(stderr,"USAGE:\t./client [-p PORT] [ -o FILE | -d DIR ] URL\n"); 	\
		exit(EXIT_FAILURE);															\
	} while(0)

#define DEFAULTFILE "index.html"

typedef struct FLAG_S{
	int p_flag : 1;
	int o_flag : 1;
	int d_flag : 1;
} FLAG_S;

/**
@brief stores most important values of an URL
@param argc argument count
*/
typedef struct URL_S {
	char *url;
	char *file;
} URL_S;

static URL_S * URL(char * fullUrl) {
	
	URL_S * this = malloc(sizeof(URL_S));

	if(strncmp(fullUrl,"http://",7)!=0){
		fprintf(stderr,"invalid url");
		exit(EXIT_FAILURE);
	}
	
	char * starturl = &fullUrl[7];
	char * endurl;
	
	if((endurl = strpbrk(starturl,";/?:@=&"))==NULL){
		this->url = strdup(starturl);
		this->file = strdup(DEFAULTFILE);
	}else{
		this->url = strndup(starturl,endurl-starturl);
		this->file = (*(++endurl) != '\0')?strdup(endurl):strdup(DEFAULTFILE);
	}
	return this;
}

static void free_URL(URL_S * this){
	free(this->url);
	free(this->file);
	//free(this);
	
}

/**
@brief reads the header and checks if it is valid
@details "cursor" is at the next headerline afterwards

@param sockfile file to read the header from
*/
static void checkResponseHeader(FILE * sockfile) {
	char *buf = malloc(sizeof(char)*CHARLENGTH);
	int responseValue;
	fgets(buf, sizeof(char)*CHARLENGTH, sockfile);
	int x = sscanf(buf, "HTTP/1.1 %d", &responseValue);
	if (x != 1) {
		fprintf(stderr,"%s\n", buf);
		fprintf(stderr, "%s ERROR: invalid header\n", fileName);
		exit(EXIT_FAILURE);
	}

	if (responseValue != 200) {
		fprintf(stderr, "%s ERROR: %s\n", fileName, &buf[9]);
		exit(EXIT_FAILURE);
	}
	free(buf);
}

/**
@brief prints the header to a file

@param sockfile file to write to
@param url full url the header goes to
*/
static void printHeader(FILE * sockfile,URL_S * url) {
	char header[2092] = { 0 };
	sprintf(header, "GET /%s HTTP/1.1\r\nHost: "
		"%s\r\n"
		"Connection: close\r\n\r\n", url->file, url->url);

	if (fputs(header, sockfile) == EOF)
		ERROR_EXIT("fputs");

	if (fflush(sockfile) == EOF)
		ERROR_EXIT("fflush");
}

/**
@brief main class of the program
@details reads arguments, setup for socket
@param argc argument count
@argv array of argument strings

@return 1 on success or an other error if failed

*/
int main(int argc, char *argv[]) {
	fileName = argv[0];
	FLAG_S flags = {0,0,0};
	FILE * out = stdout;
	char* p_port = "80";
	char *d_dir= malloc(sizeof(char)*CHARLENGTH);



	int c;
	while ((c= getopt(argc, argv, "p:o:d:")) != -1) {
		switch (c) {
		case 'p':
			if (flags.p_flag) { USAGE(); }
				 flags.p_flag++; 
				 p_port = strdup(optarg);
				 if (isValidPort(p_port) == 0) {
					 ERROR_EXIT("INVALID PORT");
				 }
				 break;
		case 'o':
			if (flags.o_flag || flags.d_flag) { USAGE(); }
				flags.o_flag++; 
				d_dir = strdup(optarg);
				break;
		case 'd':
			if(flags.o_flag || flags.d_flag) { USAGE(); }
				flags.d_flag++; 
				d_dir = strdup(optarg);
				break;
		default:
			USAGE();
		}
	}
	
	
	char * fullUrl;
	if ((fullUrl=argv[optind]) == NULL) {
		USAGE();
	}
	URL_S *url = URL(fullUrl);


	struct addrinfo hints, *ai;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // create Internet Protocol (IP) socket
	hints.ai_socktype = SOCK_STREAM; // use TCP as transport protocol

	if (getaddrinfo(url->url, p_port, &hints, &ai) != 0) {ERROR_EXIT("getaddrinfo");}

	int sockfd;
	if ((sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) < 0) { ERROR_EXIT("socket"); }

	if ((connect(sockfd, ai->ai_addr, ai->ai_addrlen)) < 0)
		ERROR_EXIT("connection");

	FILE *sockfile = fdopen(sockfd, "r+");
	if (sockfile == NULL)
		ERROR_EXIT("fdopen");


	printHeader(sockfile,url);
	checkResponseHeader(sockfile);


	
	if (flags.o_flag || flags.d_flag) {
		char * pathOutFile;
		if (flags.d_flag) {
			pathOutFile = malloc((strlen(d_dir) + 2 + strlen(url->file)) * sizeof(char));
			strcpy(pathOutFile,d_dir);
			strcat(pathOutFile, "/");
			strcat(pathOutFile, url->file);
		} else{
			pathOutFile = malloc((strlen(d_dir) + 1) * sizeof(char));
			strcpy(pathOutFile,d_dir);
		}
		out = fopen(pathOutFile, "w");
		free(pathOutFile);
		free(d_dir);
		if (out == NULL) {ERROR_EXIT("Could not open File"); }
	}
	
	
	goToEndOfHeader(sockfile);
	writeFileToFile(sockfile,out);
	fflush(out);
	fclose(out);
	free_URL(url);
	fclose(sockfile);
	return 0;
}
