#ifndef TINYC_PARSER_DECL_H_
#define TINYC_PARSER_DECL_H_

#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "../span.h"
#include "expr.h"
#include "node.h"
#include "type/type.h"

namespace tinyc {

class BlockStatement;

enum class StorageClassSpecifierKind {
    Auto,
    Register,
    Static,
    Extern,
    Typedef,
};

class StorageClassSpecifier : public Node {
public:
    StorageClassSpecifier(StorageClassSpecifierKind kind, Span span)
        : kind_(kind), span_(span) {}

    inline const StorageClassSpecifierKind& kind() const { return kind_; }

    inline Span span() const override { return span_; }

    std::string debug() const override;

private:
    const StorageClassSpecifierKind kind_;
    const Span span_;
};

enum class DeclarationKind {
    Variables,
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

enum class VariableDeclarationInitKind {
    Expr,
    List,
};

class VariableDeclarationInit : public Node {
public:
    virtual ~VariableDeclarationInit() {}
    virtual VariableDeclarationInitKind kind() const = 0;
};

class VariableDeclarationInitExpr : public VariableDeclarationInit {
public:
    VariableDeclarationInitExpr(const std::shared_ptr<Expression>& expr)
        : expr_(expr) {}

    inline const std::shared_ptr<Expression>& expr() const { return expr_; }

    inline VariableDeclarationInitKind kind() const override {
        return VariableDeclarationInitKind::Expr;
    }

    inline Span span() const override { return expr_->span(); }

    inline std::string debug() const override { return expr_->debug(); }

private:
    const std::shared_ptr<Expression> expr_;
};

class VariableDeclarationInitList : public VariableDeclarationInit {
public:
    VariableDeclarationInitList(
        LCurly lcurly,
        const std::vector<std::shared_ptr<VariableDeclarationInit>>&
            initializers,
        RCurly rcurly)
        : lcurly_(lcurly), initializers_(initializers), rcurly_(rcurly) {}

    inline const LCurly& lcurly() const { return lcurly_; }

    inline const std::vector<std::shared_ptr<VariableDeclarationInit>>&
    initializers() const {
        return initializers_;
    }

    inline const RCurly& rcurly() const { return rcurly_; }

    inline VariableDeclarationInitKind kind() const override {
        return VariableDeclarationInitKind::List;
    }

    Span span() const override {
        std::vector<Span> spans;
        spans.emplace_back(lcurly_.span());
        for (const auto& initializer : initializers_)
            spans.emplace_back(initializer->span());
        spans.emplace_back(rcurly_.span());
        return concat_spans(spans);
    }

    std::string debug() const override {
        std::stringstream ss;
        ss << lcurly_.debug() << " ";
        if (!initializers_.empty()) {
            ss << initializers_[0]->debug();
            for (int i = 1; i < initializers_.size(); i++)
                ss << ", " << initializers_[i]->debug();
        }
        ss << " " << rcurly_.debug();
        return ss.str();
    }

private:
    const LCurly lcurly_;
    const std::vector<std::shared_ptr<VariableDeclarationInit>> initializers_;
    const RCurly rcurly_;
};

enum class VariableDeclarationKind {
    Named,
    Anonymous,
};

class VariableDeclaration : public Node {
public:
    virtual ~VariableDeclaration() {}
    virtual const std::optional<StorageClassSpecifier>& class_specifier()
        const = 0;
    virtual const std::shared_ptr<Type>& type() const = 0;
    virtual const std::optional<VariableDeclarationName> name() const = 0;
    virtual VariableDeclarationKind kind() const = 0;
};

class NamedVariableDeclaration : public VariableDeclaration {
public:
    NamedVariableDeclaration(
        const std::optional<StorageClassSpecifier> class_specifier,
        const std::shared_ptr<Type>& type, const VariableDeclarationName& name)
        : class_specifier_(class_specifier),
          type_(type),
          name_(name),
          initializer_(std::nullopt) {}

    NamedVariableDeclaration(
        const std::optional<StorageClassSpecifier> class_specifier,
        const std::shared_ptr<Type>& type, const VariableDeclarationName& name,
        const std::pair<Assign, std::shared_ptr<VariableDeclarationInit>>&
            initializer)
        : class_specifier_(class_specifier),
          type_(type),
          name_(name),
          initializer_(initializer) {}

    inline const std::optional<
        std::pair<Assign, std::shared_ptr<VariableDeclarationInit>>>&
    initializer() const {
        return initializer_;
    }

    inline const std::optional<StorageClassSpecifier>& class_specifier()
        const override {
        return class_specifier_;
    }

    inline const std::shared_ptr<Type>& type() const override { return type_; }

    inline const std::optional<VariableDeclarationName> name() const override {
        return name_;
    }

    inline VariableDeclarationKind kind() const override {
        return VariableDeclarationKind::Named;
    }

    Span span() const override {
        std::vector<Span> spans;
        if (class_specifier_.has_value())
            spans.emplace_back(class_specifier_->span());
        spans.emplace_back(type_->span());
        spans.emplace_back(name_.span());
        if (initializer_.has_value()) {
            spans.emplace_back(initializer_->first.span());
            spans.emplace_back(initializer_->second->span());
        }
        return concat_spans(spans);
    }

