#include "preprocessor.h"

#include <stdbool.h>

#include "../collections/vector.h"
#include "../lexer/stream.h"

// TODO: Add features
tstream_t *preprocess(context_t *ctx, tstream_t *ts) {
    vector_t *tokens = vector_new();
    while (!tstream_eos(ts)) {
        // Just ignore spaces
        if (tstream_token(ts)->kind != TK_SPACE) {
            vector_push(tokens, tstream_token(ts));
        }
        tstream_advance(ts);
    }
    return tstream_new(tokens);
}
