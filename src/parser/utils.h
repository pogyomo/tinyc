#ifndef TINYC_PARSER_UTILS_H_
#define TINYC_PARSER_UTILS_H_

#include "../context.h"
#include "../stream.h"

// Check `expr` is true and returns from the function this macro involved with
// false if `expr` is false.
#define TRY(expr)         \
    do {                  \
        if (!(expr)) {    \
            return false; \
        }                 \
    } while (0)

// Check if token exists in `ts` and returns true if token exists.
// Returns false if `ts` is eos and then report error.
bool check_eos(context_t *ctx, tstream_t *ts);

// Check if current token of `ts` is `kind` and returns true if such.
// Returns false if not and then report error.
bool check_kind(context_t *ctx, tstream_t *ts, token_kind_t kind);

// Check if current token of `ts` is one of `kinds` and returns true if such.
// Returns false if not and then report error.
bool check_kinds(context_t *ctx, tstream_t *ts, size_t n, ...);

// Check if current token of `ts` is `kind`, then returns true and advance `ts`
// if such.
// Returns false if not and then report error.
bool expect_kind(context_t *ctx, tstream_t *ts, token_kind_t kind,
                 span_t *span);

#endif  // TINYC_PARSER_UTILS_H_
