#include <stdio.h>

extern char * fileName;

#define CHARLENGTH 1024

#define ERROR_EXIT(...)                                                 \
    do                                                                  \
    {                                                                   \
        ERROR_MSG(__VA_ARGS__);                                         \
        exit(EXIT_FAILURE);                                             \
    } while (0)

#define ERROR_MSG(...)                                                  \
    do                                                                  \
    {                                                                   \
        fprintf(stderr, "%s ERROR: ", fileName);                        \
        fprintf(stderr, __VA_ARGS__);                                   \
        fprintf(stderr, "\n");                                          \
    } while (0)

#define SUCCESS_EXIT()                                  \
    do{                                                 \
        fclose(stdout);                                 \
        fprintf(stderr, "%s Success\n", fileName);      \
        exit(EXIT_SUCCESS);                             \
    }while(0)

int isValidPort(char *);
void goToEndOfHeader(FILE*);
void writeFileToFile(FILE *,FILE * );