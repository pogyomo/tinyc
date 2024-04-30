#ifndef TINYC_LEXER_STREAM_H_
#define TINYC_LEXER_STREAM_H_

#include <memory>
#include <stack>
#include <stdexcept>
#include <vector>

#include "token.h"

namespace tinyc {

class TokenStream {
public:
    TokenStream() : pos_(0), ts_() {}

    TokenStream(const std::vector<std::shared_ptr<Token>>& ts)
        : pos_(0), ts_(ts) {}

    // Returns true if no token available from this stream.
    inline bool eos() const { return pos_ >= ts_.size(); }

    // Returns true if no token is exist in this stream.
    inline bool empty() const { return ts_.empty(); }

    // Returns current peeking token in this stream.
    // Calling this when `eos()` or `empty()` returns true, throw `out_of_range`
    // exception.
    const std::shared_ptr<Token>& token() const;

    // Returns last token in token stream.
    // Calling this when `eos()` or `empty()` returns true, throw `out_of_range`
    // exception.
    const std::shared_ptr<Token>& last() const;

    // Push current state of this stream into state stack.
    inline void push_state() { states_.push(pos_); }

    // Pop state of this stream from stack and set current state to this.
    // If state stack is empty, nothing happen.
    void pop_state();

    // Advance the position in this stream.
    // If `empty()` returns true, nothing happen and has no effect.
    // This returns true if it success to advance the position.
    bool advance();

    // Retrest the position in this stream.
    // If in the start of this stream, nothing happen and has no effect.
    // This returns true if it success to retrest the position.
    bool retrest();

private:
    int pos_;
    std::stack<int> states_;
    const std::vector<std::shared_ptr<Token>> ts_;
};

}  // namespace tinyc

#endif  // TINYC_LEXER_STREAM_H_
