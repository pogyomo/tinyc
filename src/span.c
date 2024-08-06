#include "span.h"

#include "panic.h"

// Compare `lhs` and `rhs`, and returns the result.
// - `lhs` < `rhs`, then returns < 0.
// - `lhs` > `rhs`, then returns > 0.
// - Otherwise returns == 0.
static int cmp_position(const struct position *lhs,
                        const struct position *rhs) {
    if (lhs->row < rhs->row) {
        return -1;
    } else if (lhs->row > rhs->row) {
        return 1;
    } else {
        if (lhs->col < rhs->col) {
            return -1;
        } else if (lhs->col > rhs->col) {
            return 1;
        } else {
            return 0;
        }
    }
}

void merge_span(const struct span *lhs, const struct span *rhs,
                struct span *merged) {
    if (lhs->id != rhs->id)
        fatal_error("cache id mismatched: %zu != %zu", lhs->id, rhs->id);
    int cmp_start = cmp_position(&lhs->start, &rhs->start);
    int cmp_end = cmp_position(&lhs->end, &rhs->end);
    merged->start = cmp_start <= 0 ? lhs->start : rhs->start;
    merged->end = cmp_end >= 0 ? lhs->end : rhs->end;
}
