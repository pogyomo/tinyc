#include "stream.h"

#include "../panic.h"

void stream_init(struct stream *st, const char *s) {
    st->s = s;
    st->row = st->col = st->offset = 0;
}

bool stream_eos(struct stream *st) { return st->s[st->offset] == '\0'; }

void stream_advance(struct stream *st, struct position *pos) {
    if (stream_eos(st)) return;
    pos->row = st->row;
    pos->col = st->col;

    st->col++;
    st->offset++;
    while (st->s[st->offset] && st->s[st->offset + 1]) {
        if (st->s[st->offset] == '\\' && st->s[st->offset + 1] == '\n') {
            st->offset += 2;
            st->row++;
            st->col = 0;
        } else {
            break;
        }
    }
}

char stream_char(struct stream *st) {
    if (stream_eos(st)) fatal_error("used eos stream");
    return st->s[st->offset];
}

bool stream_accept_c(struct stream *st, char c, struct position *pos) {
    if (stream_eos(st) || st->s[st->offset] != c) return false;
    stream_advance(st, pos);
    return true;
}

bool stream_accept_s(struct stream *st, const char *s, struct position *pos) {
    size_t row = st->row;
    size_t col = st->col;
    size_t offset = st->offset;
    struct position curr = *pos;
    for (size_t i = 0; s[i]; i++) {
        if (stream_eos(st) || st->s[st->offset] != s[i]) {
            st->row = row;
            st->col = col;
            st->offset = offset;
            return false;
        } else {
            stream_advance(st, &curr);
        }
    }
    *pos = curr;
    return true;
}
