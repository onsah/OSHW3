#include "memalloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

void *alloc_test (int size) {
    printf ("test for allocating %d bytes...\n", size);

    void *chunk = mem_allocate (size);

    printf ("allocated %d bytes in adress %p\n", size, chunk);
    mem_print ();

    return chunk;
}

void free_test (void *ptr) {
    printf ("test for freeing adress %p\n", ptr);

    mem_free (ptr);
    printf ("freed adress %p\n", ptr);
    mem_print ();
}

int main (int argc, char *argv[]) {
    
    int size = 64; //KB
    void *chunkptr = malloc (size);

    if (argc < 2) {
        printf("needs at least an argument\n");
        exit(1);
    }

    Method m = atoi(argv[1]);

    printf("Method: %d\n", m);

    mem_init (chunkptr, size, m);

    mem_print ();

    void *ptr = alloc_test (256);
    // Must be sizeof (Hole) next
    assert (ptr == head + sizeof (Hole));

    alloc_test (256);

    void *ptr3 = alloc_test (128);
    alloc_test (128);

    free_test (ptr);
    free_test (ptr3);

    alloc_test (128);
}