    std::string debug() const override {
        std::stringstream ss;
        if (class_specifier_.has_value())
            ss << class_specifier_->debug() << " ";
        ss << type_->debug() << " " << name_.debug();
        if (initializer_.has_value()) {
            ss << " " << initializer_->first.debug() << " ";
            ss << initializer_->second->debug();
        }
        return ss.str();
    }

private:
    const std::optional<StorageClassSpecifier> class_specifier_;
    const std::shared_ptr<Type> type_;
    const VariableDeclarationName name_;
    const std::optional<
        std::pair<Assign, std::shared_ptr<VariableDeclarationInit>>>
        initializer_;
};

class AnonymousVariableDeclaration : public VariableDeclaration {
public:
    AnonymousVariableDeclaration(
        const std::optional<StorageClassSpecifier>& class_specifier,
        const std::shared_ptr<Type>& type)
        : class_specifier_(class_specifier), type_(type) {}

    inline const std::optional<StorageClassSpecifier>& class_specifier()
        const override {
        return class_specifier_;
    }

    inline const std::shared_ptr<Type>& type() const override { return type_; }

    inline const std::optional<VariableDeclarationName> name() const override {
        return std::nullopt;
    }

    inline VariableDeclarationKind kind() const override {
        return VariableDeclarationKind::Anonymous;
    }

    inline Span span() const override {
        std::vector<Span> spans;
        if (class_specifier_.has_value())
            spans.emplace_back(class_specifier_->span());
        spans.emplace_back(type_->span());
        return concat_spans(spans);
    }

    std::string debug() const override {
        std::stringstream ss;
        if (class_specifier_.has_value())
            ss << class_specifier_->debug() << " ";
        ss << type_->debug();
        return ss.str();
    }

private:
    const std::optional<StorageClassSpecifier> class_specifier_;
    const std::shared_ptr<Type> type_;
};

class VariablesDeclaration : public Declaration {
public:
    VariablesDeclaration(
        const std::vector<std::shared_ptr<VariableDeclaration>>& decls,
        Semicolon semicolon)
        : decls_(decls), semicolon_(semicolon) {}

    inline const std::vector<std::shared_ptr<VariableDeclaration>>& decls()
        const {
        return decls_;
    }

    inline const Semicolon& semicolon() const { return semicolon_; }

    inline DeclarationKind kind() const override {
        return DeclarationKind::Variables;
    }

    Span span() const override {
        std::vector<Span> spans;
        for (const auto& decl : decls_) spans.emplace_back(decl->span());
        spans.emplace_back(semicolon_.span());
        return concat_spans(spans);
    }

    std::string debug() const override {
        std::stringstream ss;
        if (!decls_.empty()) {
            ss << decls_[0]->debug();
            for (int i = 1; i < decls_.size(); i++)
                ss << ", " << decls_[i]->debug();
        }
        ss << semicolon_.debug();
        return ss.str();
    }

private:
    const std::vector<std::shared_ptr<VariableDeclaration>> decls_;
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

class FunctionDeclaration : public Declaration {
public:
    FunctionDeclaration(
        const std::optional<StorageClassSpecifier> class_specifier,
        const std::shared_ptr<Type>& ret_type,
        const FunctionDeclarationName& name, const LParen lparen,
        const std::vector<FunctionParam>& args, const RParen rparen)
        : class_specifier_(class_specifier),
          ret_type_(ret_type),
          name_(name),
          lparen_(lparen),
          params_(args),
          rparen_(rparen),
          body_(std::nullopt),
          semicolon_(std::nullopt) {}

    FunctionDeclaration(
        const std::optional<StorageClassSpecifier> class_specifier,
        const std::shared_ptr<Type>& ret_type,
        const FunctionDeclarationName& name, const LParen lparen,
        const std::vector<FunctionParam>& args, const RParen rparen,
        const std::shared_ptr<BlockStatement> body)
        : class_specifier_(class_specifier),
          ret_type_(ret_type),
          name_(name),
          lparen_(lparen),
          params_(args),
          rparen_(rparen),
          body_(body),
          semicolon_(std::nullopt) {}

    FunctionDeclaration(
        const std::optional<StorageClassSpecifier> class_specifier,
        const std::shared_ptr<Type>& ret_type,
        const FunctionDeclarationName& name, const LParen lparen,
        const std::vector<FunctionParam>& args, const RParen rparen,
        const Semicolon semicolon)
        : class_specifier_(class_specifier),
          ret_type_(ret_type),
          name_(name),
          lparen_(lparen),
          params_(args),
          rparen_(rparen),
          body_(std::nullopt),
          semicolon_(semicolon) {}

    FunctionDeclaration(
        const std::optional<StorageClassSpecifier> class_specifier,
        const std::shared_ptr<Type>& ret_type,
        const FunctionDeclarationName& name, const LParen lparen,
        const std::vector<FunctionParam>& args, const RParen rparen,
        const std::shared_ptr<BlockStatement> body, const Semicolon semicolon)
        : class_specifier_(class_specifier),
          ret_type_(ret_type),
          name_(name),
          lparen_(lparen),
          params_(args),
          rparen_(rparen),
          body_(body),
          semicolon_(semicolon) {}

    inline const std::optional<StorageClassSpecifier>& class_specifier() const {
        return class_specifier_;
    }

    inline const std::shared_ptr<Type>& ret_type() const { return ret_type_; }

    inline const FunctionDeclarationName& name() const { return name_; }

    inline const std::vector<FunctionParam>& params() const { return params_; }

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
    const std::optional<StorageClassSpecifier> class_specifier_;
    const std::shared_ptr<Type> ret_type_;
    const FunctionDeclarationName name_;
    const LParen lparen_;
    const std::vector<FunctionParam> params_;
    const RParen rparen_;
    const std::optional<std::shared_ptr<BlockStatement>> body_;
    const std::optional<Semicolon> semicolon_;
};

}  // namespace tinyc

#endif  // TINYC_PARSER_DECL_H_
