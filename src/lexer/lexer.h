#ifndef TINYC_LEXER_LEXER_H_
#define TINYC_LEXER_LEXER_H_

#include <istream>
#include <optional>
#include <vector>

#include "../context.h"
#include "error.h"
#include "stream.h"

namespace tinyc {

// Convert the characters in `is` into the list of token.
// If error happne, report error and return nullopt.
std::optional<TokenStream> lex(Context& ctx, std::istream& is,
                               const std::string& name);

}  // namespace tinyc

#endif  // TINYC_LEXER_LEXER_H_
