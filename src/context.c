#include "context.h"

#include "cache.h"
#include "preprocessor/macro.h"

void context_init(context_t *ctx) {
    cache_init(&ctx->cache);
    mtable_init(&ctx->mtable);
}
