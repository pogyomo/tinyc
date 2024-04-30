#include "stream.h"

namespace tinyc {

const std::shared_ptr<Token>& TokenStream::token() const {
    if (eos() || empty()) {
        throw std::out_of_range("");
    } else {
        return ts_[pos_];
    }
}

const std::shared_ptr<Token>& TokenStream::last() const {
    if (eos() || empty()) {
        throw std::out_of_range("");
    } else {
        return ts_.back();
    }
}

void TokenStream::pop_state() {
    if (!states_.empty()) {
        pos_ = states_.top();
        states_.pop();
    }
}

bool TokenStream::advance() {
    if (!eos()) {
        pos_++;
        return true;
    } else {
        return false;
    }
}

bool TokenStream::retrest() {
    if (pos_ > 0) {
        pos_--;
        return true;
    } else {
        return false;
    }
}

}  // namespace tinyc
