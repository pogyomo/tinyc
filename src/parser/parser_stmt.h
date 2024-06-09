#ifndef TINYC_PARSER_STMT_H_
#define TINYC_PARSER_STMT_H_

#include "../context.h"
#include "../stream.h"
#include "ast.h"

// Parse given `ts` as statement and initialize `stmt` with it.
// Returns false if error happen.
bool parse_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt);

// Parse given `ts` as labeled-statement and initialize `stmt` with it.
// Returns false if error happen.
bool parse_labeled_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt);

// Parse given `ts` as case-statement and initialize `stmt` with it.
// Returns false if error happen.
bool parse_case_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt);

// Parse given `ts` as default-statement and initialize `stmt` with it.
// Returns false if error happen.
bool parse_default_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt);

// Parse given `ts` as expr-statement and initialize `stmt` with it.
// Returns false if error happen.
bool parse_expr_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt);

// Parse given `ts` as block-statement and initialize `stmt` with it.
// Returns false if error happen.
bool parse_block_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt);

// Parse given `ts` as if-statement and initialize `stmt` with it.
// Returns false if error happen.
bool parse_if_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt);

// Parse given `ts` as switch-statement and initialize `stmt` with it.
// Returns false if error happen.
bool parse_switch_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt);

// Parse given `ts` as while-statement and initialize `stmt` with it.
// Returns false if error happen.
bool parse_while_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt);

// Parse given `ts` as do-while-statement and initialize `stmt` with it.
// Returns false if error happen.
bool parse_do_while_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt);

// Parse given `ts` as for-statement and initialize `stmt` with it.
// Returns false if error happen.
bool parse_for_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt);

// Parse given `ts` as goto-statement and initialize `stmt` with it.
// Returns false if error happen.
bool parse_goto_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt);

// Parse given `ts` as continue-statement and initialize `stmt` with it.
// Returns false if error happen.
bool parse_continue_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt);

// Parse given `ts` as break-statement and initialize `stmt` with it.
// Returns false if error happen.
bool parse_break_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt);

// Parse given `ts` as return-statement and initialize `stmt` with it.
// Returns false if error happen.
bool parse_return_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt);

#endif  // TINYC_PARSER_STMT_H_
