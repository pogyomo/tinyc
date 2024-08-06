// Copyrignts (C) 2024 pogyomo. Released under the MIT license.

#ifndef TINYC_PARSE_STMT_H_
#define TINYC_PARSE_STMT_H_

#include <stdbool.h>

#include "ast/stmt.h"
#include "context.h"
#include "stream.h"

// Parse `ts` as statement, then initialize `*stmt` with it.
// Returns false if error happen.
bool parse_stmt(struct parse_context *ctx, struct tstream *ts,
                struct stmt **stmt);

// Parse `ts` as label-statement, then initialize `*stmt` with it.
// Returns false if error happen.
bool parse_label_stmt(struct parse_context *ctx, struct tstream *ts,
                      struct stmt **stmt);

// Parse `ts` as case-statement, then initialize `*stmt` with it.
// Returns false if error happen.
bool parse_case_stmt(struct parse_context *ctx, struct tstream *ts,
                     struct stmt **stmt);

// Parse `ts` as default-statement, then initialize `*stmt` with it.
// Returns false if error happen.
bool parse_default_stmt(struct parse_context *ctx, struct tstream *ts,
                        struct stmt **stmt);

// Parse `ts` as block-statement, then initialize `*stmt` with it.
// Returns false if error happen.
bool parse_block_stmt(struct parse_context *ctx, struct tstream *ts,
                      struct stmt **stmt);

// Parse `ts` as expr-statement, then initialize `*stmt` with it.
// Returns false if error happen.
bool parse_expr_stmt(struct parse_context *ctx, struct tstream *ts,
                     struct stmt **stmt);

// Parse `ts` as if-statement, then initialize `*stmt` with it.
// Returns false if error happen.
bool parse_if_stmt(struct parse_context *ctx, struct tstream *ts,
                   struct stmt **stmt);

// Parse `ts` as switch-statement, then initialize `*stmt` with it.
// Returns false if error happen.
bool parse_switch_stmt(struct parse_context *ctx, struct tstream *ts,
                       struct stmt **stmt);

// Parse `ts` as while-statement, then initialize `*stmt` with it.
// Returns false if error happen.
bool parse_while_stmt(struct parse_context *ctx, struct tstream *ts,
                      struct stmt **stmt);

// Parse `ts` as do-while-statement, then initialize `*stmt` with it.
// Returns false if error happen.
bool parse_do_while_stmt(struct parse_context *ctx, struct tstream *ts,
                         struct stmt **stmt);

// Parse `ts` as for-statement, then initialize `*stmt` with it.
// Returns false if error happen.
bool parse_for_stmt(struct parse_context *ctx, struct tstream *ts,
                    struct stmt **stmt);

// Parse `ts` as goto-statement, then initialize `*stmt` with it.
// Returns false if error happen.
bool parse_goto_stmt(struct parse_context *ctx, struct tstream *ts,
                     struct stmt **stmt);

// Parse `ts` as continue-statement, then initialize `*stmt` with it.
// Returns false if error happen.
bool parse_continue_stmt(struct parse_context *ctx, struct tstream *ts,
                         struct stmt **stmt);

// Parse `ts` as break-statement, then initialize `*stmt` with it.
// Returns false if error happen.
bool parse_break_stmt(struct parse_context *ctx, struct tstream *ts,
                      struct stmt **stmt);

// Parse `ts` as return-statement, then initialize `*stmt` with it.
// Returns false if error happen.
bool parse_return_stmt(struct parse_context *ctx, struct tstream *ts,
                       struct stmt **stmt);

#endif  // TINYC_PARSE_STMT_H_
