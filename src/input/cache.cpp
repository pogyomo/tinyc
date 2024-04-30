#include "cache.h"

#include <sstream>
#include <stdexcept>

#include "input.h"

namespace tinyc {

int InputCache::cache(std::istream& is) {
    int id = cached_.size();
    cached_.push_back(Input(is, id));
    return id;
}

const Input& InputCache::fetch(int id) {
    if (id >= cached_.size()) {
        std::stringstream ss;
        ss << "unknown id `" << id << "` was passed to `fetch`";
        throw std::out_of_range(ss.str());
    } else {
        return cached_[id];
    }
}

}  // namespace tinyc
