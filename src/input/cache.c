#include "cache.h"

#include <stdlib.h>

#include "../collections/vector.h"
#include "../panic.h"

icache_t *icache_new() {
    icache_t *cache = malloc(sizeof(icache_t));
    if (!cache) panic_internal("faield to allocate memory");
    cache->cache = vector_new();
    return cache;
}

// Cache given input and return unique id corresponding to it.
icache_id_t icache_cache(icache_t *cache, input_t *input) {
    icache_id_t id = cache->cache->len;
    vector_push(cache->cache, input);
    return id;
}

// Fetch an input from this cache by using unique `id` corresponding to the
// input.
input_t *icache_fetch(icache_t *cache, icache_id_t id) {
    if (id >= cache->cache->len)
        panic_internal("no input corresponding to the id %d", id);
    return vector_at(cache->cache, id);
}
