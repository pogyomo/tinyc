// Copyrignts (C) 2024 pogyomo. Released under the MIT license.

#ifndef TINYC_CONTEXT_H_
#define TINYC_CONTEXT_H_

#include <stdbool.h>
#include <stddef.h>

#include "string.h"
#include "vector.h"

// The values `context_fetch` returns.
struct cache_entry {
    const char *name;       // Where the content come from.
    struct string content;  // No modified content of input.
};

// Collection of data used through entire compile process.
struct context {
    struct vector caches;  // Collection of `cache_entry`.
    bool should_report;
};

// Initialize `context`. Should be called before use it.
void context_init(struct context *ctx);

// Suppress report.
void context_suppress_report(struct context *ctx);

// Activate report.
void context_activate_report(struct context *ctx);

// Cache given file, returns unique id for it.
size_t context_cache_file(struct context *ctx, const char *path);

// Fetch cached code associated with `id`.
const struct cache_entry *context_fetch(const struct context *ctx, size_t id);

#endif  // TINYC_CONTEXT_H_
