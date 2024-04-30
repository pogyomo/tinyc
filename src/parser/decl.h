#ifndef TINYC_PARSER_DECL_H_
#define TINYC_PARSER_DECL_H_

#include <string>

#include "../span.h"
#include "node.h"

namespace tinyc {

enum class DeclarationKind {
    Function,
    Variable,
};

class Declaration : public Node {
public:
    virtual ~Declaration() {}
    virtual DeclarationKind kind() const = 0;
};

enum class FunctionDeclarationStorageSpecifierKind {
    Extern,
    Static,
};

class FunctionDeclarationStorageSpecifier : public Node {
public:
    FunctionDeclarationStorageSpecifier(
        FunctionDeclarationStorageSpecifierKind kind, Span span)
        : kind_(kind), span_(span) {}

    const FunctionDeclarationStorageSpecifierKind& kind() const {
        return kind_;
    }

    Span span() const override { return span_; }

    std::string debug() const override {
        switch (kind_) {
            case FunctionDeclarationStorageSpecifierKind::Extern:
                return "extern";
            case FunctionDeclarationStorageSpecifierKind::Static:
                return "static";
            default:
                return "";
        }
    }

private:
    const FunctionDeclarationStorageSpecifierKind kind_;
    const Span span_;
};

class FunctionDeclaration : public Declaration {
public:
};

}  // namespace tinyc

#endif  // TINYC_PARSER_DECL_H_
