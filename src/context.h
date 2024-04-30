#ifndef TINYC_CONTEXT_H_
#define TINYC_CONTEXT_H_

#include "input/cache.h"

namespace tinyc {

// A collection of data structure which is used while the compile process.
class Context {
public:
    InputCache& input_cache() { return input_cache_; }

private:
    InputCache input_cache_;
};

}  // namespace tinyc

#endif  // TINYC_CONTEXT_H_
