#ifndef TINYC_PREPROCESSOR_PREPROCESSOR_H_
#define TINYC_PREPROCESSOR_PREPROCESSOR_H_

#include <stdbool.h>

#include "../context.h"

// Do preprocess to given `input` and initialize `output` with it.
// `input` and `output` must be different.
// Returns false if error happen.
bool preprocess(context_t *ctx, VECTOR_REF(token_t) restrict input,
                VECTOR_REF(token_t) restrict output);

#endif  // TINYC_PREPROCESSOR_PREPROCESSOR_H_
