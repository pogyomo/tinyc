#ifndef TINYC_PREPROCESSOR_PREPROCESSOR_H_
#define TINYC_PREPROCESSOR_PREPROCESSOR_H_

#include "../context.h"
#include "../lexer/stream.h"

namespace tinyc {

TokenStream preprocess(Context& ctx, TokenStream& ts);

}  // namespace tinyc

#endif  // TINYC_PREPROCESS_PREPROCESS_H_
