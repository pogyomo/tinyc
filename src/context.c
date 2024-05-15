#include "context.h"

#include <stdlib.h>

#include "input/cache.h"
#include "panic.h"
#include "preprocessor/table.h"

context_t *context_new() {
    context_t *ctx = malloc(sizeof(context_t));
    if (!ctx) panic_internal("failed to allocate memory");
    ctx->icache = icache_new();
    ctx->mtable = mtable_new();
    return ctx;
}
