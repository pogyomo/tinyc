#ifndef TINYC_PARSER_DECL_H_
#define TINYC_PARSER_DECL_H_

#include "../context.h"
#include "../stream.h"
#include "ast.h"

// Parse given `ts` as statement and initialize `decl` with it.
// Returns false if error happen.
bool parse_decl(context_t *ctx, tstream_t *ts, decl_t *decl);

// Parse given `ts` as variable declaration by assuming its base type is `base`
// and initialize `decl` with it.
// Returns false if error happen.
bool parse_var_decl(context_t *ctx, tstream_t *ts, type_t *base, decl_t *decl);

// Parse given `ts` as function declaration by assuming its base type is `base`
// and initialize `decl` with it.
// Returns false if error happen.
bool parse_fun_decl(context_t *ctx, tstream_t *ts, type_t *base, decl_t *decl);

#endif  // TINYC_PARSER_DECL_H_
