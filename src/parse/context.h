// Copyrignts (C) 2024 pogyomo. Released under the MIT license.

#ifndef TINYC_PARSE_CONTEXT_H_
#define TINYC_PARSE_CONTEXT_H_

#include "../context.h"

struct typedef_name {
    struct typedef_name *next;
    const char *name;
};

struct parse_context {
    struct context *ctx;
    struct typedef_name *typedef_names;
};

// Initialize `parse_context`. Should be called before use it.
void parse_context_init(struct parse_context *parse_ctx, struct context *ctx);

// Add `name` as typedef name.
void parse_context_insert_typedef_name(struct parse_context *ctx,
                                       const char *name);

// Check if `name` is registered as typedef name.
bool parse_context_typedef_name_exists(struct parse_context *ctx,
                                       const char *name);

#endif  // TINYC_PARSE_CONTEXT_H_
