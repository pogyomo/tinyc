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

class Sizeof : public Node {
public:
    Sizeof(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "sizeof"; }

private:
    const Span span_;
};

}  // namespace tinyc

#endif  // TINYC_PARSER_NODE_H_
