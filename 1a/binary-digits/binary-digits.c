#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

// macro used to print usage
// do while is not strictly needed but prevents errors when using the macro in some specific situations (but not in this program)
#define USAGE()																	\
	do{																			\
		fprintf(stdout,"USAGE: %s [-d DELAY] [-o OUTPUTFILE] [FILE]...", name);		\
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

static char * name = "binary-digits";	// holds name of the program, is hardcoded since argv[0] doesn't have to hold the correct name
								// for example it could be set wrong when the program is executed using exec (man exec)
								// but that also means that it doesn't change when you rename the program
								// under linux you could figure out the correct name using 'readlink("/proc/self/exe", char * buf, size_t bufsiz);' but i can't be bothered to do it like this

typedef struct Settings{		// struct that holds all settings of the program
	float delay;
	FILE * output;
}Settings;

static int handle_arguments(int argc, char ** argv, Settings *set){
    int opt;
	while((opt=getopt(argc, argv, "d:o:"))!=-1){
        char* end = NULL;

		switch(opt){
			case 'd':
                set->delay = strtod(optarg,&end);
                if(optarg + strlen(optarg) != end){
                    ERROR_EXIT("d value is not a valid double");
                }
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
    if(set->output == NULL){
        ERROR_EXIT("couldn't open output file");
    }
    return optind;
}

static void handle_file(FILE * input, Settings * set){
    int character;
    while((character = getc(input)) != EOF){
        for(int i = 0; i < 8; i++){
            if(character & 128){        //is leftmost bit set to 1
                fprintf(set->output,"1");
            }
            else{
                fprintf(set->output,"0");
            }
            fflush(set->output);
            character <<= 1;

            struct timespec tim1;
            tim1.tv_sec = set->delay;
            tim1.tv_nsec = (long)(set->delay * 1000000000L)%1000000000L;        
            int res;
            do {
                res = nanosleep(&tim1, &tim1);
            } while (res && errno == EINTR);
            if(res == -1){
                ERROR_MSG("couldn't wait for given time");
            }
        }
    }
}

int main(int argc, char ** argv){
    Settings set = {0,stdout};
    int i = handle_arguments(argc, argv, &set);

    if(i == argc){
        handle_file(stdin,&set);
    }
    for(; i < argc; i++){
        FILE * input = fopen(argv[i], "r");
        if(input == NULL){
            ERROR_MSG("couldn't open file %s", argv[i]);
            continue;
        }
        handle_file(input,&set);
        fclose(input);
    }

   	exit(EXIT_SUCCESS);
}