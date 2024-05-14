#ifndef TINYC_INPUT_CACHE_H_
#define TINYC_INPUT_CACHE_H_

#include <stdlib.h>

#include "../collections/vector.h"
#include "input.h"

// A data structure which hold inputs and manage it by unique ids.
typedef struct {
    vector_t *cache;
} icache_t;

typedef size_t icache_id_t;

// Create a new empty cache.
icache_t *icache_new();

// Cache given input and return unique id corresponding to it.
icache_id_t icache_cache(icache_t *cache, input_t *input);

// Fetch an input from this cache by using unique `id` corresponding to the
// input.
input_t *icache_fetch(icache_t *cache, icache_id_t id);

#endif  // TINYC_INPUT_CACHE_H_
