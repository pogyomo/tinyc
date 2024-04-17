#ifndef TINYC_INPUT_INPUT_H_
#define TINYC_INPUT_INPUT_H_

#include <istream>
#include <string>
#include <utility>
#include <vector>

namespace tinyc {

// A class represent a input from input stream.
class Input {
public:
    // Construct a new `Input` from given `is` input stream.
    Input(std::istream& is) : row_(0), offset_(0) {
        std::string line;
        while (std::getline(is, line, '\n')) {
            lines_.push_back(line);
        }
    }

    // Returns current peeking character of this input.
    // It's undefined behavior to call this when `empty()` returns true.
    inline char ch() const { return eoi() ? '\0' : lines_[row_][offset_]; }

    // Returns pair of current peeking character's row and offset.
    inline std::pair<int, int> pos() const { return {row_, offset_}; }

    // Returns current peeking character's row in this input.
    inline int row() const { return row_; }

    // Returns current peeking character's offset in this input.
    inline int offset() const { return offset_; }

    // Returns true if no character remain in this input.
    inline bool eoi() const { return row_ >= lines_.size(); }

    // Advance the position in this input.
    // Nothing happen if `empty()` returns true.
    inline void advance() {
        if (eoi()) {
            return;
        }
        offset_++;

        // Ignore empty lines.
        while (row_ < lines_.size() && offset_ >= lines_[row_].size()) {
            offset_ = 0;
            row_++;
        }
    }

    // Reset position and go back to start.
    inline void reset() {
        row_ = 0;
        offset_ = 0;
    }

private:
    int row_;
    int offset_;
    std::vector<std::string> lines_;
};

}  // namespace tinyc

#endif  // TINYC_INPUT_INPUT_H_
