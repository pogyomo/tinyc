#include "stream.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../panic.h"
#include "../span.h"
#include "input.h"

istream_t *istream_new(input_t *input) {
    istream_t *is = malloc(sizeof(istream_t));
    if (!is) panic_internal("failed to allocate memory");
    is->input = input;
    is->lrow = 0;
    is->row = 0;
    is->offset = 0;
    return is;
}

bool istream_eos(istream_t *is) { return is->input->lines->len <= is->row; }

char istream_char(istream_t *is) {
    if (istream_eos(is))
        panic_internal("char called when istream reach to eos");
    return string_at(input_get_line(is->input, is->row), is->offset);
}

istream_state_t istream_state(istream_t *is) {
    istream_state_t state = {is->lrow, is->row, is->offset};
    return state;
}

void istream_set_state(istream_t *is, istream_state_t state) {
    is->lrow = state.lrow;
    is->row = state.row;
    is->offset = state.offset;
}

position_t istream_pos(istream_t *is) {
    if (istream_eos(is)) panic_internal("pos called when istream reach to eos");
    position_t pos = {is->row, is->offset};
    return pos;
}

void istream_advance(istream_t *is) {
    if (istream_eos(is)) return;

    bool continue_line = false;
    is->offset++;
    if (is->offset + 1 == input_get_line(is->input, is->row)->len &&
        istream_char(is) == '\\') {
        continue_line = true;
        is->offset++;
    }

    if (is->offset == input_get_line(is->input, is->row)->len) {
        if (!continue_line) {
            is->lrow++;
        }
        is->row++;
        is->offset = 0;
        while (is->row < is->input->lines->len &&
               input_get_line(is->input, is->row)->len == 0) {
            is->lrow++;
            is->row++;
        }
    }
}

bool istream_accept(istream_t *is, char *s, position_t *end) {
    if (istream_eos(is)) {
        return false;
    }

    size_t lrow = is->lrow;
    size_t row = is->row;
    size_t offset = is->offset;

    position_t acc_end = istream_pos(is);
    size_t len = strlen(s);
    for (int i = 0; i < len; i++) {
        if (istream_eos(is) || is->lrow != lrow || istream_char(is) != s[i]) {
            is->lrow = lrow;
            is->row = row;
            is->offset = offset;
            return false;
        }
        acc_end = istream_pos(is);
        istream_advance(is);
    }
    *end = acc_end;
    return true;
}
