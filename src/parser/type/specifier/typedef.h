#ifndef TINYC_PARSER_TYPE_TYPEDEF_H_
#define TINYC_PARSER_TYPE_TYPEDEF_H_

#include "specifier.h"
namespace tinyc {

class TypedefNameSpecifier : public TypeSpecifier {
public:
    TypedefNameSpecifier(const std::string& name, Span span)
        : name_(name), span_(span) {}

    inline TypeSpecifierKind kind() const override {
        return TypeSpecifierKind::TypedefName;
    }

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return name_; }

private:
    const std::string name_;
    const Span span_;
};

}  // namespace tinyc

#endif
