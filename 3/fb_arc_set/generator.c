#include <time.h>
#include "circularBuffer.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

//#define DEBUG

/**
 * @brief creates a random permutation
 * @arg permutation: list of elements
 * @arg n: length of permutation
 */
static void permutations(int *permutation, const int n);

/**
 * @brief calculates one solution
 * @arg graphSize: amount of edges
 * @arg graph: edges of the graph
 * @arg maxNode: biggest node of the graph
 * 
 * @return: one possible solution
 */
static Solution calculate_one_solution(const int graphSize, const Edge *graph, const int maxNode);

/**
 * @brief gets the biggest node of the graph
 * @arg graph: edges of the graph
 * @arg graphSize: amount of edges
 * 
 * @return: bigest node of the graph
 */
static int get_max_node(const Edge *graph, const int graphSize);

/**
 * @brief reads the graph and writes it to edges
 * 
 * @arg argc: argument count of program
 * @arg argv: arguments of program
 * @arg graph: memory to save the read graph to
 */
static void read_input(const int argc, const char **argv, Edge *graph);

static void permutations(int *permutation, const int n)
{
    for (int i = 0; i < n; i++)
    {
        permutation[i] = i;
    }

    int j;
    int tmp;
    for (int i = 0; i < n + 1; i++)
    {
        j = rand() % n;
        tmp = permutation[j];
        permutation[j] = permutation[i];
        permutation[i] = tmp;
    }
}

static Solution calculate_one_solution(const int graphSize, const Edge *graph, const int maxNode)
{
    Solution returns;
    int counter;
    int permutation[maxNode];
    permutations(permutation, maxNode);

    counter = 0;
    for (int i = 0; i < graphSize - 1; i++)
    {
        for (int j = 0; j < maxNode + 1; j++)
        {
            if (counter == MAXRETURN + 1)
            {
                break;
            }
            if (graph[i].end == permutation[j])
            {
                returns.edges[counter] = graph[i];
                counter++;
            }
            if (graph[i].start == permutation[j])
            {
                break;
            }
        }
    }
    returns.amount = counter;

    return returns;
}

static int get_max_node(const Edge *graph, const int graphSize)
{
    int max = 0;
    for (int i = 0; i < graphSize; i++)
    {
        max = MAX(graph[i].start, max);
        max = MAX(graph[i].end, max);
    }
    max++;
    return max;
}

static void read_input(const int argc, const char **argv, Edge *graph)
{
    int startNode = 0;
    int endNode = 0;
    for (int i = 1; i < argc; i++)
    {
        Edge newEdge;
        if (sscanf(argv[i], "%d-%d", &startNode, &endNode) != 2 || startNode < 0 || endNode < 0)
        {
            clean_loaded_buffer();
            ERROR_EXIT("edge format has to be 'node'-'node'\nall nodes have to be a positiv integer values\n");
        }
        newEdge.start = startNode;
        newEdge.end = endNode;
        graph[i - 1] = newEdge;
    }
}

int main(int argc, const char **argv)
{
    name = argv[0];
    Edge *graph = malloc(sizeof(Edge) * (argc - 1));

    load_buffer();

    if (1 == argc)
    {
        clean_loaded_buffer();
        ERROR_EXIT("graph has to have at least one edge");
    }

    if (increment_state() != 0)
    {
        ERROR_EXIT("couldn't increment system state");
    }
    srand(time(0) + get_state() * 1000);

    read_input(argc, argv, graph);

    int amountVertices = get_max_node(graph, argc);

    int state;
    while ((state = get_state()) > 0)
    {
        Solution returns = calculate_one_solution(argc, graph, amountVertices);

        if (returns.amount < MAXRETURN)
        {
#ifdef DEBUG
            printEdge(returns.edges, returns.amount);
#endif
            if (circ_buf_write(&returns) != 0)
            {
                ERROR_EXIT("error writing to shared memory");
            }
        }
        else
        {
#ifdef DEBUG
            fprintf(stdout, "Solution too Big");
#endif
        }
    }

    free(graph);
    sem_post(free_sem);

    clean_loaded_buffer();
    exit(EXIT_SUCCESS);
}