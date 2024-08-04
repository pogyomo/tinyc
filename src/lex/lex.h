// Copyrignts (C) 2024 pogyomo. Released under the MIT license.

#ifndef TINYC_LEX_LEX_H_
#define TINYC_LEX_LEX_H_

#include <stdbool.h>

#include "../context.h"
#include "token.h"

// Convert given file into list of tokens, then store it to `tokens`. `*tokens`
// will be NULL if no tokne generated.
// This function returns false if and only if the conversion failed.
bool lex_file(struct context *ctx, const char *path, struct token **tokens);

#endif  // TINYC_LEX_LEX_H_
