#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

// macro used to print usage
// do while is not strictly needed but prevents errors when using the macro in some specific situations (but not in this program)
#define USAGE()                                                           	\
	do                                                                    	\
	{                                                                     	\
		fprintf(stdout, "USAGE: %s [-i] [-o outfile] file1 file2\n", name); \
		exit(EXIT_FAILURE);                                               	\
	} while (0)

// macro used to print errors, program still continues on
// the ... let's you give arguments to the macro which will automaticly be written at the position of __VA_ARGS__
#define ERROR_MSG(...)                        \
	do                                        \
	{                                         \
		fprintf(stderr, "%s ERROR:\t", name); \
		fprintf(stderr, __VA_ARGS__);         \
		fprintf(stderr, "\n");                \
	} while (0)

// macro used to exit the program and automaticly print an error message
#define ERROR_EXIT(...)         \
	do                          \
	{                           \
		ERROR_MSG(__VA_ARGS__); \
		exit(EXIT_FAILURE);     \
	} while (0)

static const char *name;

typedef struct Flags
{
	unsigned int i : 1;
} Flags;

typedef struct
{
	FILE *in1;
	FILE *in2;
	FILE *out;
} FILES;

static int char_to_compare(int character, Flags flags)
{
	if (flags.i)
	{
		return tolower(character);
	}
	else
	{
		return character;
	}
}

static void handle_arguments(Flags *flags, FILES *file, int argc, char **argv)
{
	int opt;
	while ((opt = getopt(argc, argv, "io:")) != -1)
	{
		switch (opt)
		{
		case 'i':
			flags->i = 1;
			break;
		case 'o':
			if (file->out != stdout)
				fclose(file->out);
			file->out = fopen(optarg, "w");
			break;
		default:
			USAGE();
		}
	}

	if (file->out == NULL)
	{
		ERROR_EXIT("error at opening output file");
	}

	if (optind + 2 != argc)
	{
		USAGE();
	}
	file->in1 = fopen(argv[optind], "r");
	file->in2 = fopen(argv[optind + 1], "r");
	if (file->in1 == NULL || file->in2 == NULL)
	{
		ERROR_EXIT("error at opening file");
	}
}

static void handle_files(FILES *file, Flags flags)
{
	int read1;
	int read2;

	int row = 1;

	int mistakes = 0;
	while((read1 = fgetc(file->in1)) != -1 && (read2 = fgetc(file->in2)) != -1){
		if (read1 == '\n' || read2 == '\n' || read1 == -1 || read2 == -1){
			if(mistakes != 0){
				fprintf(file->out, "Line: %d Character: %d\n", row, mistakes);
			}
			if(read1 == '\n' || read1 == -1){
				while(read2 != '\n' && read2 != -1){
					read2 = fgetc(file->in2);
				}
			}
			else if(read2 == '\n' || read2 == -1){
				while(read1 != '\n' && read1 != -1) {
					read1 = fgetc(file->in1);
				}
			}

			row++;
			mistakes = 0;
		}
		else if (char_to_compare(read1, flags) != char_to_compare(read2, flags)){
			mistakes++;
		}

	}

	if (!feof(file->in1) && !feof(file->in2))
	{ // only one file has to be read to the end since it stops after fully reading one
		ERROR_EXIT("couldn't read files");
	}
}

int main(int argc, char **argv)
{

	name = argv[0];

	Flags flags = {0};

	FILES file = {NULL, NULL, stdout};

	handle_arguments(&flags, &file, argc, argv);
	handle_files(&file, flags);

	fclose(file.in1);
	fclose(file.in2);
	fclose(file.out);

	exit(EXIT_SUCCESS);
}