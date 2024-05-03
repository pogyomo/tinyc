#ifndef TINYC_PARSER_TYPE_SPECIFIER_BUILTIN_H_
#define TINYC_PARSER_TYPE_SPECIFIER_BUILTIN_H_

#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "specifier.h"

namespace tinyc {

enum class BuiltinTypeSpecifierKind {
    Void,
    Signed,
    Unsigned,
    Char,
    Short,
    Int,
    Long,
    Float,
    Double,
};

class BuiltinTypeSpecifier : public TypeSpecifier {
public:
    BuiltinTypeSpecifier(BuiltinTypeSpecifierKind kind, Span span)
        : kind_(kind), span_(span) {}

    inline const BuiltinTypeSpecifierKind& builtin_kind() const {
        return kind_;
    }

    inline TypeSpecifierKind kind() const override {
        return TypeSpecifierKind::Builtin;
    }

    inline Span span() const override { return span_; }

    std::string debug() const override {
        switch (kind_) {
            case BuiltinTypeSpecifierKind::Void:
                return "void";
            case BuiltinTypeSpecifierKind::Signed:
                return "signed";
            case BuiltinTypeSpecifierKind::Unsigned:
                return "unsigned";
            case BuiltinTypeSpecifierKind::Char:
                return "char";
            case BuiltinTypeSpecifierKind::Short:
                return "short";
            case BuiltinTypeSpecifierKind::Int:
                return "int";
            case BuiltinTypeSpecifierKind::Long:
                return "long";
            case BuiltinTypeSpecifierKind::Float:
                return "float";
            case BuiltinTypeSpecifierKind::Double:
                return "double";
            default:
                return "";
        }
    }

private:
    const BuiltinTypeSpecifierKind kind_;
    const Span span_;
};

}  // namespace tinyc

#endif  // TINYC_PARSER_TYPE_SPECIFIER_BUILTIN_H_
