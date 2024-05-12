#ifndef TINYC_PARSER_TYPE_TYPEDEF_H_
#define TINYC_PARSER_TYPE_TYPEDEF_H_

#include "specifier.h"
namespace tinyc {

class TypedefNameTypeSpecifier : public TypeSpecifier {
public:
    TypedefNameTypeSpecifier(const std::string& name, Span span)
        : name_(name), span_(span) {}

    inline const std::string& name() const { return name_; }

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
