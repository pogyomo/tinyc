#include "stream.h"

#include <stdbool.h>
#include <stdlib.h>

#include "../report/panic.h"
#include "../span.h"
#include "input.h"

istream_t *istream_new(input_t *input) {
    istream_t *is = malloc(sizeof(input_t));
    if (!is) panic("failed to allocate memory");
    is->input = input;
    is->lrow = 0;
    is->row = 0;
    is->offset = 0;
    return is;
}

// Returns non zero value if this stream reach to eos.
bool istream_is_eos(istream_t *is) { return is->input->lines->len <= is->row; }

// Returns currently peeking character.
char istream_get_char(istream_t *is) {
    if (istream_is_eos(is)) panic("get_char called when istream reach to eos");
    return string_at(input_get_line(is->input, is->row), is->offset);
}

// Returns currently peeking character's position in input.
position_t istream_get_pos(istream_t *is) {
    position_t pos = {is->row, is->offset};
    return pos;
}

// Advance position of this stream so that `istream_get_*` can get next item.
void istream_advance(istream_t *is) {
    if (istream_is_eos(is)) return;
    is->offset++;
    if (is->offset + 1 == input_get_line(is->input, is->row)->len &&
        istream_get_char(is) == '\\') {
        is->offset++;
    }
}

int istream_accept(istream_t *is, char c, position_t *end);

int istream_accept_str(istream_t *is, char *s, position_t *end);
