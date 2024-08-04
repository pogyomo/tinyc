#ifndef TINYC_CPP_CONTEXT_H_
#define TINYC_CPP_CONTEXT_H_

#include "../context.h"

struct cpp_context {
    struct context *ctx;
    size_t id;
};

// Initialize `struct cpp_context`. Should be called before use it.
void cpp_context_init(struct cpp_context *cpp_ctx, struct context *ctx,
                      size_t id);

#endif  // TINYC_CPP_CONTEXT_H_
