#include "span.h"

#include "report/panic.h"

position_t min(position_t a, position_t b) {
    if (a.row < b.row) {
        return a;
    } else if (b.row < a.row) {
        return b;
    } else {
        if (a.offset < b.offset) {
            return a;
        } else {
            return b;
        }
    }
}

position_t max(position_t a, position_t b) {
    if (a.row > b.row) {
        return a;
    } else if (b.row > a.row) {
        return b;
    } else {
        if (a.offset > b.offset) {
            return a;
        } else {
            return b;
        }
    }
}

span_t concat_span(span_t a, span_t b) {
    if (a.id != b.id) panic("concat span with different id");
    span_t span = {a.id, min(a.start, b.start), max(a.end, b.end)};
    return span;
}
