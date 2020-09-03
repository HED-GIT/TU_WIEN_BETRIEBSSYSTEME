#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

typedef struct Settings{		// struct that holds all settings of the program
	unsigned int s : 1;			// the :1 makes it a bit-field, it only tells the compiler that at most one bit is used
	unsigned int i : 1;			// it can make optimization based on that knowledge
	FILE * output;				// gcc should also throw a warning if you try to assign a value greater then 1 to a
}Settings;

char * name = "ispalindrom";	// holds name of the program, is hardcoded since argv[0] doesn't have to hold the correct name
								// for example it could be set wrong when the program is executed using exec (man exec)
								// but that also means that it doesn't change when you rename the program
								// under linux you could figure out the correct name using 'readlink("/proc/self/exe", char * buf, size_t bufsiz);' but i can't be bothered to do it like this

// converts the word to a lowercase version of itself
void string_to_lower(char * word){
	for(; *word!='\0'; word++){
		*word = tolower(*word);
	}
}

// remove spaces from a word
void remove_space(char * word){
	for(char* reader = word; *reader != '\0'; ++reader){
		if(*reader!=' '){
			*word = *reader;
			++word;
		}
	}
	*word = '\0';
}

// checks if the word is a palindrom
int is_palindrom(char* word){	
	char* last = word+strlen(word)-1;
	while(last >= word){
		if(*last != *word)
			return 0;
		
		--last;
		++word;
	}
	return 1;
}

// handles one individuel line
void string_handler(const char* word, Settings * set){
	char * checkWord = strdup(word);	// copy string so that the original string can be printed

	if(checkWord == NULL){
		ERROR_EXIT("error allocating memory");
	}
	
	if(set->s)
		remove_space(checkWord);
	if(set->i)
		string_to_lower(checkWord);
	

	if(is_palindrom(checkWord))
		fprintf(set->output, "%s is a palindrom\n", word);
	else
		fprintf(set->output, "%s is not a palindrom\n", word);
		
	free(checkWord);
}

// reads file line for line and checks for palindrom
void handle_file(FILE * input, Settings * set){
	
	char * line = NULL;
	size_t size = 0;
	while(getline(&line, &size, input) != -1){
		if(line[ strlen(line) - 1] == '\n'){		//strip \n in case it got read by getline
			line[ strlen(line) - 1] = '\0';
		}
		string_handler(line, set);
	}

	free(line);
	if(!feof(input)){
		ERROR_MSG("Some error happened while reading a file");	//print message and continue
	}
}


// reads commandline arguments
// returns position of first input-file
int handle_arguments(Settings * set, int argc, char ** argv){
	int opt;
	while((opt=getopt(argc, argv, "sio:"))!=-1){
		switch(opt){
			case 's':
				set->s = 1;
				break;
			case 'i':
				set->i = 1;
				break;
			case 'o':
				if(set->output != stdout){
					fclose(set->output);
				}
				set->output = fopen(optarg, "w");	// should be done after getopt since only last -o is needed to be opened
				break;
			default:
				USAGE();
		}
	}

	if(set->output == NULL){		// done here cause only last call of fopen is important
		ERROR_EXIT("couldn't open output-file");
	}
	return optind;
}

int main(int argc,char ** argv){
		
	Settings set = {0,0,stdout};
	
	int i = handle_arguments(&set, argc, argv);

	if(i >= argc){												//no additional arguments, read from stdin
		handle_file(stdin, &set);
	}
	else{
		for(;i<argc;i++){

			FILE * input = fopen(argv[i], "r");
			if(input == NULL){
				ERROR_MSG("couldn't open file %s", argv[i]);		//give warning, ignore file and continue
				continue;
			}
			handle_file(input, &set);
			fclose(input);
		}
	}
	exit(EXIT_SUCCESS);
}
