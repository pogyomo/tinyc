#ifndef TINYC_CACHE_T_
#define TINYC_CACHE_T_

#include "collections/string.h"
#include "collections/vector.h"

// A struct holding a input like file as newline-splitted lines.
typedef struct {
    VECTOR(string_t) lines;
    string_t name;  // Name of this input.
} input_t;

// Initialize `input` with file in `path`.
void input_from_file(input_t *input, char *path);

// Returns `n`-th line of `input`.
string_t *input_at(input_t *input, size_t n);

// A struct to manage inputs with unique id.
typedef struct {
    VECTOR(input_t) caches;
} cache_t;

// A unique id associated with a input.
typedef size_t cache_id_t;

// Initialize `cache` to be empty.
void cache_init(cache_t *cache);

// Cache a input by creating it from file in `path`.
cache_id_t cache_file(cache_t *cache, char *path);

// Fetch input from `cache` with `id`.
input_t *cache_fetch(cache_t *cache, cache_id_t id);

#endif  // TINYC_CACHE_T_
