#include "stream.h"

#include <stdexcept>

#include "../span.h"

namespace tinyc {

char InputStream::ch() const {
    if (eos()) {
        throw std::out_of_range("`ch()` called when `eos()` returns true");
    } else {
        return input_.lines()[row_][offset_];
    }
}

void InputStream::advance() {
    if (eos()) {
        return;
    }

    auto lines = input_.lines();
    offset_++;
    if (offset_ == lines[row_].size()) {
        offset_ = 0;
        row_++;
        while (row_ < lines.size() && lines[row_].empty()) {
            row_++;
        }
    }
}

bool InputStream::accept(char c, Span& span) {
    if (eos() || ch() != c) {
        return false;
    }
    span = Span(input_.id(), pos(), pos());
    advance();
    return true;
}

bool InputStream::accept(const std::string& s, Span& span) {
    int row = row_;
    int offset = offset_;
    Position start = pos();
    Position end = pos();
    for (const auto c : s) {
        if (eos() || ch() != c) {
            row_ = row;
            offset_ = offset;
            return false;
        }
        end = pos();
        advance();
    }
    span = Span(input_.id(), start, end);
    return true;
}

}  // namespace tinyc
