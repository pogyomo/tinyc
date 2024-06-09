#ifndef TINYC_PARSER_EXPR_H_
#define TINYC_PARSER_EXPR_H_

#include "../context.h"
#include "../stream.h"
#include "ast.h"

// Parse given `ts` as expression and initialize `expr` with it.
// Returns false if error happen.
bool parse_expr(context_t *ctx, tstream_t *ts, expr_t *expr);

// Parse given `ts` as assignment-expression and initialize `expr` with it.
// Returns false if error happen.
bool parse_assign_expr(context_t *ctx, tstream_t *ts, expr_t *expr);

// Parse given `ts` as constant expression and initialize `expr` with it.
// Returns false if error happen.
bool parse_const_expr(context_t *ctx, tstream_t *ts, expr_t *expr);

// Parse given `ts` as conditional-expression and initialize `expr` with it.
// Returns false if error happen.
bool parse_cond_expr(context_t *ctx, tstream_t *ts, expr_t *expr);

// Parse given `ts` as logical-or-expression and initialize `expr` with it.
// Returns false if error happen.
bool parse_logical_or_expr(context_t *ctx, tstream_t *ts, expr_t *expr);

// Parse given `ts` as logical-and-expression and initialize `expr` with it.
// Returns false if error happen.
bool parse_logical_and_expr(context_t *ctx, tstream_t *ts, expr_t *expr);

// Parse given `ts` as inclusize-or-expression and initialize `expr` with it.
// Returns false if error happen.
bool parse_or_expr(context_t *ctx, tstream_t *ts, expr_t *expr);

// Parse given `ts` as exclusive-or-expression and initialize `expr` with it.
// Returns false if error happen.
bool parse_xor_expr(context_t *ctx, tstream_t *ts, expr_t *expr);

// Parse given `ts` as and-expression and initialize `expr` with it.
// Returns false if error happen.
bool parse_and_expr(context_t *ctx, tstream_t *ts, expr_t *expr);

// Parse given `ts` as equality-expression and initialize `expr` with it.
// Returns false if error happen.
bool parse_equality_expr(context_t *ctx, tstream_t *ts, expr_t *expr);

// Parse given `ts` as relational-expression and initialize `expr` with it.
// Returns false if error happen.
bool parse_relational_expr(context_t *ctx, tstream_t *ts, expr_t *expr);

// Parse given `ts` as shift-expression and initialize `expr` with it.
// Returns false if error happen.
bool parse_shift_expr(context_t *ctx, tstream_t *ts, expr_t *expr);

// Parse given `ts` as additive-expression and initialize `expr` with it.
// Returns false if error happen.
bool parse_additive_expr(context_t *ctx, tstream_t *ts, expr_t *expr);

// Parse given `ts` as multiplicative-expression and initialize `expr` with it.
// Returns false if error happen.
bool parse_multiplicative_expr(context_t *ctx, tstream_t *ts, expr_t *expr);

// Parse given `ts` as cast-expression and initialize `expr` with it.
// Returns false if error happen.
bool parse_cast_expr(context_t *ctx, tstream_t *ts, expr_t *expr);

// Parse given `ts` as unary-expression and initialize `expr` with it.
// Returns false if error happen.
bool parse_unary_expr(context_t *ctx, tstream_t *ts, expr_t *expr);

// Parse given `ts` as postfix-expression and initialize `expr` with it.
// Returns false if error happen.
bool parse_postfix_expr(context_t *ctx, tstream_t *ts, expr_t *expr);

// Parse given `ts` as primary-expression and initialize `expr` with it.
// Returns false if error happen.
bool parse_primary_expr(context_t *ctx, tstream_t *ts, expr_t *expr);

#endif  // TINYC_PARSER_EXPR_H_
