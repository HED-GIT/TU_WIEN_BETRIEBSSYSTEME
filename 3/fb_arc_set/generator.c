#include "circularBuffer.h"


#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))

#define DEBUG

void permutations(int * permutation, int n) {
  for (int i = 0; i < n; i++) {
    permutation[i] = i;
  }
	
  int j;
  int tmp;
  for (int i = 0; i < n+1; i++) {
    j = rand() % n;
    tmp = permutation[j];
    permutation[j] = permutation[i];
    permutation[i] = tmp;
  }

}

static returnValue calculateOneSolution(const int argc, edge *edges, const int maxNode) {
	returnValue returns;
	int counter;
	int permutation[maxNode];
    permutations(permutation, maxNode);

    counter = 0;
    for (int i = 0; i < argc - 1; i++) {
      for (int j = 0; j < maxNode + 1; j++) {
        if (counter == MAXRETURN+1) {
          break;
        }
        if (edges[i].end == permutation[j]) {
          returns.returnEdges[counter] = edges[i];
          counter++;
        }
        if (edges[i].start == permutation[j]) {
          break;
        }
      }
    }
    returns.amount = counter;
	
	return returns;
}

int getMaxInt(struct edges * allEdges, int n) {
	int max = 0;
	for (int i = 0; i < n; i++) {
		max = MAX(allEdges[i].start,max);
		max = MAX(allEdges[i].end, max);
	}
	max++;
  return max;
}


void readInput(const int * argc, char * argv[], edge * graph){
  int startNode = 0;
  int endNode = 0;
  for (int i = 1; i < * argc; i++) {
    edge newEdge;
    if (sscanf(argv[i], "%d-%d", & startNode, & endNode) != 2 || startNode < 0 || endNode < 0) {
      clean_loaded_buffer();
      ERROR_EXIT("edge format has to be 'node'-'node'\nall nodes have to be a positiv integer values\n");
    }
	newEdge.start = startNode;
	newEdge.end = endNode;
    graph[i - 1] = newEdge;
  }
}


int main(int argc, char * argv[]) {
  name = argv[0];
  edge * graph = malloc(sizeof(edge)*(argc-1));

  load_buffer();
  
  
  if (1 == argc) {
    clean_loaded_buffer();
    ERROR_EXIT("graph has to have at least one edge");
  }

  if(increment_state()!=0){
	ERROR_EXIT("couldn't increment system state");
  }
  srand(time(0) + get_state() * 1000);
  
  readInput(&argc,argv,graph);

  int amountVertices = getMaxInt(graph, argc);
  
  int state;
  while ((state = get_state()) > 0) {
    returnValue returns = calculateOneSolution(argc,graph,amountVertices);
 
    
    if (returns.amount < MAXRETURN) {
#ifdef DEBUG
			printEdge(returns.returnEdges, returns.amount);
#endif
			if(circ_buf_write(&returns)!=0){
				ERROR_EXIT("error writing to shared memory");
			}
		}
		else {
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