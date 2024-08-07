#ifndef TINYC_PARSE_FUN_DEF_H_
#define TINYC_PARSE_FUN_DEF_H_

#include <stdbool.h>

#include "ast/fun_def.h"
#include "context.h"
#include "stream.h"

// Parse `ts` as function definition, and then update `def` with it.
// Returns false if error happen.
// `*fallback` will be true if error happen, but can be restart as declaration.
bool parse_fun_def(struct parse_context *ctx, struct tstream *ts,
                   struct fun_def **def, bool *fallback);

#endif  // TINYC_PARSE_FUN_DEF_H_
