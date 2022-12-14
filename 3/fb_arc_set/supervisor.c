#include <stdio.h>
#include <signal.h>
#include <limits.h>
#include "circularBuffer.h"

#define USAGE()                                             \
	do                                                      \
	{                                                       \
		fprintf(stderr, "%s no arguments allowed\n", name); \
		exit(EXIT_FAILURE);                                 \
	} while (0)

int terminate = 0;

/**
 * @brief signalhandler
 * 
 * @arg sig_num: number of the signal called 
 */
void signalhandler(int sig_num);

/**
 * @brief sets up the signals used in the program
 */
static void setup_signals(void);

void signal_handler(int sig_num)
{
	terminate++;
}

static void setup_signals(void){

	struct sigaction sa;
  	sigemptyset(&sa.sa_mask);
 	sa.sa_handler = signal_handler;
  	sa.sa_flags = 0;
  	sigaction(SIGINT, &sa, 0);
	sigaction(SIGTERM, &sa, 0);

	// i am pretty certain that the lecture tells you to do this the following way
	// signal(SIGINT, signal_handler);
	// signal(SIGTERM, signal_handler);
	// while this is fine in most programs in this it would lead to problems
	// if a signalhandler is set with signal and the program is interupted while it waits at a sem_wait then it will continue waiting at the sem_wait after the signal handler is executed
	// this means it would be impossible to shutdown the application while it waits at a semaphore
	// for this reason sigaction is used instead
}

int main(int argc, const char **argv)
{
    name = argv[0];
    setup_signals();

    if (argc != 1)
    {
        USAGE();
    }

    setup_buffer();

    Solution newSolution = {.amount = INT_MAX};
    Solution bestSolution = {.amount = INT_MAX};

	fprintf(stdout, "waiting for solutions from ./generator\n");

    while (!terminate)
    {

        int error_value;
        if ((error_value = circ_buf_read(&newSolution)) != 0)
        {
            if (error_value == 1)
            {
                break;
            }
            ERROR_EXIT("error while reading buffer");
        }

        if (newSolution.amount == 0)
        {
            fprintf(stdout, "This graph is already acyclic\n");
            break;
        }

        if (newSolution.amount < bestSolution.amount)
        {
            bestSolution = newSolution;
            printEdge(newSolution.edges, newSolution.amount);
        }
    }

	fprintf(stdout,"shutting down\n");
    set_state(0);
    sem_post(free_sem);

    clean_buffer();
    exit(EXIT_SUCCESS);
}