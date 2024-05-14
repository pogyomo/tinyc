#ifndef TINYC_LEXER_LEXER_H_
#define TINYC_LEXER_LEXER_H_

#include "../context.h"
#include "stream.h"

// Convert given file's context into token stream.
tstream_t *lex_file(context_t *ctx, char *path);

#endif  // TINYC_LEXER_LEXER_H_
