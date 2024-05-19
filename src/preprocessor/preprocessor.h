#ifndef TINYC_PREPROCESSOR_PREPROCESSOR_H_
#define TINYC_PREPROCESSOR_PREPROCESSOR_H_

#include <stdbool.h>

#include "../context.h"

// Do preprocess to given `input` and initialize `output` with it.
// `input` and `output` must be different.
// Returns false if error happen.
bool preprocess(context_t *ctx, vector_t *restrict input,
                vector_t *restrict output);

#endif  // TINYC_PREPROCESSOR_PREPROCESSOR_H_
