#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

// shared memory name
#define SHMNAME "/your_student_id_SHM"

// name of semaphore indicating if the buffer is full
#define SEM_F "/your_student_id_sem_f"
// name of smeaphore indicating if there is data in the buffer
#define SEM_U "/your_student_id_sem_u"
// name of semaphore indicating if someone is currently writing
#define SEM_W "/your_student_id_sem_W"

// size of the circular buffer
#define BUFFERLENGTH 5

// maximum amount of edges in a solution
#define MAXRETURN 8

// macro used to terminate with an error
#define ERROR_EXIT(...)                                       \
    do                                                        \
    {                                                         \
        fprintf(stderr, "%s ERROR: " __VA_ARGS__ "\n", name); \
        exit(EXIT_FAILURE);                                   \
    } while (0)

// macro used to terminate successfully
#define SUCCESS_EXIT()      \
    do                      \
    {                       \
        exit(EXIT_SUCCESS); \
    } while (0)

/**
*@brief saves start and end vertice of an edge
*/
typedef struct Edge
{
    int start;
    int end;
} Edge;

/**
*@brief saves the amount of edges and the edges that should be removed (max 8)
*/
typedef struct Solution
{
    Edge edges[MAXRETURN];
    int amount;
} Solution;

/**
 * @brief the structure of our buffer
 */
typedef struct Buffer
{
    int state;
    Solution values[BUFFERLENGTH];
    int writePosition;
    int readPosition;
} Buffer;

/**
 * @brief writes value to the circular buffer
 * @arg val: value to write to the buffer
 * 
 * @return 0 on success, anything else on error
 */
int circ_buf_write(const Solution *val);

/**
 * @brief reads value from the circular buffer
 * @arg value: memory to write the data to
 * 
 * @return 0 on success, anything else on error
 */
int circ_buf_read(Solution *value);

/**
 * @brief sets up the shared memory and the semaphore for the supervisor
 */
void setup_buffer();

/**
 * @brief destroys the shared memory and the semaphore for the supervisor
 */
void clean_buffer();

/**
 * @brief sets up the shared memory and the semaphore for the generator
 */
void load_buffer();

/**
 * @brief destroys the shared memory and the semaphore for the generator
 */
void clean_loaded_buffer();

/**
 * @brief increments state of the system
 */
int increment_state();

/**
 * @brief sets state of the system
 */
int set_state(int i);

/**
 * @brief gets the state of the system
 * 
 * @returns value of the system
 */
int get_state();

/**
 * @brief prints an array of edges
 * 
 * @arg i_edge: list of edges to print
 * @arg length: amount of edges to print
 */
void printEdge(const Edge *i_edge, int length);

// semaphore indicating buffer is full
extern sem_t *free_sem;
// semaphore indicating if there is data in the buffer
extern sem_t *used_sem;
// semaphore indicating if someone is currently writing
extern sem_t *write_sem;

// shared memory descriptor
extern int shmfd;
// shared memory
extern Buffer *buf;
// name of the program
extern const char *name;