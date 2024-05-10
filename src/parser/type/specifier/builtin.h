#ifndef TINYC_PARSER_TYPE_SPECIFIER_BUILTIN_H_
#define TINYC_PARSER_TYPE_SPECIFIER_BUILTIN_H_

#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "specifier.h"

namespace tinyc {

// reference: https://ja.cppreference.com/w/cpp/language/types
enum class BuiltinTypeSpecifierKind {
    Void,
    Char,
    UnsignedChar,
    Short,
    UnsignedShort,
    Int,
    UnsignedInt,
    Long,
    UnsignedLong,
    LongLong,
    UnsignedLongLong,
    Float,
    Double,
    LongDouble,
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
            case BuiltinTypeSpecifierKind::Char:
                return "char";
            case BuiltinTypeSpecifierKind::UnsignedChar:
                return "unsigned char";
            case BuiltinTypeSpecifierKind::Int:
                return "int";
            case BuiltinTypeSpecifierKind::UnsignedInt:
                return "unsigned int";
            case BuiltinTypeSpecifierKind::Long:
                return "long";
            case BuiltinTypeSpecifierKind::UnsignedLong:
                return "unsigned long";
            case BuiltinTypeSpecifierKind::LongLong:
                return "long long";
            case BuiltinTypeSpecifierKind::UnsignedLongLong:
                return "unsigned long long";
            case BuiltinTypeSpecifierKind::Float:
                return "float";
            case BuiltinTypeSpecifierKind::Double:
                return "double";
            case BuiltinTypeSpecifierKind::LongDouble:
                return "long double";
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
