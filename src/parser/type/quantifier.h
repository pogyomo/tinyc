#ifndef TINYC_PARSER_TYPE_QUANTIFIER_H_
#define TINYC_PARSER_TYPE_QUANTIFIER_H_

#include "../node.h"

namespace tinyc {

enum class TypeQuantifierKind {
    Const,
    Volatile,
};

class TypeQuantifier : public Node {
public:
    TypeQuantifier(TypeQuantifierKind kind, Span span)
        : kind_(kind), span_(span) {}

    const TypeQuantifierKind& kind() const { return kind_; }

    inline Span span() const override { return span_; }

    inline std::string debug() const override {
        if (kind_ == TypeQuantifierKind::Const) {
            return "const";
        } else {
            return "volatile";
        }
    }

private:
    const TypeQuantifierKind kind_;
    const Span span_;
};

}  // namespace tinyc

#endif  // TINYC_PARSER_TYPE_QUANTIFIER_H_
