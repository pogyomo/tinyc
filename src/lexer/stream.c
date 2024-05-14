#include "stream.h"

#include <stdlib.h>

#include "../collections/vector.h"
#include "../report/panic.h"
#include "token.h"

tstream_t *tstream_new(vector_t *tokens) {
    tstream_t *ts = malloc(sizeof(tstream_t));
    if (!ts) panic("failed to allocate memory");
    ts->tokens = tokens;
    ts->pos = 0;
    return ts;
}

int tstream_is_eos(tstream_t *ts) { return ts->tokens->len <= ts->pos; }

token_t *tstream_get_token(tstream_t *ts) {
    if (tstream_is_eos(ts)) {
        panic("get from token stream which reach to eos");
    }
    return vector_at(ts->tokens, ts->pos);
}

size_t tstream_get_state(tstream_t *ts) { return ts->pos; }

void tstream_set_state(tstream_t *ts, size_t state) { ts->pos = state; }

void tstream_advance(tstream_t *ts) {
    if (tstream_is_eos(ts)) return;
    ts->pos++;
}

void tstream_retreat(tstream_t *ts) {
    if (ts->pos == 0) return;
    ts->pos--;
}
