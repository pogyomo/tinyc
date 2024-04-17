#ifndef TINYC_INPUT_CACHE_H_
#define TINYC_INPUT_CACHE_H_

#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "input.h"

namespace tinyc {

// A class which cache input with its name and can easily fetch it using unique
// id.
class InputCache {
public:
    using cacheid_t = int;

    // Construct a new empty `InputCache`.
    InputCache() : inputs_() {}

    // Create `Input` from given input stream and return unique id for it.
    cacheid_t cache(std::istream& is, const std::string& name) {
        Input input(is);
        cacheid_t id = inputs_.size();
        inputs_.push_back({input, name});
        name_to_id_[name] = id;
        return id;
    }

    // Fetch input correspond to `id`.
    // If no such input exist, throw `out_of_range` exception.
    Input& fetch_input(cacheid_t id) { return inputs_.at(id).first; }

    // Fetch name correspond to `id`.
    // If no such input exist, throw `out_of_range` exception.
    const std::string& fetch_name(cacheid_t id) const {
        return inputs_.at(id).second;
    }

    // Search unique id correspond to given name.
    // If no correspond id exist, throw `out_of_range` exception.
    cacheid_t id_from_name(const std::string& name) {
        return name_to_id_.at(name);
    }

private:
    std::map<std::string, cacheid_t> name_to_id_;
    std::vector<std::pair<Input, std::string>> inputs_;
};

}  // namespace tinyc

#endif  // TINYC_INPUT_CACHE_H_
