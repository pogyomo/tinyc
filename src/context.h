#ifndef TINYC_CONTEXT_H_
#define TINYC_CONTEXT_H_

#include "input/cache.h"
#include "preprocessor/macro.h"

namespace tinyc {

// A collection of data structure which is used while the compile process.
class Context {
public:
    Context() : input_cache_(), macro_table_() {}

    InputCache& input_cache() { return input_cache_; }

    MacroTable& macro_table() { return macro_table_; }

private:
    InputCache input_cache_;
    MacroTable macro_table_;
};

}  // namespace tinyc

#endif  // TINYC_CONTEXT_H_
