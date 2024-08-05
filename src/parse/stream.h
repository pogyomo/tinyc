// Copyrignts (C) 2024 pogyomo. Released under the MIT license.

#ifndef TINYC_PARSE_STREAM_H_
#define TINYC_PARSE_STREAM_H_

#include <stdbool.h>

#include "../context.h"
#include "../lex/token.h"

// A helper macro that returns `ret_val` when `cond` is false.
#define TRY(cond)         \
    do {                  \
        if (!(cond)) {    \
            return false; \
        }                 \
    } while (0)

// A helper struct to iterate over tokens.
struct tstream {
    struct token *curr;
    struct token *last;  // Most recently used token. Must be non-NULL.
};

// Initialize `tstream`. Should be called before use it.
// `tokens` must be non-NULL.
void tstream_init(struct tstream *ts, struct token *tokens);

// Returns true if no token available.
bool tstream_eos(struct tstream *ts);

// Returns currently peeking token.
// Cause error if `tstream_eos` returns true.
struct token *tstream_curr(struct tstream *ts);

// Returns most recently used token.
// This function always returns non-NULL pointer.
struct token *tstream_last(struct tstream *ts);

// Advance stream so next token available.
// Nothing happen if `tstream_eos` returns true.
void tstream_advance(struct tstream *ts);

// Checks if `ts` is not reaches to eos.
// Returns false if the assertion failed, and then report error.
bool tstream_check_eos(struct context *ctx, struct tstream *ts);

// Checks if current token is punctuation and the kind is `kind`.
// Returns false if the assertion failed, and then report error.
bool tstream_check_punct(struct context *ctx, struct tstream *ts,
                         enum token_punct_kind kind);

// Checks if current token is keyword and the kind is `kind`.
// Returns false if the assertion failed, and then report error.
bool tstream_check_keyword(struct context *ctx, struct tstream *ts,
                           enum token_keyword_kind kind);

// Checks if current token is integer.
// Returns false if the assertion failed, and then report error.
bool tstream_check_int(struct context *ctx, struct tstream *ts);

// Checks if current token is floating number.
// Returns false if the assertion failed, and then report error.
bool tstream_check_float(struct context *ctx, struct tstream *ts);

// Checks if current token is identifier.
// Returns false if the assertion failed, and then report error.
bool tstream_check_ident(struct context *ctx, struct tstream *ts);

// Checks if current token is string.
// Returns false if the assertion failed, and then report error.
bool tstream_check_string(struct context *ctx, struct tstream *ts);

// Checks if current token is character.
// Returns false if the assertion failed, and then report error.
bool tstream_check_char(struct context *ctx, struct tstream *ts);

// Checks if current token is punctuation and the kind is `kind`.
// Returns false if the assertion failed.
// Unlike `tstream_check_punct`, this does nothing other than check.
bool tstream_is_punct(struct tstream *ts, enum token_punct_kind kind);

// Checks if current token is keyword and the kind is `kind`.
// Returns false if the assertion failed.
// Unlike `tstream_check_keyword`, this does nothing other than check.
bool tstream_is_keyword(struct tstream *ts, enum token_keyword_kind kind);

// Checks if current token is integer.
// Returns false if the assertion failed.
// Unlike `tstream_check_int`, this does nothing other than check.
bool tstream_is_int(struct tstream *ts);

// Checks if current token is floating number.
// Returns false if the assertion failed.
// Unlike `tstream_check_float`, this does nothing other than check.
bool tstream_is_float(struct tstream *ts);

// Checks if current token is identifier.
// Returns false if the assertion failed.
// Unlike `tstream_check_ident`, this does nothing other than check.
bool tstream_is_ident(struct tstream *ts);

// Checks if current token is string.
// Returns false if the assertion failed.
// Unlike `tstream_check_string`, this does nothing other than check.
bool tstream_is_string(struct tstream *ts);

// Checks if current token is character.
// Returns false if the assertion failed.
// Unlike `tstream_check_char`, this does nothing other than check.
bool tstream_is_char(struct tstream *ts);

#endif  // TINYC_PARSE_STREAM_H_
