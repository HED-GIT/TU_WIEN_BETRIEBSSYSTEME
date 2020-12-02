#include <stdio.h>

char * fileName;
#define CHARLENGTH 1024

#define ERROR_EXIT(...)                                             \
    do{                                                             \
        fprintf(stderr, "%s ERROR: " __VA_ARGS__"\n",fileName);     \
        exit(EXIT_FAILURE);                                         \
    }while(0)

#define ERROR_MSG(...)                                              \
    do{                                                             \
        fprintf(stderr, "%s ERROR: " __VA_ARGS__"\n",fileName);     \
        exit(EXIT_FAILURE);                                         \
    }while(0)

#define SUCCESS_EXIT()                      \
    do{                                     \
        fclose(stdout);                     \
        fprintf(stderr, "%s Success\n");    \
        exit(EXIT_SUCCESS);                 \
    }while(0)

int isValidPort(char *);
void goToEndOfHeader(FILE*);
void writeFileToFile(FILE *,FILE * );