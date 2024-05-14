#ifndef TINYC_INPUT_STREAM_H_
#define TINYC_INPUT_STREAM_H_

#include <stdbool.h>
#include <stdlib.h>

#include "../span.h"
#include "input.h"

typedef struct {
    input_t *input;
    size_t lrow;
    size_t row;
    size_t offset;
} istream_t;

// Construct a new istream from `input`.
istream_t *istream_new(input_t *input);

// Returns non zero value if this stream reach to eos.
bool istream_eos(istream_t *is);

// Returns currently peeking character.
char istream_char(istream_t *is);

// Returns currently peeking character's position in input.
position_t istream_pos(istream_t *is);

// Advance position of this stream so that `istream_get_*` can get next item.
void istream_advance(istream_t *is);

// If current character is `c`, advance position to skip `c`, then set `end` to
// the position of `c` in straem and return true.
bool istream_accept(istream_t *is, char c, position_t *end);

// If current characters is `s`, advance position to skip `s`, then set `end` to
// the last character's position of `s` in straem and return true.
bool istream_accept_str(istream_t *is, string_t *s, position_t *end);

#endif  // TINYC_INPUT_STREAM_H_
