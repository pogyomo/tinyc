// Copyrignts (C) 2024 pogyomo. Released under the MIT license.

#ifndef TINYC_PARSE_EXPR_H_
#define TINYC_PARSE_EXPR_H_

#include <stdbool.h>

#include "ast.h"
#include "context.h"
#include "stream.h"

// Parse `ts` as expression, then initialize `*expr` with it.
// Returns false if error happen.
bool parse_expr(struct parse_context *ctx, struct tstream *ts,
                struct expr **expr);

// Parse `ts` as assignment-expression, then initialize `*expr` with it.
// Returns false if error happen.
bool parse_assign_expr(struct parse_context *ctx, struct tstream *ts,
                       struct expr **expr);

// Parse `ts` as conditional-expression, then initialize `*expr` with it.
// Returns false if error happen.
bool parse_cond_expr(struct parse_context *ctx, struct tstream *ts,
                     struct expr **expr);

// Parse `ts` as logical-or-expression, then initialize `*expr` with it.
// Returns false if error happen.
bool parse_logical_or_expr(struct parse_context *ctx, struct tstream *ts,
                           struct expr **expr);

// Parse `ts` as logical-and-expression, then initialize `*expr` with it.
// Returns false if error happen.
bool parse_logical_and_expr(struct parse_context *ctx, struct tstream *ts,
                            struct expr **expr);

// Parse `ts` as inclusive-or-expression, then initialize `*expr` with it.
// Returns false if error happen.
bool parse_inclusive_or_expr(struct parse_context *ctx, struct tstream *ts,
                             struct expr **expr);

// Parse `ts` as exclusive-or-expression, then initialize `*expr` with it.
// Returns false if error happen.
bool parse_exclusive_or_expr(struct parse_context *ctx, struct tstream *ts,
                             struct expr **expr);

// Parse `ts` as and-expression, then initialize `*expr` with it.
// Returns false if error happen.
bool parse_and_expr(struct parse_context *ctx, struct tstream *ts,
                    struct expr **expr);

// Parse `ts` as equality-expression, then initialize `*expr` with it.
// Returns false if error happen.
bool parse_equality_expr(struct parse_context *ctx, struct tstream *ts,
                         struct expr **expr);

// Parse `ts` as relational-expression, then initialize `*expr` with it.
// Returns false if error happen.
bool parse_relational_expr(struct parse_context *ctx, struct tstream *ts,
                           struct expr **expr);

// Parse `ts` as shift-expression, then initialize `*expr` with it.
// Returns false if error happen.
bool parse_shift_expr(struct parse_context *ctx, struct tstream *ts,
                      struct expr **expr);

// Parse `ts` as additive-expression, then initialize `*expr` with it.
// Returns false if error happen.
bool parse_additive_expr(struct parse_context *ctx, struct tstream *ts,
                         struct expr **expr);

// Parse `ts` as multiplicative-expression, then initialize `*expr` with it.
// Returns false if error happen.
bool parse_multiplicative_expr(struct parse_context *ctx, struct tstream *ts,
                               struct expr **expr);

// Parse `ts` as cast-expression, then initialize `*expr` with it.
// Returns false if error happen.
bool parse_cast_expr(struct parse_context *ctx, struct tstream *ts,
                     struct expr **expr);

// Parse `ts` as unary-expression, then initialize `*expr` with it.
// Returns false if error happen.
bool parse_unary_expr(struct parse_context *ctx, struct tstream *ts,
                      struct expr **expr);

// Parse `ts` as postfix-expression, then initialize `*expr` with it.
// Returns false if error happen.
bool parse_postfix_expr(struct parse_context *ctx, struct tstream *ts,
                        struct expr **expr);

// Parse `ts` as primary-expression, then initialize `*expr` with it.
// Returns false if error happen.
bool parse_primary_expr(struct parse_context *ctx, struct tstream *ts,
                        struct expr **expr);

#endif  // TINYC_PARSE_EXPR_H_
