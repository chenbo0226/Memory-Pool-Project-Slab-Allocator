#include <stdio.h>

#include "memory_pool.h"

int main(void)
{
    pool_init();

    char *first = pool_alloc();
    char *second = pool_alloc();

    if (first == NULL || second == NULL) {
        fprintf(stderr, "Failed to allocate the initial memory blocks.\n");
        return 1;
    }

    snprintf(first, BLOCK_SIZE, "Legacy block 1");
    snprintf(second, BLOCK_SIZE, "Legacy block 2");

    printf("first  = %p: %s\n", (void *)first, first);
    printf("second = %p: %s\n", (void *)second, second);

    pool_free(first);

    char *reused = pool_alloc();
    if (reused == NULL) {
        fprintf(stderr, "Failed to allocate the returned memory block.\n");
        pool_free(second);
        return 1;
    }

    snprintf(reused, BLOCK_SIZE, "Reused legacy block");
    printf("reused = %p: %s\n", (void *)reused, reused);

    if (reused != first) {
        fprintf(stderr, "Free-list LIFO reuse test failed.\n");
        pool_free(second);
        pool_free(reused);
        return 1;
    }

    printf("Success: the free-list reused the most recently freed block.\n");

    pool_free(second);
    pool_free(reused);
    return 0;
}
