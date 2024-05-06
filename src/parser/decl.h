#ifndef TINYC_PARSER_DECL_H_
#define TINYC_PARSER_DECL_H_

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "../span.h"
#include "node.h"
#include "type/type.h"

namespace tinyc {

class BlockStatement;

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

class FunctionDeclarationArg : public Node {
public:
    FunctionDeclarationArg(const std::shared_ptr<Type>& type,
                           const FunctionDeclarationArgName& name)
        : type_(type), name_(name) {}

    inline const std::shared_ptr<Type>& type() const { return type_; }

    inline const std::optional<FunctionDeclarationArgName>& name() const {
        return name_;
    }

    inline Span span() const override {
        if (name_.has_value())
            return concat_spans({type_->span(), name_.value().span()});
        else
            return type_->span();
    }

    inline std::string debug() const override {
        if (name_.has_value())
            return type_->debug() + " " + name_.value().debug();
        else
            return type_->debug();
    }

private:
    const std::shared_ptr<Type> type_;
    const std::optional<FunctionDeclarationArgName> name_;
};

class FunctionDeclaration : public Declaration {
public:
    FunctionDeclaration(const std::shared_ptr<Type>& ret_type,
                        const FunctionDeclarationName& name,
                        const LParen lparen,
                        const std::vector<FunctionDeclarationArg>& args,
                        const RParen rparen)
        : ret_type_(ret_type),
          name_(name),
          lparen_(lparen),
          args_(args),
          rparen_(rparen),
          body_(std::nullopt),
          semicolon_(std::nullopt) {}

    FunctionDeclaration(const std::shared_ptr<Type>& ret_type,
                        const FunctionDeclarationName& name,
                        const LParen lparen,
                        const std::vector<FunctionDeclarationArg>& args,
                        const RParen rparen,
                        const std::shared_ptr<BlockStatement> body)
        : ret_type_(ret_type),
          name_(name),
          lparen_(lparen),
          args_(args),
          rparen_(rparen),
          body_(body),
          semicolon_(std::nullopt) {}

    FunctionDeclaration(const std::shared_ptr<Type>& ret_type,
                        const FunctionDeclarationName& name,
                        const LParen lparen,
                        const std::vector<FunctionDeclarationArg>& args,
                        const RParen rparen, const Semicolon semicolon)
        : ret_type_(ret_type),
          name_(name),
          lparen_(lparen),
          args_(args),
          rparen_(rparen),
          body_(std::nullopt),
          semicolon_(semicolon) {}

    FunctionDeclaration(const std::shared_ptr<Type>& ret_type,
                        const FunctionDeclarationName& name,
                        const LParen lparen,
                        const std::vector<FunctionDeclarationArg>& args,
                        const RParen rparen,
                        const std::shared_ptr<BlockStatement> body,
                        const Semicolon semicolon)
        : ret_type_(ret_type),
          name_(name),
          lparen_(lparen),
          args_(args),
          rparen_(rparen),
          body_(body),
          semicolon_(semicolon) {}

    inline const std::shared_ptr<Type>& ret_type() const { return ret_type_; }

    inline const FunctionDeclarationName& name() const { return name_; }

    inline const std::vector<FunctionDeclarationArg>& args() const {
        return args_;
    }

    inline const std::optional<std::shared_ptr<BlockStatement>>& body() const {
        return body_;
    }

    inline const std::optional<Semicolon>& semicolon() const {
        return semicolon_;
    }

    inline DeclarationKind kind() const override {
        return DeclarationKind::Function;
    }

    Span span() const override;

    inline std::string debug() const override { return "todo"; }

private:
    const std::shared_ptr<Type> ret_type_;
    const FunctionDeclarationName name_;
    const LParen lparen_;
    const std::vector<FunctionDeclarationArg> args_;
    const RParen rparen_;
    const std::optional<std::shared_ptr<BlockStatement>> body_;
    const std::optional<Semicolon> semicolon_;
};

}  // namespace tinyc

#endif  // TINYC_PARSER_DECL_H_
