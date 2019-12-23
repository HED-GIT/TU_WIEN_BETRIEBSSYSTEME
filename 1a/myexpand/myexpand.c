#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define USAGE() {fprintf(stderr,"%s\t[USAGE]\t %s [-t tabstop] [file...]\n",fileName,fileName);exit(EXIT_FAILURE);}
#define ERROR_EXIT(...) { fprintf(stderr, "%s\t[ERROR] " __VA_ARGS__"\n",fileName); exit(EXIT_FAILURE); }
#define SUCCESS_EXIT() {exit(EXIT_SUCCESS);}

int tabstop = 8;
char * fileName;

void readFile (FILE * file){
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
}

int main(int argc, char ** argv){
	fileName = argv[0];
	
	int arguments = 0;
	
	int opt;
	while((opt=getopt(argc,argv,"t:"))!=-1){
		switch(opt){
			case 't':
				arguments++;
				tabstop = (int) strtol(optarg, (char **)NULL, 10);
				break;
			default:
				USAGE();
		}
	}
	
	if(tabstop <= 0){
		ERROR_EXIT("value of -t is invalid");
	}
	if(arguments > 1){
		USAGE();
	}
	
	if(arguments * 2 + 1 == argc){
		readFile(stdin);
		SUCCESS_EXIT();
	} else{
		for(int i = arguments * 2 + 1; i < argc; i++){
			FILE * input = fopen(argv[i],"r");
			if(input == NULL){
				ERROR_EXIT("unable to open file");
			}
			readFile(input);
			fclose(input);
		}
		SUCCESS_EXIT();
	}
}