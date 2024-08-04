#ifndef TINYC_CPP_CPP_H_
#define TINYC_CPP_CPP_H_

#include <stdbool.h>

#include "../context.h"
#include "token.h"

// Do preprocess for given file specified by `path`, then initialize `tokens`
// with it. `*tokens` will be NULL if no token generated.
// This function returns false if and only if the preprocessing failed.
bool cpp_file(struct context *ctx, const char *path, struct cpp_token **tokens);

#endif  // TINYC_CPP_CPP_H_
