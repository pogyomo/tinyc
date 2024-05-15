#ifndef TINYC_CONTEXT_H_
#define TINYC_CONTEXT_H_

#include "input/cache.h"
#include "preprocessor/table.h"

typedef struct {
    icache_t *icache;
    mtable_t *mtable;
} context_t;

context_t *context_new();

#endif  // TINYC_CONTEXT_H_
