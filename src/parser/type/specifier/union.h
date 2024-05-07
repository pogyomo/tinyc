#ifndef TINYC_PARSER_TYPE_SPECIFIER_UNION_H_
#define TINYC_PARSER_TYPE_SPECIFIER_UNION_H_

#include <optional>
#include <sstream>

#include "../../../span.h"
#include "../../decl.h"
#include "../../node.h"
#include "specifier.h"

namespace tinyc {

enum class UnionTypeSpecifierKind {
    Named,
    Anonymous,
};

class UnionTypeSpecifier : public TypeSpecifier {
public:
    virtual ~UnionTypeSpecifier() {}
    virtual UnionTypeSpecifierKind union_kind() const = 0;

    TypeSpecifierKind kind() const override { return TypeSpecifierKind::Enum; }
};

class UnionTypeSpecifierBody : public Node {
public:
    UnionTypeSpecifierBody(LCurly lcurly,
                           const std::vector<VariableDeclarations> decls,
                           RCurly rcurly)
        : lcurly_(lcurly), decls_(decls), rcurly_(rcurly) {}

    inline const LCurly& lcurly() const { return lcurly_; }

    inline const std::vector<VariableDeclarations>& decls() const {
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
    const std::vector<VariableDeclarations> decls_;
    const RCurly rcurly_;
};

class NamedUnionTypeSpecifierName : public Node {
public:
    NamedUnionTypeSpecifierName(const std::string& name, Span span)
        : name_(name), span_(span) {}

    inline const std::string& name() const { return name_; }

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return name_; }

private:
    const std::string name_;
    const Span span_;
};

class NamedUnionTypeSpecifier : public UnionTypeSpecifier {
public:
    NamedUnionTypeSpecifier(Union union_kw,
                            const NamedUnionTypeSpecifierName& name)
        : union_kw_(union_kw), name_(name), body_(std::nullopt) {}

    NamedUnionTypeSpecifier(Union union_kw,
                            const NamedUnionTypeSpecifierName& name,
                            UnionTypeSpecifierBody body)
        : union_kw_(union_kw), name_(name), body_(body) {}

    const Union& union_kw() const {
        return union_kw_;
        ;
    }

    const NamedUnionTypeSpecifierName& name() const { return name_; }

    const std::optional<UnionTypeSpecifierBody>& body() const { return body_; }

    inline UnionTypeSpecifierKind union_kind() const override {
        return UnionTypeSpecifierKind::Named;
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
    const NamedUnionTypeSpecifierName name_;
    const std::optional<UnionTypeSpecifierBody> body_;
};

class AnonymousUnionTypeSpecifier : public UnionTypeSpecifier {
public:
    AnonymousUnionTypeSpecifier(Union union_kw, UnionTypeSpecifierBody body)
        : union_kw_(union_kw), body_(body) {}

    const Union& union_kw() const { return union_kw_; }

    const UnionTypeSpecifierBody& body() const { return body_; }

    inline UnionTypeSpecifierKind union_kind() const override {
        return UnionTypeSpecifierKind::Anonymous;
    }

    inline Span span() const override {
        return concat_spans({union_kw_.span(), body_.span()});
    }

    inline std::string debug() const override {
        return union_kw_.debug() + " " + body_.debug();
    }

private:
    const Union union_kw_;
    const UnionTypeSpecifierBody body_;
};

}  // namespace tinyc

#endif  // TINYC_PARSER_TYPE_SPECIFIER_UNION_H_
