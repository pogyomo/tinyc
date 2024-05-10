#ifndef TINYC_LEXER_TOKEN_H_
#define TINYC_LEXER_TOKEN_H_

#include <string>

#include "../span.h"

namespace tinyc {

enum class TokenKind {
    // Operators
    Or = 1,        // ||
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
    Question,      // ?
    Sharp,         // #
    Dot,           // .
    Arrow,         // ->
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

    // Keywords
    Auto,      // auto
    Break,     // break
    Case,      // case
    Char,      // char
    Const,     // const
    Continue,  // continue
    Default,   // default
    Do,        // do
    Double,    // double
    Else,      // else
    Enum,      // enum
    Extern,    // extern
    Float,     // float
    For,       // for
    Goto,      // goto
    If,        // if
    Inline,    // inline
    Int,       // int
    Long,      // long
    Register,  // register
    Restrict,  // restrict
    Return,    // return
    Short,     // short
    Signed,    // signed
    Sizeof,    // sizeof
    Static,    // static
    Struct,    // struct
    Switch,    // switch
    Typedef,   // typedef
    Union,     // union
    Unsigned,  // unsigned
    Void,      // void
    Volatile,  // volatile
    While,     // while

    // Literals
    Integer,     // 10, -2, ...
    Identifier,  // ident, Ident_20, ...
    String,      // "hello world!", ...
    Character,   // 'a', 'A', ...
};

// A base class of token which hold its kind and span in source code.
class Token {
public:
    Token(TokenKind kind, Span span, int lrow)
        : kind_(kind), span_(span), lrow_(lrow) {}
    virtual ~Token() {}
    virtual std::string debug() const = 0;
    inline const TokenKind& kind() const { return kind_; }
    inline const Span& span() const { return span_; }
    inline const int lrow() const { return lrow_; }

private:
    const TokenKind kind_;
    const Span span_;
    int lrow_;
};

// A class for symbol like +, <<, etc.
class SymbolToken : public Token {
public:
    SymbolToken(TokenKind kind, Span span, int lrow)
        : Token(kind, span, lrow) {}
    std::string debug() const override;
};

// A class for keyword like int, long, etc.
class KeywordToken : public Token {
public:
    KeywordToken(TokenKind kind, Span span, int lrow)
        : Token(kind, span, lrow) {}
    std::string debug() const override;
};

// A class for literal like 10, ident, "str", etc.
template <class T>
class ValueToken : public Token {
public:
    ValueToken(TokenKind kind, T value, Span span, int lrow)
        : Token(kind, span, lrow), value_(value) {}
    inline const T& value() const { return value_; }
    std::string debug() const override;

private:
    const T value_;
};

}  // namespace tinyc

#endif  // TINYC_TOKEN_H_
