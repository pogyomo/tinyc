#ifndef TINYC_PARSER_TYPE_STRUCT_H_
#define TINYC_PARSER_TYPE_STRUCT_H_

#include <optional>
#include <sstream>

#include "../../../span.h"
#include "../../node.h"
#include "../type.h"
#include "specifier.h"

namespace tinyc {

enum class StructSpecifierKind {
    Named,
    Anonymous,
};

class StructSpecifier : public TypeSpecifier {
public:
    virtual ~StructSpecifier() {}
    virtual StructSpecifierKind struct_kind() const = 0;

    TypeSpecifierKind kind() const override { return TypeSpecifierKind::Enum; }
};

class StructSpecifierDeclIdent : public Node {
public:
    StructSpecifierDeclIdent(const std::string& name, Span span)
        : name_(name), span_(span) {}

    inline const std::string& name() const { return name_; }

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return name_; }

private:
    const std::string name_;
    const Span span_;
};

class StructSpecifierDecl : public Node {
public:
    StructSpecifierDecl(const std::shared_ptr<Type> type,
                        const StructSpecifierDeclIdent& ident)
        : type_(type), ident_(ident) {}

    inline const std::shared_ptr<Type>& type() const { return type_; }

    inline const StructSpecifierDeclIdent& ident() const { return ident_; }

    inline Span span() const override {
        return concat_spans({type_->span(), ident_.span()});
    }

    inline std::string debug() const override {
        return type_->debug() + " " + ident_.debug();
    }

private:
    const std::shared_ptr<Type> type_;
    const StructSpecifierDeclIdent ident_;
};

class StructSpecifierBody : public Node {
public:
    StructSpecifierBody(LCurly lcurly,
                        const std::vector<StructSpecifierDecl> decls,
                        RCurly rcurly)
        : lcurly_(lcurly), decls_(decls), rcurly_(rcurly) {}

    inline const LCurly& lcurly() const { return lcurly_; }

    inline const std::vector<StructSpecifierDecl>& decls() const {
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
    const std::vector<StructSpecifierDecl> decls_;
    const RCurly rcurly_;
};

class NamedStructSpecifierName : public Node {
public:
    NamedStructSpecifierName(const std::string& name, Span span)
        : name_(name), span_(span) {}

    inline const std::string& name() const { return name_; }

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return name_; }

private:
    const std::string name_;
    const Span span_;
};

class NamedStructSpecifier : public StructSpecifier {
public:
    NamedStructSpecifier(Struct struct_kw, const NamedStructSpecifierName& name)
        : struct_kw_(struct_kw), name_(name), body_(std::nullopt) {}

    NamedStructSpecifier(Struct struct_kw, const NamedStructSpecifierName& name,
                         StructSpecifierBody body)
        : struct_kw_(struct_kw), name_(name), body_(body) {}

    const Struct& struct_kw() const {
        return struct_kw_;
        ;
    }

    const NamedStructSpecifierName& name() const { return name_; }

    const std::optional<StructSpecifierBody>& body() const { return body_; }

    inline StructSpecifierKind struct_kind() const override {
        return StructSpecifierKind::Named;
    }

    inline Span span() const override {
        std::vector<Span> spans;
        spans.emplace_back(struct_kw_.span());
        spans.emplace_back(name_.span());
        if (body_.has_value()) {
            spans.emplace_back(body_->span());
        }
        return concat_spans(spans);
    }

    inline std::string debug() const override {
        if (body_.has_value()) {
            return struct_kw_.debug() + " " + name_.debug() + " " +
                   body_->debug();
        } else {
            return struct_kw_.debug() + " " + name_.debug();
        }
    }

private:
    const Struct struct_kw_;
    const NamedStructSpecifierName name_;
    const std::optional<StructSpecifierBody> body_;
};

class AnonymousStructSpecifier : public StructSpecifier {
    AnonymousStructSpecifier(Struct struct_kw, StructSpecifierBody body)
        : struct_kw_(struct_kw), body_(body) {}

    const Struct& struct_kw() const { return struct_kw_; }

    const StructSpecifierBody& body() const { return body_; }

    inline StructSpecifierKind struct_kind() const override {
        return StructSpecifierKind::Anonymous;
    }

    inline Span span() const override {
        return concat_spans({struct_kw_.span(), body_.span()});
    }

    inline std::string debug() const override {
        return struct_kw_.debug() + " " + body_.debug();
    }

private:
    const Struct struct_kw_;
    const StructSpecifierBody body_;
};

}  // namespace tinyc

#endif  // TINYC_PARSER_TYPE_STRUCT_H_
