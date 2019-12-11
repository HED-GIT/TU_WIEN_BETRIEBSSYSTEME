#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define MAXLENGTH 10000

typedef struct{
	int i;
	int o;
}Flags;

typedef struct{
	FILE * in1;
	FILE * in2;
	FILE * out;
}FILES;

int charToCompare(int character, Flags* flags){
	if(flags->i){
		return tolower(character);
	}
	else{
		return character;
	}
}

int handler(char *text1, char *text2, Flags* flags){
	static int row = 0;
	static int mistakes = 0;
	if(*text1 == '\n' || *text2 == '\n' || *text1 == '\0' || *text2 == '\0'){

		int returnint = mistakes;
		row++;
		mistakes = 0;
		return returnint;
	}
	else if(charToCompare(*text1,flags)==charToCompare(*text2,flags)){

		return handler(++text1,++text2,flags);

	}
	else{
		mistakes++;
		return handler(++text1,++text2,flags);
	}
}

void argumentHandler(Flags * flags, FILES * file, int argc, char ** argv){
	int opt;
	int counter=0;
	while((opt = getopt(argc, argv, "io:"))!=-1){
		switch(opt){
			case 'i':
			counter++;
			flags->i++;
			break;
			case 'o':
			counter+=2;
			flags->o++;
			file->out = fopen(optarg, "w");
			if(file->out==NULL){	
				fprintf(stderr,"error at opening output file");
				exit(EXIT_FAILURE);
			}
			break;
			default:
			fprintf(stderr,"error at reading argument");
			exit(EXIT_FAILURE);
		}
	}
	if(counter+3!=argc){
		fprintf(stderr,"error at reading arguments %d %d", argc, counter+3);
		exit(EXIT_FAILURE);
	}
	
	file->in1 = fopen(argv[counter+1],"r");
	file->in2 = fopen(argv[counter+2],"r");
	if(file->in1 == NULL||file->in2==NULL){
		fprintf(stderr,"error at opening file");
		exit(EXIT_FAILURE);
	}
}

void fileHandler(FILES* file, Flags* flags){
	char * read1 = malloc(sizeof(char)*MAXLENGTH);
	char * read2 = malloc(sizeof(char)*MAXLENGTH);
	int row =0;
	while((fgets(read1,sizeof(char)*MAXLENGTH,file->in1)!=NULL) 
		&& (fgets(read2,sizeof(char)*MAXLENGTH,file->in2)!=NULL)){
		int mistakes = handler(read1,read2,flags);
		if(mistakes)
			fprintf(file->out,"Line: %d Character: %d\n",row, mistakes);
		row++;
	}

	free(read1);
	free(read2);
	
}

int main(int argc, char ** argv){
	Flags flags = {0,0};

	FILES file = {NULL,NULL,stdout};
	
	argumentHandler(&flags,&file,argc,argv);
	fileHandler(&file,&flags);
	
	return 0;
}