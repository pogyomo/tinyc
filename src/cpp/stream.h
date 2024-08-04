#ifndef TINYC_CPP_STREAM_H_
#define TINYC_CPP_STREAM_H_

#include <stdbool.h>
#include <stddef.h>

#include "../span.h"

// Iterate a string by ignoreing `\\\n`.
struct stream {
    const char *s;
    size_t row;
    size_t col;
    size_t offset;
};

// Initialize `struct stream`. Should be called before use it.
void stream_init(struct stream *st, const char *s);

// Returns true if `st` reaches to end.
bool stream_eos(struct stream *st);

// Advance stream, and returns previous character.
// `pos` will be updated to the position of character that was consumed.
void stream_advance(struct stream *st, struct position *pos);

// Returns currently peeking character.
// Cause error if `stream_eos` returns true.
char stream_char(struct stream *st);

// Advance stream if current character is `c`, and in such cases, update `pos`
// to the position of consumed character.
bool stream_accept_c(struct stream *st, char c, struct position *pos);

// Advance stream if current characters is `s`, and in such cases, update `pos`
// to the position of lastly consumed character.
bool stream_accept_s(struct stream *st, const char *s, struct position *pos);

#endif  // TINYC_CPP_STREAM_H_
