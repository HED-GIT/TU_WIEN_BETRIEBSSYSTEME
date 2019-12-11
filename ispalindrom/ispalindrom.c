#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXLENGTH 10000

typedef struct{
	int s;
	int i;
	int o;
}Flags;

void toLower(char * word){
	for(; *word!='\0';++word){
		*word = tolower(*word);
	}
}

void removeSpace(char * word){
	for(char* reader = word; *reader != '\0'; ++reader){
		if(*reader!=' '){
			*word = *reader;
			++word;
		}
	}
	*word = '\0';
}

int ispalindrom(char* word){	
	char* last = word+strlen(word)-1;
	while(last >= word){
		if(*last != *word)
			return 0;
		
		--last;
		++word;
	}
	return 1;
	
}

void stringHandler(char* word, FILE * out, Flags * flags){
	char * checkWord = malloc(sizeof(char)*MAXLENGTH);
	strcpy(checkWord,word);
	
	if(flags->s)
		removeSpace(checkWord);
	if(flags->i)
		toLower(checkWord);
	

	if(ispalindrom(checkWord)){
		fprintf(out,"%s is a palindrom\n",word);
	}
	else{
		fprintf(out,"%s is not a palindrom\n",word);
	}
	free(checkWord);


}

void handleFile(FILE * input, FILE * output, Flags * flags){
	char read[MAXLENGTH];
	while(fgets(read,sizeof(read),input)!=NULL){
		if(read[strlen(read)-1]=='\n'){

			read[strlen(read)-1]='\0';
		}
		stringHandler(read,output,flags);
	}

}

int main(int argc,char ** argv){
	FILE * outFile = stdout;
		
	Flags flags={0,0,0};
	
	int opt;
	
	while((opt=getopt(argc,argv,"sio:"))!=-1){
		switch(opt){
			case 's':
				flags.s++;
				break;
			case 'i':
				flags.i++;
				break;
			case 'o':
				flags.o++;
				outFile = fopen(optarg,"w");
				break;
			default:
				exit(EXIT_FAILURE);
		}
	}
	
	int i=flags.s+flags.i+flags.o*2+1;
	if(i>=argc){
		handleFile(stdin,outFile,&flags);
	}
	else{
		for(;i<argc;i++){

			FILE * input = fopen(argv[i],"r");

			handleFile(input,outFile,&flags);
			//fprintf(stdout,"test\n");
			fclose(input);
			
		}
	}
	return 0;
}
