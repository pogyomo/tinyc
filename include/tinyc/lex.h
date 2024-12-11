#ifndef TINYC_LEX_H_
#define TINYC_LEX_H_

#include "tinyc/token.h"

struct tinyc_lex {};

/// Extract a token, return pointer to it.
/// Returns NULL if no token available.
struct tinyc_token_header *tinyc_lex_once(struct tinyc_lex *lex);

#endif  // TINYC_LEX_H_
