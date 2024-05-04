#ifndef TINYC_PARSER_DECL_H_
#define TINYC_PARSER_DECL_H_

#include <memory>
#include <string>
#include <vector>

#include "../span.h"
#include "node.h"
#include "type/type.h"

namespace tinyc {

enum class DeclarationKind {
    Variable,
    Function,
};

class Declaration : public Node {
public:
    virtual ~Declaration() {}
    virtual DeclarationKind kind() const = 0;
};

class VariableDeclarationName : public Node {
public:
    VariableDeclarationName(const std::string& name, Span span)
        : name_(name), span_(span) {}

    inline const std::string& name() const { return name_; }

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return name_; }

private:
    const std::string name_;
    const Span span_;
};

class VariableDeclaration : public Declaration {
public:
    VariableDeclaration(const std::shared_ptr<Type>& type,
                        const VariableDeclarationName& name)
        : type_(type), name_(name) {}

    const std::shared_ptr<Type>& type() const { return type_; }

    const VariableDeclarationName& name() const { return name_; }

    inline DeclarationKind kind() const override {
        return DeclarationKind::Variable;
    }

    inline Span span() const override {
        return concat_spans({type_->span(), name_.span()});
    }

    inline std::string debug() const override { return "todo"; }

private:
    const std::shared_ptr<Type> type_;
    const VariableDeclarationName name_;
};

class FunctionDeclarationName : public Node {
public:
    FunctionDeclarationName(const std::string& name, Span span)
        : name_(name), span_(span) {}

    inline const std::string& name() const { return name_; }

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return name_; }

private:
    const std::string name_;
    const Span span_;
};

class FunctionDeclarationArgName : public Node {
public:
    FunctionDeclarationArgName(const std::string& name, Span span)
        : name_(name), span_(span) {}

    inline const std::string& name() const { return name_; }

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return name_; }

private:
    const std::string name_;
    const Span span_;
};

class FunctionDeclaration : public Declaration {
public:
    FunctionDeclaration(const std::shared_ptr<FunctionType>& type,
                        const FunctionDeclarationName& name,
                        const std::vector<FunctionDeclarationArgName>& args)
        : type_(type), name_(name), args_(args) {}

    const std::shared_ptr<FunctionType>& type() const { return type_; }

    const FunctionDeclarationName& name() const { return name_; }

    const std::vector<FunctionDeclarationArgName>& args() const {
        return args_;
    }

    inline DeclarationKind kind() const override {
        return DeclarationKind::Variable;
    }

    inline Span span() const override {
        return concat_spans({type_->span(), name_.span()});
    }

    inline std::string debug() const override { return "todo"; }

private:
    const std::shared_ptr<FunctionType> type_;
    const FunctionDeclarationName name_;
    const std::vector<FunctionDeclarationArgName> args_;
};

}  // namespace tinyc

#endif  // TINYC_PARSER_DECL_H_
