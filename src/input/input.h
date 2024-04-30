#ifndef TINYC_INPUT_INPUT_H_
#define TINYC_INPUT_INPUT_H_

#include <istream>
#include <string>
#include <vector>

namespace tinyc {

// A class represent a input of this compiler.
class Input {
public:
    Input(std::istream& is, int id);

    // Returns its unique id.
    inline const int id() const { return id_; }

    // Returns lines which represent this input.
    inline const std::vector<std::string>& lines() const { return lines_; }

private:
    const int id_;
    const std::vector<std::string> lines_;
};

}  // namespace tinyc

#endif  // TINYC_INPUT_INPUT_H_
