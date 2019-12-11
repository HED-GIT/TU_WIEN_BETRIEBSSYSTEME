#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct{
	int full;
	int compressed;
}Counter;

Counter writer(char new, FILE * out){
	static char character=EOF;
	static int amount = 0;
	
	static Counter counter = {0,0};
	
	counter.full++;

	if(new == EOF){
		fprintf(out,"%c%d",character,amount);
		character = EOF;
		amount = 0;
		counter.compressed++;
	}
	else if(new == character){
		amount++;
	} 
	else{
		if(character != EOF)
			fprintf(out,"%c%d",character,amount);
		character = new;
		amount = 1;
		counter.compressed++;
	}
	return counter;
	
}

Counter fileHandler(FILE * outFile, FILE * inFile){
	char next;

	while((next = fgetc(inFile))!= EOF){
		writer(next,outFile);
	}
	return writer(EOF,outFile);
}

int main(int argc, char ** argv){
	int o_flag=0;
	int opt;
	
	FILE * outFile = stdout;
	
	while((opt=getopt(argc,argv,"o:"))!=-1){
		switch(opt){
			case 'o':
				o_flag++;
				outFile = fopen(optarg,"w");
				break;
			default:
				exit(EXIT_FAILURE);
		}
	}

	Counter counter;

	o_flag *= 2;
	if(o_flag+1==argc){
		counter=fileHandler(outFile,stdin);
	} 
	else{
		o_flag++;
		for(;o_flag<argc;o_flag++){

			FILE * input = fopen(argv[o_flag],"r");

			counter=fileHandler(outFile,input);
			fclose(input);
			
		}
	}
	fprintf(stdout,"full file size: %d\ncompressed file size: %d\n",counter.full,counter.compressed);	
	return 0;
}
