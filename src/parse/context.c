#include "context.h"

#include <stdlib.h>
#include <string.h>

#include "../panic.h"

void parse_context_init(struct parse_context *parse_ctx, struct context *ctx) {
    parse_ctx->ctx = ctx;
    parse_ctx->typedef_names = NULL;
}

void parse_context_insert_typedef_name(struct parse_context *ctx,
                                       const char *name) {
    struct typedef_name *new_name = malloc(sizeof(struct typedef_name));
    if (!new_name) fatal_error("memory allocation failed");
    new_name->next = ctx->typedef_names;
    new_name->name = name;
    ctx->typedef_names = new_name;
}

bool parse_context_typedef_name_exists(struct parse_context *ctx,
                                       const char *name) {
    for (struct typedef_name *typedef_name = ctx->typedef_names; typedef_name;
         typedef_name = typedef_name->next) {
        if (strcmp(name, typedef_name->name) == 0) return true;
    }
    return false;
}
