#ifndef TINYC_INPUT_CACHE_H_
#define TINYC_INPUT_CACHE_H_

#include <istream>
#include <string>
#include <vector>

#include "input.h"

namespace tinyc {

class InputCache {
public:
    InputCache() : cached_() {}

    // Cache input by creating it from `is`, then returns unique id for it.
    int cache(std::istream& is, const std::string& name);

    // Fetch the input associated with the `id`.
    // If no such input exist, throw `out_of_range` exception.
    const Input& fetch(int id);

private:
    std::vector<Input> cached_;
};

}  // namespace tinyc

#endif  // TINYC_INPUT_CACHE_H_
