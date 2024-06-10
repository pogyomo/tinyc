#ifndef TINYC_STREAM_H_
#define TINYC_STREAM_H_

#include <string.h>

#include "lexer/token.h"

// Stream of input.
typedef struct {
    input_t *input;
    size_t lrow;
    size_t row;
    size_t offset;
} istream_t;

// Internal state of `istream_t`.
typedef struct {
    size_t lrow;
    size_t row;
    size_t offset;
} istream_state_t;

// Initialize `is` with `input`.
void istream_init(istream_t *is, input_t *input);

// Get internal state of `is`.
istream_state_t istream_state(istream_t *is);

// Set internal state of `is` to `state`.
void istream_set_state(istream_t *is, istream_state_t state);

// Returns true if `is` reach to eos.
bool istream_eos(istream_t *is);

// Returns a character currently `is` is peeking.
char istream_char(istream_t *is);

// Returns a position of character currently `is` is peeking.
position_t istream_pos(istream_t *is);

// Advance position so that next item is available.
void istream_advance(istream_t *is);

// If first n characters of `is` is `s`, advance position to skip these
// character, set `end` to last character's position, append `s` to `buf`, then
// return true.
// You can set `end` and `buf` to NULL.
bool istream_accept(istream_t *is, char *s, position_t *end, string_t *buf);

// Stream of tokens.
typedef struct {
    VECTOR_REF(token_t) tokens;
    size_t pos;
} tstream_t;

// Internal state of tstream_t.
typedef size_t tstream_state_t;

// Construct a new token stream from list of token.
void tstream_init(tstream_t *ts, vector_t *tokens);

// Returns non zero value if this stream reach to eos.
bool tstream_eos(tstream_t *ts);

// Returns true if current token's kind is `kind`.
bool tstream_is(tstream_t *ts, token_kind_t kind);

// Returns true and advance stream if current token's kind is `kind`.
// `span` will be the token's span if it kind is `kind`.
bool tstream_expect(tstream_t *ts, token_kind_t kind, span_t *span);

// Returns current peeking token.
token_t *tstream_token(tstream_t *ts);

// Returns last token in this stream.
token_t *tstream_last(tstream_t *ts);

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

#endif  // TINYC_STREAM_H_
