#ifndef TINYC_INPUT_INPUT_H_
#define TINYC_INPUT_INPUT_H_

#include <istream>
#include <string>
#include <vector>

namespace tinyc {

// A class represent a input of this compiler.
class Input {
public:
    Input(std::istream& is, int id, const std::string& name);

    // Returns its unique id.
    inline const int id() const { return id_; }

    // Returns this input's name.
    inline const std::string& name() const { return name_; }

    // Returns lines which represent this input.
    inline const std::vector<std::string>& lines() const { return lines_; }

private:
    const int id_;
    const std::string name_;
    const std::vector<std::string> lines_;
};

}  // namespace tinyc

#endif  // TINYC_INPUT_INPUT_H_
