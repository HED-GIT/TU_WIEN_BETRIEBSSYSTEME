#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

// macro used to print usage
// do while is not strictly needed but prevents errors when using the macro in some specific situations (but not in this program)
#define USAGE()																	\
	do{																			\
		fprintf(stdout,"USAGE: %s [-i] [-o outfile] keyword [file...]", name);		\
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
								// the :1 makes the int a bit-field, it only tells the compiler that at most one bit is used
	unsigned int i : 1;			// it can make optimization based on that knowledge
	FILE * output;				// gcc should also throw a warning if you try to assign a value greater then 1 to it
}Settings;

static char * name = "mygrep";			// holds name of the program, is hardcoded since argv[0] doesn't have to hold the correct name
								// for example it could be set wrong when the program is executed using exec (man exec)
								// but that also means that it doesn't change when you rename the program
								// under linux you could figure out the correct name using 'readlink("/proc/self/exe", char * buf, size_t bufsiz);' but i can't be bothered to do it like this


typedef struct{
	int i;
	int o;
}Flags;

static void string_to_lower(char * word){	
	for(; *word!='\0';++word){
		*word = tolower(*word);
	}
}

static void handle_file(FILE * input, Settings * set, char * key){
	char  * read = NULL;
	size_t size = 0;

	if(set->i){
		string_to_lower(key);
	}

	while(getline(&read, &size, input) != -1){
		
		if(read[strlen(read)-1]=='\n'){
			read[strlen(read)-1]='\0';
		}
	
		char * checkWord = strdup(read);
		if(checkWord == NULL){
			ERROR_EXIT("couldn't allocate memory");
		}
		
		if(set->i){
			string_to_lower(checkWord);
		}

		if(strstr(checkWord, key)!=NULL){
			fprintf(set->output, "%s\n", read);
		}
		free(checkWord);
	}
	if(!feof(input)){
		ERROR_EXIT("couldn't read file to end");
	}
}

int main(int argc, char ** argv){

	Settings set = {0,stdout};
	int opt;
	
	while((opt=getopt(argc, argv, "io:"))!=-1){
		switch(opt){
			case 'i':
				set.i = 1;
				break;
			case 'o':
				if(set.output != stdout)
					fclose(set.output);
				set.output = fopen(optarg, "w");
				break;
			default:
				USAGE();
		}
	}
	
	if(set.output == NULL){
		ERROR_EXIT("couldn't open output file");
	}

	if(optind >= argc){
		USAGE();
	}	
	int i = optind;
	
	char * key = argv[i];
	
	i++;
	
	if(i>=argc){
		handle_file(stdin, &set, key);
	}
	else{
		for(;i<argc;i++){

			FILE * input = fopen(argv[i], "r");

			handle_file(input, &set, key);
			fclose(input);
			
		}
	}
	exit(EXIT_SUCCESS);
}
