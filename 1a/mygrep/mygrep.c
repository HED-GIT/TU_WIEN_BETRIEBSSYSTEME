#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#define MAXLENGTH 10000

typedef struct{
	int i;
	int o;
}Flags;

void toLower(char * word){	
	for(; *word!='\0';++word){
		*word = tolower(*word);
	}
}

void handleFile(FILE * input, FILE * output, Flags * flags, char * key){
	char read[MAXLENGTH];
	char * checkWord = malloc(sizeof(char)*MAXLENGTH);
	while(fgets(read,sizeof(read),input)!=NULL){
		
		if(read[strlen(read)-1]=='\n'){
			read[strlen(read)-1]='\0';
		}
	
		strcpy(checkWord,read);
		
		if(flags->i){
			toLower(checkWord);
			toLower(key);
		}

		if(strstr(checkWord,key)!=NULL){
			fprintf(output,"%s\n",read);
		}

	}
	free(checkWord);

}

int main(int argc, char ** argv){
	FILE * output = stdout;
	Flags flags = {0,0};
	int counter=0;
	int opt;
	
	while((opt=getopt(argc,argv,"io:"))!=-1){
		switch(opt){
			case 'i':
				flags.i++;
				counter++;
				break;
			case 'o':
				counter+=2;
				flags.o++;
				output = fopen(optarg,"w");
				break;
			default:
				exit(EXIT_FAILURE);
		}
	}
	
	if(counter+1>=argc){
		fprintf(stderr,"Input invalid %d %d\n",counter,argc);
		exit(EXIT_FAILURE);
	}	
	int i=flags.i+flags.o*2+1;
	
	char * key = argv[i];
	
	i++;
	
	if(i>=argc){
		handleFile(stdin,output,&flags,key);
	}
	else{
		for(;i<argc;i++){

			FILE * input = fopen(argv[i],"r");

			handleFile(input,output,&flags, key);
			fclose(input);
			
		}
	}
	return 0;
}
