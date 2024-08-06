// Copyrignts (C) 2024 pogyomo. Released under the MIT license.

#ifndef TINYC_PARSE_DECL_H_
#define TINYC_PARSE_DECL_H_

#include <stdbool.h>

#include "ast/decl.h"
#include "context.h"
#include "stream.h"

// Parse `ts` as initializer, then initialize `*init` with it.
// Returns false if error happen.
bool parse_initializer(struct parse_context *ctx, struct tstream *ts,
                       struct initializer **init);

// Parse `ts` as declaration, then initialize `*decl` with it.
// Returns false if error happen.
bool parse_decl(struct parse_context *ctx, struct tstream *ts,
                struct decl **decl);

#endif  // TINYC_PARSE_DECL_H_
