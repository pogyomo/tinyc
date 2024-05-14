#include "context.h"

#include <stdlib.h>

#include "input/cache.h"
#include "panic.h"

context_t *context_new() {
    context_t *ctx = malloc(sizeof(context_t));
    if (!ctx) panic_internal("failed to allocate memory");
    ctx->cache = icache_new();
    return ctx;
}
