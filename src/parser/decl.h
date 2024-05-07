#ifndef TINYC_PARSER_DECL_H_
#define TINYC_PARSER_DECL_H_

#include <memory>
#include <optional>
#include <sstream>
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

class VariableDeclarationInitializer : public Node {
public:
    inline const Assign& assign() const { return assign_; }

    inline const std::shared_ptr<Expression>& expr() const { return expr_; }

    Span span() const override;

    std::string debug() const override;

private:
    const Assign assign_;
    const std::shared_ptr<Expression> expr_;
};

class VariableDeclaration : public Declaration {
public:
    VariableDeclaration(const std::shared_ptr<Type>& type, Semicolon semicolon)
        : type_(type),
          name_(std::nullopt),
          initializer_(std::nullopt),
          semicolon_(semicolon) {}

    VariableDeclaration(const std::shared_ptr<Type>& type,
                        const VariableDeclarationName& name,
                        Semicolon semicolon)
        : type_(type),
          name_(name),
          initializer_(std::nullopt),
          semicolon_(semicolon) {}

    VariableDeclaration(const std::shared_ptr<Type>& type,
                        const VariableDeclarationInitializer& initializer,
                        Semicolon semicolon)
        : type_(type),
          name_(std::nullopt),
          initializer_(initializer),
          semicolon_(semicolon) {}

    VariableDeclaration(const std::shared_ptr<Type>& type,
                        const VariableDeclarationName& name,
                        const VariableDeclarationInitializer& initializer,
                        Semicolon semicolon)
        : type_(type),
          name_(name),
          initializer_(initializer),
          semicolon_(semicolon) {}

    inline const std::shared_ptr<Type>& type() const { return type_; }

    inline const std::optional<VariableDeclarationName>& name() const {
        return name_;
    }

    inline const std::optional<VariableDeclarationInitializer> initializer()
        const {
        return initializer_;
    }

    inline const Semicolon& semicolon() const { return semicolon_; }

    inline DeclarationKind kind() const override {
        return DeclarationKind::Variable;
    }

    Span span() const override {
        std::vector<Span> spans;
        spans.emplace_back(type_->span());
        if (name_.has_value()) spans.emplace_back(name_->span());
        if (initializer_.has_value()) spans.emplace_back(initializer_->span());
        spans.emplace_back(semicolon_.span());
        return concat_spans(spans);
    }

    std::string debug() const override {
        std::stringstream ss;
        ss << type_->debug();
        if (name_.has_value()) ss << " " << name_->debug();
        if (initializer_.has_value()) ss << " " << initializer_->debug();
        ss << semicolon_.debug();
        return ss.str();
    }

private:
    const std::shared_ptr<Type> type_;
    const std::optional<VariableDeclarationName> name_;
    const std::optional<VariableDeclarationInitializer> initializer_;
    const Semicolon semicolon_;
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

    inline std::string debug() const override;

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
