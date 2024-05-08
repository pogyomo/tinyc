#ifndef TINYC_PREPROCESSOR_PREPROCESSOR_H_
#define TINYC_PREPROCESSOR_PREPROCESSOR_H_

#include <optional>

#include "../context.h"
#include "../lexer/stream.h"

namespace tinyc {

// Process with directive in `ts` and return preprocessed token stream.
// If error happne, report error and return nullopt.
std::optional<TokenStream> preprocess(Context& ctx, TokenStream&& ts);

}  // namespace tinyc

#endif  // TINYC_PREPROCESS_PREPROCESS_H_
