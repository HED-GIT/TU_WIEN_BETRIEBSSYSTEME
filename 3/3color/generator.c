#include <time.h>
#include "circularBuffer.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

//#define DEBUG

/**
 * @brief calculate one solution
 * 
 * @arg argc: amount of edges of the graph
 * @arg allEdges: edges of the graph
 * @arg maxNode: biggest node of the graph
 */
static Solution calculate_one_solution(const int argc, const Edge *edges, const int maxNode);

/**
 * @brief reads the graph and writes it to edges
 * 
 * @arg argc: argument count of program
 * @arg argv: arguments of program
 * @arg allEdges: memory to save the read graph to
 */
static void read_input(const int argc, const char **argv, Edge *allEdges);

/**
 * @brief handles the generating of solutions and sends them to the supervisor
 * 
 * @arg max: the biggest node of the graph
 * @arg argc: amount of edges
 * @arg allEdges: list of edges
*/
static void handler(const int max, const int argc, const Edge *allEdges);


static void handler(const int max, const int argc, const Edge *allEdges)
{
	Solution my_solution;

	int state;
	while ((state = get_state()) > 0)
	{

		my_solution = calculate_one_solution(argc, allEdges, max);

		if (my_solution.amount < 0)
		{
#ifdef DEBUG
			fprintf(stdout, "Solution too Big\n");
#endif 
		}
		else
		{
#ifdef DEBUG
			printEdge(my_solution.edges, my_solution.amount);
#endif
			if (circ_buf_write(&my_solution) != 0)
			{
				ERROR_EXIT("error writing to shared memory");
			}
		}
	}
	if (state != 0)
	{
		ERROR_EXIT("couldn't fetch program state");
	}
}

static void read_input(const int argc, const char **argv, Edge *allEdges)
{
	int startNode = 0;
	int endNode = 0;
	for (int i = 1; i < argc; i++)
	{
		if (sscanf(argv[i], "%d-%d", &startNode, &endNode) != 2)
		{
			ERROR_EXIT("all nodes have to have the form number-number");
		}
		Edge Edge;
		Edge.start = MIN(startNode, endNode);
		Edge.end = MAX(startNode, endNode);

		allEdges[i - 1] = Edge;
	}
}

static Solution calculate_one_solution(const int argc, const Edge *edges, const int maxNode)
{
	Solution newSolution;

	int node_color[maxNode];
	for (int i = 0; i < maxNode; i++)
	{
		int num = (rand() % (3)) + 1;
		node_color[i] = num;
	}

	int NumberColorConflicts = 0;
	for (int i = 0; i < argc - 1; i++)
	{
		if (node_color[edges[i].start] == node_color[edges[i].end])
		{
			if (NumberColorConflicts >= MAXRETURN)	// if to many conflicts are found
			{
				newSolution.amount = -1;	// indicates that to many are found
				return newSolution;
			}
			newSolution.edges[NumberColorConflicts] = edges[i];
			NumberColorConflicts++;
		}
	}
	newSolution.amount = NumberColorConflicts;
	return newSolution;
}

int main(int argc, const char **argv)
{
	name = argv[0];

	if (1 == argc)
	{
		ERROR_EXIT("no graph given");
	}

	Edge *allEdges = malloc(sizeof(Edge) * (argc - 1));
	read_input(argc, argv, allEdges);

	load_buffer();

	if (increment_state() != 0)
	{
		ERROR_EXIT("couldn't increment system state");
	}

	// this is done to ensure that all generator run on a different seed
	// if you would just use time(0) as your seed then all generator that started in the same second would use the same seed
	// this is not needed to pass the assignment
	// if you do not implement this mechanism then you can also remove the set_state, get_state and increment_state functions and instead set the values directly.
	// you would also not increment the state after a generator is started since the incremented value is only important for this mechanic
	srand(time(0) + get_state() * 1000);


	int max = 0;
	for (int i = 0; i < argc - 1; i++)
	{
		// we do not have to check if the start is bigger then the end since we sorted them that way during the read_input function
		max = MAX(allEdges[i].end, max);
	}
	max++;
	handler(max, argc, allEdges);

	free(allEdges);


	// if the supervisor terminates it makes a sem_post to free_sem because one or more generator could be currently waiting to write
	// this allows one generator to terminate
	// by also doing a sem_post here we free another spot in the circular buffer, allowing another generator to terminate
	// which then also makes a sem post,...
	// this goes on till all generator terminate 
	sem_post(free_sem);

	clean_loaded_buffer();

	SUCCESS_EXIT();
}
