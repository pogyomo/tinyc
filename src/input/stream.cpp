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

Position InputStream::pos() const {
    if (eos()) {
        throw std::out_of_range("`pos()` called when `eos()` returns true");
    } else {
        return {row_, offset_};
    }
}

int InputStream::lrow() const {
    if (eos()) {
        throw std::out_of_range("`lrow()` called when `eos()` returns true");
    } else {
        return lrow_;
    }
}

void InputStream::advance() {
    if (eos()) {
        return;
    }

    auto cont_line = false;
    auto lines = input_.lines();
    offset_++;
    if (offset_ + 1 == lines[row_].size() && ch() == '\\') {
        offset_++;
        cont_line = true;
    }
    if (offset_ == lines[row_].size()) {
        if (!cont_line) {
            lrow_++;
        }
        row_++;
        offset_ = 0;
        while (row_ < lines.size() && lines[row_].empty()) {
            lrow_++;
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

bool InputStream::accept(char c) {
    Span span(0);
    return accept(c, span);
}

bool InputStream::accept(const std::string& s, Span& span) {
    if (eos()) {
        return false;
    }

    int lrow = lrow_;
    int row = row_;
    int offset = offset_;

    Position start = pos();
    Position end = pos();
    for (const auto c : s) {
        if (eos() || ch() != c) {
            lrow_ = lrow;
            row_ = row;
            offset_ = offset;
            return false;
        }
        end = pos();
        advance();
    }

    if (lrow_ != lrow) {
        lrow_ = lrow;
        row_ = row;
        offset_ = offset;
        return false;
    } else {
        span = Span(input_.id(), start, end);
        return true;
    }
}

bool InputStream::accept(const std::string& s) {
    Span span(0);
    return accept(s, span);
}

}  // namespace tinyc
