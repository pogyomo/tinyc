#include "cache.h"

#include <stdio.h>

#include "collections/string.h"
#include "collections/vector.h"
#include "panic.h"

void input_from_file(input_t *input, char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) panic_internal("failed to open file");

    string_from(&input->name, path);
    vector_init(&input->lines, sizeof(string_t));

    string_t line;
    string_init(&line);
    char c;
    while (!feof(fp)) {
        c = fgetc(fp);
        if (c == '\n') {
            vector_push(&input->lines, &line);
            string_init(&line);
        } else {
            string_push(&line, c);
        }
    }
}

string_t *input_at(input_t *input, size_t n) {
    return vector_at(&input->lines, n);
}

void cache_init(cache_t *cache) {
    vector_init(&cache->caches, sizeof(input_t));
}

cache_id_t cache_file(cache_t *cache, char *path) {
    input_t input;
    input_from_file(&input, path);
    cache_id_t id = cache->caches.len;
    vector_push(&cache->caches, &input);
    return id;
}

input_t *cache_fetch(cache_t *cache, cache_id_t id) {
    return vector_at(&cache->caches, id);
}
