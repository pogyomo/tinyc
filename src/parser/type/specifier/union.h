#ifndef TINYC_PARSER_TYPE_SPECIFIER_UNION_H_
#define TINYC_PARSER_TYPE_SPECIFIER_UNION_H_

#include <optional>
#include <sstream>

#include "../../../span.h"
#include "../../node.h"
#include "../type.h"
#include "specifier.h"

namespace tinyc {

enum class UnionSpecifierKind {
    Named,
    Anonymous,
};

class UnionSpecifier : public TypeSpecifier {
public:
    virtual ~UnionSpecifier() {}
    virtual UnionSpecifierKind union_kind() const = 0;

    TypeSpecifierKind kind() const override { return TypeSpecifierKind::Enum; }
};

class UnionSpecifierDeclIdent : public Node {
public:
    UnionSpecifierDeclIdent(const std::string& name, Span span)
        : name_(name), span_(span) {}

    inline const std::string& name() const { return name_; }

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return name_; }

private:
    const std::string name_;
    const Span span_;
};

class UnionSpecifierDecl : public Node {
public:
    UnionSpecifierDecl(const std::shared_ptr<Type> type,
                       const UnionSpecifierDeclIdent& ident)
        : type_(type), ident_(ident) {}

    inline const std::shared_ptr<Type>& type() const { return type_; }

    inline const UnionSpecifierDeclIdent& ident() const { return ident_; }

    inline Span span() const override {
        return concat_spans({type_->span(), ident_.span()});
    }

    inline std::string debug() const override {
        return type_->debug() + " " + ident_.debug();
    }

private:
    const std::shared_ptr<Type> type_;
    const UnionSpecifierDeclIdent ident_;
};

class UnionSpecifierBody : public Node {
public:
    UnionSpecifierBody(LCurly lcurly,
                       const std::vector<UnionSpecifierDecl> decls,
                       RCurly rcurly)
        : lcurly_(lcurly), decls_(decls), rcurly_(rcurly) {}

    inline const LCurly& lcurly() const { return lcurly_; }

    inline const std::vector<UnionSpecifierDecl>& decls() const {
        return decls_;
    }

    inline const RCurly& rcurly() const { return rcurly_; }

    Span span() const override {
        std::vector<Span> spans;
        spans.emplace_back(lcurly_.span());
        for (const auto& decl : decls_) {
            spans.emplace_back(decl.span());
        }
        spans.emplace_back(rcurly_.span());
        return concat_spans(spans);
    }

    std::string debug() const override {
        std::stringstream ss;
        ss << lcurly_.debug();
        for (const auto& decl : decls_) {
            ss << " " << decl.debug();
        }
        ss << " " << rcurly_.debug();
        return ss.str();
    }

private:
    const LCurly lcurly_;
    const std::vector<UnionSpecifierDecl> decls_;
    const RCurly rcurly_;
};

class NamedUnionSpecifierName : public Node {
public:
    NamedUnionSpecifierName(const std::string& name, Span span)
        : name_(name), span_(span) {}

    inline const std::string& name() const { return name_; }

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return name_; }

private:
    const std::string name_;
    const Span span_;
};

class NamedUnionSpecifier : public UnionSpecifier {
public:
    NamedUnionSpecifier(Union union_kw, const NamedUnionSpecifierName& name)
        : union_kw_(union_kw), name_(name), body_(std::nullopt) {}

    NamedUnionSpecifier(Union union_kw, const NamedUnionSpecifierName& name,
                        UnionSpecifierBody body)
        : union_kw_(union_kw), name_(name), body_(body) {}

    const Union& union_kw() const {
        return union_kw_;
        ;
    }

    const NamedUnionSpecifierName& name() const { return name_; }

    const std::optional<UnionSpecifierBody>& body() const { return body_; }

    inline UnionSpecifierKind union_kind() const override {
        return UnionSpecifierKind::Named;
    }

    inline Span span() const override {
        std::vector<Span> spans;
        spans.emplace_back(union_kw_.span());
        spans.emplace_back(name_.span());
        if (body_.has_value()) {
            spans.emplace_back(body_->span());
        }
        return concat_spans(spans);
    }

    inline std::string debug() const override {
        if (body_.has_value()) {
            return union_kw_.debug() + " " + name_.debug() + " " +
                   body_->debug();
        } else {
            return union_kw_.debug() + " " + name_.debug();
        }
    }

private:
    const Union union_kw_;
    const NamedUnionSpecifierName name_;
    const std::optional<UnionSpecifierBody> body_;
};

class AnonymousUnionSpecifier : public UnionSpecifier {
    AnonymousUnionSpecifier(Union union_kw, UnionSpecifierBody body)
        : union_kw_(union_kw), body_(body) {}

    const Union& union_kw() const { return union_kw_; }

    const UnionSpecifierBody& body() const { return body_; }

    inline UnionSpecifierKind union_kind() const override {
        return UnionSpecifierKind::Anonymous;
    }

    inline Span span() const override {
        return concat_spans({union_kw_.span(), body_.span()});
    }

    inline std::string debug() const override {
        return union_kw_.debug() + " " + body_.debug();
    }

private:
    const Union union_kw_;
    const UnionSpecifierBody body_;
};

}  // namespace tinyc

#endif  // TINYC_PARSER_TYPE_SPECIFIER_UNION_H_
