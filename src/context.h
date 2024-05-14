#ifndef TINYC_CONTEXT_H_
#define TINYC_CONTEXT_H_

#include "input/cache.h"

typedef struct {
    icache_t *cache;
} context_t;

context_t *context_new();

#endif  // TINYC_CONTEXT_H_
