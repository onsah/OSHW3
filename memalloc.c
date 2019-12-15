#include "memalloc.h"

int mem_init (void *chunkptr, int size, int method)
{
    chunk_info.m_ptr = chunkptr;
    chunk_info.m_size = size * 1024;
    chunk_info.m_method = method;

    // We have only a single empty hole
    head = (Hole*) chunkptr;
    head->is_free = 1;
    head->size = (chunk_info.m_size) - sizeof (Hole);
    head->next = 0;

    // init mutex
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&mutex, &attr);

    return 0;
}

void *mem_allocate (int size)
{
    if (size < MIN_ALLOC_REQUEST || size > MAX_ALLOC_REQUEST) {
        printf("size %d is not an acceptable amount\n", size);
        return 0;
    }

    // printf("locking...\n");
    pthread_mutex_lock (&mutex);

    // TODO: add best and worst fit too
    // printf("searching...\n");
    Hole *found = 0;
    for (Hole *curr = head; curr != 0; curr = curr->next) {
        if (curr->is_free) {
            if (curr->size >= size) {
                switch (chunk_info.m_method) {
                    case BestFit:
                        if (found) {
                            // curr is smaller so better bestfit
                            if (found->size > curr->size)
                                found = curr;
                        } else {
                            found = curr;
                        }
                        break;
                    case WorstFirst:
                        if (found) {
                            if (found->size < curr->size)
                                found = curr;
                        } else {
                            found = curr;
                        }
                        break;
                    case FirstFit:
                    default:
                        found = curr;
                        // FirstFit so exit when found one
                        goto endsearch;
                }
            }
        }
    }
endsearch:
    ;

    // split the block
    int rem_size = found->size - size;
    printf("Remaining size: %d bytes\n", rem_size);
    found->is_free = 0;
    found->size = size;

    // Split if there is a remaining size
    if (rem_size > 0) {
        Hole *next = HOLE_START(found) + found->size;
        next->is_free = 1;
        next->size = rem_size - sizeof (Hole);
        next->next = found->next;

        found->next = next;
    }

    // printf("unlocking...\n");
    pthread_mutex_unlock (&mutex);
    return found ? HOLE_START(found) : 0;
}

void mem_free (void *ptr)
{
    pthread_mutex_lock(&mutex);

    Hole *prev = 0;
    Hole *found;
    for (Hole *curr = head; curr != 0; curr = curr->next) {
        if (HOLE_START(curr) == ptr) {
            if (!curr->is_free) {
                found = curr;
                break;
            } else {
                printf ("Hole is already freed\n");
                break;
            }
        }
        prev = curr;
    }

    if (found) {
        found->is_free = 1;
        // Check if can merge with next block
        if (can_merge (found)) {
            merge (found);
        }
        if (can_merge (prev)) {
            merge (prev);
        }
    } else {
        printf("Chunk is not found\n");
    }
    
    pthread_mutex_unlock(&mutex);
}

int can_merge(Hole *hole) {
    if (hole) {
        if (hole->next) {
            return hole->is_free && hole->next->is_free;
        }
    }
    return 0;
}

/**
 * Merges two freed blocks
 * Always called after @can_merge
 */
void merge(Hole* hole) {
    Hole *next = hole->next;

    hole->size += next->size + sizeof (Hole);
    hole->next = next->next;
}

void mem_print ()
{
    printf("========Heap Region========\n");
    for (Hole *curr = head; curr != 0; curr = curr->next) {
        printf("{ size: %d, is_free: %s }\n", curr->size, curr->is_free ? "true" : "false");
    }
    printf("========Heap Region========\n");
}