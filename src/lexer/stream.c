#include "stream.h"

#include <stdlib.h>

#include "../collections/vector.h"
#include "../panic.h"
#include "token.h"

tstream_t *tstream_new(vector_t *tokens) {
    tstream_t *ts = malloc(sizeof(tstream_t));
    if (!ts) panic_internal("failed to allocate memory");
    ts->tokens = tokens;
    ts->pos = 0;
    return ts;
}

bool tstream_eos(tstream_t *ts) { return ts->tokens->len <= ts->pos; }

token_t *tstream_token(tstream_t *ts) {
    if (tstream_eos(ts)) {
        panic_internal("get from token stream which reach to eos");
    }
    return vector_at(ts->tokens, ts->pos);
}

tstream_state_t tstream_state(tstream_t *ts) { return ts->pos; }

void tstream_set_state(tstream_t *ts, tstream_state_t state) {
    ts->pos = state;
}

void tstream_advance(tstream_t *ts) {
    if (tstream_eos(ts)) return;
    ts->pos++;
}

void tstream_retreat(tstream_t *ts) {
    if (ts->pos == 0) return;
    ts->pos--;
}
