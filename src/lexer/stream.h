#ifndef TINYC_LEXER_STREAM_H_
#define TINYC_LEXER_STREAM_H_

#include <stdbool.h>
#include <stdlib.h>

#include "../collections/vector.h"
#include "token.h"

typedef struct {
    vector_t *tokens;
    size_t pos;
} tstream_t;

typedef size_t tstream_state_t;

// Construct a new token stream from list of token.
tstream_t *tstream_new(vector_t *tokens);

// Returns non zero value if this stream reach to eos.
bool tstream_eos(tstream_t *ts);

// Returns current peeking token.
token_t *tstream_token(tstream_t *ts);

// Returns internal state of this stream which can be used at
// `tstream_set_state`.
tstream_state_t tstream_state(tstream_t *ts);

// Set internal state of this stream to `state` which `tstream_get_state`
// returns.
void tstream_set_state(tstream_t *ts, tstream_state_t state);

// Advance the position of this stream so that next token is available at
// `tstream_get_token`.
void tstream_advance(tstream_t *ts);

// Advance the position of this stream so that previous token is available at
// `tstream_get_token`.
void tstream_retreat(tstream_t *ts);

#endif  // TINYC_LEXER_STREAM_H_
