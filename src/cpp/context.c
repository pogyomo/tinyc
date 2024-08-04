#include "context.h"

void cpp_context_init(struct cpp_context *cpp_ctx, struct context *ctx,
                      size_t id) {
    cpp_ctx->ctx = ctx;
    cpp_ctx->id = id;
}
