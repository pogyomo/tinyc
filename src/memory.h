#ifndef TINYC_MEMORY_H_
#define TINYC_MEMORY_H_

#include <stddef.h>

// Almost same as `malloc` but panic when failed allocation.
void *malloc_panic(size_t size);

// Almost same as `realloc` but panic when failed allocation.
void *realloc_panic(void *ptr, size_t size);

// Almost same as `calloc` but panic when failed allocation.
void *calloc_panic(size_t num, size_t size);

#endif  // TINYC_MEMORY_H_
