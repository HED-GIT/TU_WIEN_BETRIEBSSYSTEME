#include "circularBuffer.h"

/**
*@brief prints out the edges of an edge array
*@param i_edge: edge array to be printed, length: amount of edges which should be printed out
*/
void printEdge(const edge * i_edge, int length) {
  fprintf(stdout, "Solution with %d edges ", length);
  for (int i = 0; i < length; i++) {
    fprintf(stdout, "%d-%d ", i_edge[i].start, i_edge[i].end);
  }
  fprintf(stdout, "\n");
}
