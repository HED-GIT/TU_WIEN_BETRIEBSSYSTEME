#include <stdio.h>

char * fileName;
#define CHARLENGTH 1024
#define ERROR_EXIT(...) { fprintf(stderr, "%s ERROR: " __VA_ARGS__"\n",fileName); exit(EXIT_FAILURE); }
#define ERROR_MSG(...) { fprintf(stderr, "%s ERROR: " __VA_ARGS__"\n",fileName); exit(EXIT_FAILURE); }
#define SUCCESS_EXIT() {fclose(stdout); fprintf(stderr, "%s Success\n"); exit(EXIT_SUCCESS);}

int isValidPort(char *);
void goToEndOfHeader(FILE*);
void writeFileToFile(FILE *,FILE * );