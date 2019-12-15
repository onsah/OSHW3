#ifndef MEM_ALLOC_H__
#define MEM_ALLOC_H__

#include <pthread.h>
#include <stdio.h>

#define MIN_CHUNK_SIZE (32)
#define MAX_CHUNK_SIZE (32 * 1024)
#define MIN_ALLOC_REQUEST (128)
#define MAX_ALLOC_REQUEST (2048000)
// Start adress of the hole
#define HOLE_START(hole_ptr) (hole_ptr + sizeof (Hole)) 

enum Method {
    FirstFit = 0,
    BestFit,
    WorstFirst,
};
typedef enum Method Method;

// This info is at the beginning of each hole
struct Hole {
    int is_free;
    int size;
    struct Hole *next;
} *head;
typedef struct Hole Hole;

struct {
    // Total heap area
    void *m_ptr;
    // Total heap size
    int m_size;
    // Allocation method
    Method m_method;
} chunk_info;

pthread_mutex_t mutex;

/**
 * Initializes the library to manage the given chunk
 * @chunkptr pointer to the memory chunk, allocated by application
 * @size size of the chunk (in bytes)
 * @method Method of allocation
 * @return 0: success, -1: error
 */
int mem_init (void *chunkptr, int size, int method);

/**
 * Allocate memory for a request
 * @size size of the chunk that will be allocated (in bytes)
 * @return pointer to the allocated memory
 */
void *mem_allocate (int size);

/**
 * Free an allocated memory
 * @ptr pointer to memory to be freed
 */
void mem_free (void *ptr);

/**
 * Prints the current state of the chunk in sorted order
 */
void mem_print ();

/**
 * Checks if can merge with next block
 */
int can_merge(Hole *hole);

void merge(Hole* hole);

#endif // MEM_ALLOC_H__