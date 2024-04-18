#ifndef TINYC_PARSER_TYPE_H_
#define TINYC_PARSER_TYPE_H_

#include "../span.h"

namespace tinyc {

enum class TypeKind {
    Int,
};

// Type in c.
class Type {
public:
    virtual ~Type() {}
    virtual Span span() const = 0;
    virtual TypeKind kind() const = 0;
    virtual std::string debug() const = 0;
};

// `int`.
class IntType : public Type {
public:
    IntType(Span span) : span_(span) {}

    Span span() const override { return span_; }

    TypeKind kind() const override { return TypeKind::Int; }

    std::string debug() const override { return "int"; }

private:
    const Span span_;
};

}  // namespace tinyc

#endif  // TINYC_PARSER_TYPE_H_
