#ifndef TINYC_PARSER_NODE_H_
#define TINYC_PARSER_NODE_H_

#include <string>

#include "../span.h"

namespace tinyc {

// An abstract class for all element in ast.
class Node {
public:
    virtual ~Node() {}
    virtual Span span() const = 0;
    virtual std::string debug() const = 0;
};

class LParen : public Node {
public:
    LParen(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "("; }

private:
    const Span span_;
};

class RParen : public Node {
public:
    RParen(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return ")"; }

private:
    const Span span_;
};

class LSquare : public Node {
public:
    LSquare(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "["; }

private:
    const Span span_;
};

class RSquare : public Node {
public:
    RSquare(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "]"; }

private:
    const Span span_;
};

class LCurly : public Node {
public:
    LCurly(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "{"; }

private:
    const Span span_;
};

class RCurly : public Node {
public:
    RCurly(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "}"; }

private:
    const Span span_;
};

class Exclamation : public Node {
public:
    Exclamation(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "?"; }

private:
    const Span span_;
};

class Colon : public Node {
public:
    Colon(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return ":"; }

private:
    const Span span_;
};

class Semicolon : public Node {
public:
    Semicolon(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return ";"; }

private:
    const Span span_;
};

class Sizeof : public Node {
public:
    Sizeof(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "sizeof"; }

private:
    const Span span_;
};

class Case : public Node {
public:
    Case(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "case"; }

private:
    const Span span_;
};

class Default : public Node {
public:
    Default(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "default"; }

private:
    const Span span_;
};

class If : public Node {
public:
    If(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "if"; }

private:
    const Span span_;
};

class Else : public Node {
public:
    Else(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "else"; }

private:
    const Span span_;
};

class Switch : public Node {
public:
    Switch(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "switch"; }

private:
    const Span span_;
};

class While : public Node {
public:
    While(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "while"; }

private:
    const Span span_;
};

class Do : public Node {
public:
    Do(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "do"; }

private:
    const Span span_;
};

class For : public Node {
public:
    For(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "for"; }

private:
    const Span span_;
};

class Goto : public Node {
public:
    Goto(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "goto"; }

private:
    const Span span_;
};

class Continue : public Node {
public:
    Continue(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "continue"; }

private:
    const Span span_;
};

class Break : public Node {
public:
    Break(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "break"; }

private:
    const Span span_;
};

class Return : public Node {
public:
    Return(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "return"; }

private:
    const Span span_;
};

}  // namespace tinyc

#endif  // TINYC_PARSER_NODE_H_
