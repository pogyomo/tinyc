#ifndef TINYC_LEXER_LEXER_H_
#define TINYC_LEXER_LEXER_H_

#include <istream>
#include <vector>

#include "../context.h"
#include "error.h"
#include "stream.h"

namespace tinyc {

// Convert the characters in `is` into the list of token.
// If error happne, `es` will contains the errors.
TokenStream lex(Context& ctx, std::istream& is, std::vector<LexError>& es);

}  // namespace tinyc

#endif  // TINYC_LEXER_LEXER_H_
