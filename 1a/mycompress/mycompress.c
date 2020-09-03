#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// macro used to print usage
// do while is not strictly needed but prevents errors when using the macro in some specific situations (but not in this program)
#define USAGE()																	\
	do{																			\
		fprintf(stdout,"USAGE: %s [-o outfile] [file...]", name);				\
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

char * name = "mycompress";	// holds name of the program, is hardcoded since argv[0] doesn't have to hold the correct name
								// for example it could be set wrong when the program is executed using exec (man exec)
								// but that also means that it doesn't change when you rename the program
								// under linux you could figure out the correct name using 'readlink("/proc/self/exe", char * buf, size_t bufsiz);' but i can't be bothered to do it like this

/**
 * handles one file
 * nowhere is defined if the output (how many char where read/written) should be printed for all input files once or for every single one seperately
 * i print it once per file
 */ 
void fileHandler(FILE * outFile, FILE * inFile){
	int next;
	int current = EOF;
	int amount = 0;
	int total = 0;
	int compressed_total = 0;
	while(1){
		next = fgetc(inFile);
		if(current == EOF){								// current can only equal EOF at start of program, used to init current
			current = next;
			if(next == EOF){							//without this it would lead to an infinity loop if eof was returned on first iteration
				if(!feof(inFile)){
					ERROR_MSG("error while reading file, EOF not reached");
				}
				break;
			}
			fprintf(outFile,"%c",(char)current);		// the reason why it is printed here for the first time instead of printing it in the (next != current) if is because i have to print the char befor i know how many there are, else the formatting when reading from stdin and writing to stdout is shit
		}
		if(next != current){
			fprintf(outFile,"%i",amount);
			current = next;
			if(next != EOF)								// without this EOF would be printed to file, EOF would be interpreted as \0
				fprintf(outFile,"%c",next);
			fflush(outFile);							// without this the endmessage may be printed befor the last amount
			amount = 0;
			compressed_total+=2;
			if(next == EOF){
				if(!feof(inFile)){						// checks if EOF was reached or another error occured
					ERROR_MSG("error while reading file, EOF not reached");
				}
				break;
			}
		}			
		total++;
		amount++;
	}
	fprintf(stderr,"\n\n");
	fprintf(stderr,"Read:\t\t\t%i characters\n", total);
	fprintf(stderr,"Written:\t\t%i characters\n", compressed_total);
	fprintf(stderr,"Compression ratio:\t%.1f %%\n", ((double)compressed_total) / ((double)total) * 100);

}

int main(int argc, char ** argv){
	int opt;
	
	FILE * outFile = stdout;
	
	while((opt=getopt(argc,argv,"o:"))!=-1){
		switch(opt){
			case 'o':
				if(outFile != stdout)
					fclose(outFile);
				outFile = fopen(optarg,"w");		// would most likely be better to open file after getopt since only last call will be used
				break;
			default:
				USAGE();
		}
	}

	if(outFile == NULL){	// this is done here because only the last fopen is important, else the program would throw an error on a file that isn't used anyway
		ERROR_EXIT("couldn't open output file");
	}

	if(optind==argc){
		fileHandler(outFile,stdin);
	} 
	else{
		for(int i = optind;i<argc;i++){

			FILE * input = fopen(argv[i],"r");
			if(input == NULL){
				ERROR_MSG("file %s could not be opened",argv[i]);
				continue;			//continue even on failure of one file
			}

			fileHandler(outFile,input);
			fclose(input);
			
		}
	}
	exit(EXIT_SUCCESS);
}
