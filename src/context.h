#ifndef TINYC_CONTEXT_H_
#define TINYC_CONTEXT_H_

#include "cache.h"
#include "preprocessor/macro.h"

typedef struct {
    cache_t cache;
    mtable_t mtable;
} context_t;

void context_init(context_t *ctx);

#endif  // TINYC_CONTEXT_H_
