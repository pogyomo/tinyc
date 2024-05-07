#ifndef TINYC_LEXER_STREAM_H_
#define TINYC_LEXER_STREAM_H_

#include <memory>
#include <stdexcept>
#include <vector>

#include "token.h"

namespace tinyc {

class TokenStream {
public:
    using state_t = int;

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

    // Get internal state to be used in `set_state`.
    inline state_t get_state() { return pos_; }

    // Set internal state to `state`.
    inline void set_state(state_t state) { pos_ = state; }

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
    const std::vector<std::shared_ptr<Token>> ts_;
};

}  // namespace tinyc

#endif  // TINYC_LEXER_STREAM_H_
