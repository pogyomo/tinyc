#include "stream.h"

#include "panic.h"

void istream_init(istream_t *is, input_t *input) {
    is->input = input;
    is->lrow = 0;
    is->row = 0;
    is->offset = 0;
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

bool istream_eos(istream_t *is) { return is->input->lines.len <= is->row; }

char istream_char(istream_t *is) {
    if (istream_eos(is))
        panic_internal("char called when istream reach to eos");
    return string_at(input_at(is->input, is->row), is->offset);
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
    if (is->offset + 1 == input_at(is->input, is->row)->len &&
        istream_char(is) == '\\') {
        continue_line = true;
        is->offset++;
    }

    if (is->offset == input_at(is->input, is->row)->len) {
        if (!continue_line) {
            is->lrow++;
        }
        is->row++;
        is->offset = 0;
        while (is->row < is->input->lines.len &&
               input_at(is->input, is->row)->len == 0) {
            is->lrow++;
            is->row++;
        }
    }
}

bool istream_accept(istream_t *is, char *s, position_t *end, string_t *buf) {
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
    if (end != NULL) *end = acc_end;
    if (buf != NULL) string_append(buf, s);
    return true;
}

void tstream_init(tstream_t *ts, vector_t tokens) {
    ts->tokens = tokens;
    ts->pos = 0;
}

bool tstream_eos(tstream_t *ts) { return ts->tokens.len <= ts->pos; }

token_t *tstream_token(tstream_t *ts) {
    if (tstream_eos(ts)) {
        panic_internal("get from token stream which reach to eos");
    }
    return vector_at(&ts->tokens, ts->pos);
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
