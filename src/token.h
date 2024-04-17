#ifndef TINYC_TOKEN_H_
#define TINYC_TOKEN_H_

#include <string>

#include "span.h"

namespace tinyc {

enum class TokenKind {
    // Operators
    Or,            // ||
    And,           // &&
    Vertical,      // |
    Hat,           // ^
    Ampersand,     // &
    EQ,            // ==
    NE,            // !=
    LT,            // <
    GT,            // >
    LE,            // <=
    GE,            // >=
    LShift,        // <<
    RShift,        // >>
    Plus,          // +
    PlusPlus,      // ++
    Minus,         // -
    MinusMinus,    // --
    Star,          // *
    Slash,         // /
    Percent,       // %
    LParen,        // (
    RParen,        // )
    LCurly,        // {
    RCurly,        // }
    LSquare,       // [
    RSquare,       // ]
    Semicolon,     // ;
    Colon,         // :
    Comma,         // ,
    Tilde,         // ~
    Not,           // !
    Assign,        // =
    OrAssign,      // |=
    XorAssign,     // ^=
    AndAssign,     // &=
    LShiftAssign,  // <<=
    RShiftAssign,  // >>=
    AddAssign,     // +=
    SubAssign,     // -=
    MulAssign,     // *=
    DivAssign,     // /=
    ModAssign,     // %=

    // Literals
    Integer,     // 10, -2, ...
    Identifier,  // ident, Ident_20, ...
};

// A base class of token which hold its kind and span in source code.
class Token {
public:
    Token(TokenKind kind, Span span) : kind_(kind), span_(span) {}
    virtual ~Token() {}
    virtual std::string debug() const = 0;
    inline const TokenKind& kind() const { return kind_; }
    inline const Span& span() const { return span_; }

private:
    const TokenKind kind_;
    const Span span_;
};

// A class for symbol like +, <<, etc.
class SymbolToken : public Token {
public:
    SymbolToken(TokenKind kind, Span span) : Token(kind, span) {}
    std::string debug() const override;
};

// A class for literal like 10, ident, etc.
template <class T>
class ValueToken : public Token {
public:
    ValueToken(TokenKind kind, T value, Span span)
        : Token(kind, span), value_(value) {}
    inline const T& value() const { return value_; }
    std::string debug() const override;

private:
    const T value_;
};

}  // namespace tinyc

#endif  // TINYC_TOKEN_H_
