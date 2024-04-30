#ifndef TINYC_INPUT_STREAM_H_
#define TINYC_INPUT_STREAM_H_

#include <string>

#include "../span.h"
#include "input.h"

namespace tinyc {

// A helper class for iterating `Input`.
class InputStream {
public:
    InputStream(const Input& input) : row_(0), offset_(0), input_(input) {}

    // Returns the input this stream holding.
    inline const Input& input() const { return input_; }

    // Returns true if no character is available from this stream.
    inline bool eos() const { return row_ >= input_.lines().size(); }

    // Returns currently peeking character.
    // Calling this when `eos()` returns true, throw `out_of_range` exception.
    char ch() const;

    // Returns currently peeking character's position in source code.
    // Calling this when `eos()` returns true, throw `out_of_range` exception.
    inline Position pos() const { return {row_, offset_}; }

    // Advance the position of this stream to peek next character.
    // Calling this when `eos()` returns true, nothing happen.
    void advance();

    // If first character this stream peeking is `c`, advance this stream, then
    // returns true and set `span` to the span of accepted character.
    bool accept(char c, Span& span);

    // If first characters this stream peeking is `s`, advance this stream, then
    // returns true and set `span` to the span of accepted characters.
    bool accept(const std::string& s, Span& span);

private:
    int row_;
    int offset_;
    const Input& input_;
};

}  // namespace tinyc

#endif  // TINYC_INPUT_STREAM_H_
