#ifndef TINYC_LEXER_LEXER_H_
#define TINYC_LEXER_LEXER_H_

#include <exception>
#include <memory>
#include <vector>

#include "../input/input.h"
#include "../span.h"
#include "token.h"

namespace tinyc {

// A class represent an error happen during lexing.
class LexError : public std::exception {
public:
    // Construct a new `LexError` with message and its span.
    LexError(const std::string& msg, Span span) : msg_(msg), span_(span) {}

    // Returns error message.
    const std::string& msg() const { return msg_; }

    // Returns span where this error happen.
    const Span& span() const { return span_; }

private:
    std::string msg_;
    Span span_;
};

// A class represent a sequence of token which hold current position at the
// sequence and can advance/retrest it.
class TokenStream {
public:
    using state_t = int;

    // Construct a new empty `TokenStream`.
    TokenStream() : pos_(0), ts_() {}

    // Construct a new `TokenStream` with given list of token with initializing
    // its position to 0.
    TokenStream(const std::vector<std::shared_ptr<Token>>& ts)
        : pos_(0), ts_(ts) {}

    // Returns current peeking token in this stream.
    // It's undefined behavior to call this when `empty()` or `eos()` returns
    // true.
    inline const std::shared_ptr<Token>& token() const { return ts_[pos_]; }

    // Returns last token in token stream.
    // It's undefined behavior to call this when `eos()` returns true.
    inline const std::shared_ptr<Token>& last() { return ts_.back(); }

    // Returns current state.
    inline state_t state() const { return pos_; }

    // Set given state to this stream.
    inline void set_state(state_t state) { pos_ = state; }

    // Advance the position in this stream.
    // If `empty()` returns true, nothing happen and has no effect.
    inline void advance() {
        if (!eos()) {
            pos_++;
        }
    }

    // Retrest the position in this stream.
    // If in the start of this stream, nothing happen and has no effect.
    inline void retrest() {
        if (pos_ > 0) {
            pos_--;
        }
    }

    // Returns true if no token available from this stream.
    inline bool eos() { return pos_ >= ts_.size(); }

    // Returns true if no token is exist in this stream.
    inline bool empty() { return ts_.empty(); }

private:
    int pos_;
    const std::vector<std::shared_ptr<Token>> ts_;
};

// Convert given input into token stream.
// Errors happen during lexing is stored into `errors`.
TokenStream lex(Input& input, std::vector<LexError>& es);

}  // namespace tinyc

#endif  // TINYC_LEXER_LEXER_H_
