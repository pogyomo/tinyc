#ifndef TINYC_LEXER_LEXER_H_
#define TINYC_LEXER_LEXER_H_

#include <stdbool.h>

#include "../context.h"

// Lex file in `path`, preprocess for it, and initialize `tokens` with it.
// Returns true if success, or false when error happen.
bool lex_file(context_t *ctx, char *path, VECTOR_REF(token_t) tokens);

#endif  // TINYC_LEXER_LEXER_H_
