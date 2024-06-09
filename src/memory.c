#include "memory.h"

#include <stdlib.h>

#include "panic.h"

void *malloc_panic(size_t size) {
    void *ptr = NULL;
    if (!(ptr = malloc(size))) {
        panic_internal("failed to allocate memory");
    }
    return ptr;
}

void *realloc_panic(void *ptr, size_t size) {
    if (!(ptr = realloc(ptr, size))) {
        panic_internal("failed to reallocate memory");
    }
    return ptr;
}

void *calloc_panic(size_t num, size_t size) {
    void *ptr = NULL;
    if (!(ptr = calloc(num, size))) {
        panic_internal("failed to allocate memory");
    }
    return ptr;
}
