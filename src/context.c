#include "context.h"

#include <stdio.h>

#include "panic.h"
#include "string.h"
#include "vector.h"

#define BUFSIZE 1000

static char buf[BUFSIZE];

void context_init(struct context *ctx) {
    ctx->suppress_count = 0;
    vector_init(&ctx->caches, sizeof(struct cache_entry));
}

bool context_should_report(struct context *ctx) {
    return ctx->suppress_count == 0;
}

void context_suppress_report(struct context *ctx) { ctx->suppress_count++; }

void context_activate_report(struct context *ctx) {
    if (!ctx->suppress_count) fatal_error("cannot activate multiply");
    ctx->suppress_count--;
}

size_t context_cache_file(struct context *ctx, const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) fatal_error("no such file exists: %s", path);

    struct string content;
    string_init(&content);
    while (fgets(buf, BUFSIZE, fp)) {
        string_append(&content, buf);
    }

    struct cache_entry entry = {path, content};

    size_t id = ctx->caches.len;
    vector_push(&ctx->caches, &entry);
    return id;
}

const struct cache_entry *context_fetch(const struct context *ctx, size_t id) {
    if (id >= ctx->caches.len)
        fatal_error("no cache with associated with id %zu", id);
    return vector_at(&ctx->caches, id);
}
