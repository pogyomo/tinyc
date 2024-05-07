#include "cache.h"

#include <sstream>
#include <stdexcept>
#include <string>

#include "input.h"

namespace tinyc {

int InputCache::cache(std::istream& is, const std::string& name) {
    int id = cached_.size();
    cached_.push_back(Input(is, id, name));
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
