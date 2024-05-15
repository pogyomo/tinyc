#ifndef TINYC_PREPROCESSOR_PREPROCESSOR_H_
#define TINYC_PREPROCESSOR_PREPROCESSOR_H_

#include "../context.h"
#include "../lexer/stream.h"

// Process directives in `ts` and return processed token stream.
// If failed, report error and return NULL.
tstream_t *preprocess(context_t *ctx, tstream_t *ts);

#endif  // TINYC_PREPROCESSOR_PREPROCESSOR_H_
