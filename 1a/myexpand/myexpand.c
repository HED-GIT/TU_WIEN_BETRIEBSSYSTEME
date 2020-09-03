#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// macro used to print usage
// do while is not strictly needed but prevents errors when using the macro in some specific situations (but not in this program)
#define USAGE()																	\
	do{																			\
		fprintf(stdout,"USAGE: %s [-s] [-i] [-o outfile] [file...]", name);		\
		exit(EXIT_FAILURE);														\
	} while(0)


// macro used to print errors, program still continues on
// the ... let's you give arguments to the macro which will automaticly be written at the position of __VA_ARGS__
#define ERROR_MSG(...)											\
	do{ 														\
		fprintf(stderr, "%s ERROR:\t", name); 					\
		fprintf(stderr, __VA_ARGS__); 							\
		fprintf(stderr, "\n");									\
	} while(0)


// macro used to exit the program and automaticly print an error message
#define ERROR_EXIT(...)											\
	do{ 														\
		ERROR_MSG(__VA_ARGS__);									\
		exit(EXIT_FAILURE); 									\
	} while(0)


char * name = "myexpand";	// holds name of the program, is hardcoded since argv[0] doesn't have to hold the correct name
							// for example it could be set wrong when the program is executed using exec (man exec)
							// but that also means that it doesn't change when you rename the program
							// under linux you could figure out the correct name using 'readlink("/proc/self/exe", char * buf, size_t bufsiz);' but i can't be bothered to do it like this


void readFile(FILE * file, int tabstop){
	char readChar;
	int writePos = 0;
	while((readChar = fgetc(file))!=EOF){
		if(readChar == '\t'){
			int p = tabstop * ((writePos/tabstop)+1);
			for(; writePos<p;writePos++){
				fprintf(stdout,"%c",' ');
			}
			continue;
		} 
	
		writePos++;
		fprintf(stdout,"%c",readChar);
		if(readChar == '\n'){
			writePos=0;
		}
	}
	if(!feof(file)){
		ERROR_EXIT("couldn't read file to end");
	}
}

int main(int argc, char ** argv){
	int tabstop = 8;
	int opt;
	while((opt=getopt(argc,argv,"t:"))!=-1){
		switch(opt){
			case 't':
				tabstop = (int) strtol(optarg, (char **)NULL, 10);
				break;
			default:
				USAGE();
		}
	}
	
	if(tabstop <= 0){
		ERROR_EXIT("value of -t is invalid");
	}

	if(optind == argc){
		readFile(stdin, tabstop);
		exit(EXIT_SUCCESS);
	} else{
		for(int i = optind; i < argc; i++){
			FILE * input = fopen(argv[i], "r");
			if(input == NULL){
				ERROR_EXIT("unable to open file");
			}
			readFile(input, tabstop);
			fclose(input);
		}
		exit(EXIT_SUCCESS);
	}
}