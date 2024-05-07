#ifndef TINYC_PARSER_TYPE_SPECIFIER_STRUCT_H_
#define TINYC_PARSER_TYPE_SPECIFIER_STRUCT_H_

#include <optional>
#include <sstream>

#include "../../../span.h"
#include "../../decl.h"
#include "../../node.h"
#include "specifier.h"

namespace tinyc {

enum class StructTypeSpecifierKind {
    Named,
    Anonymous,
};

class StructTypeSpecifier : public TypeSpecifier {
public:
    virtual ~StructTypeSpecifier() {}
    virtual StructTypeSpecifierKind struct_kind() const = 0;

    TypeSpecifierKind kind() const override { return TypeSpecifierKind::Enum; }
};

class StructTypeSpecifierBody : public Node {
public:
    StructTypeSpecifierBody(LCurly lcurly,
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

class NamedStructTypeSpecifierName : public Node {
public:
    NamedStructTypeSpecifierName(const std::string& name, Span span)
        : name_(name), span_(span) {}

    inline const std::string& name() const { return name_; }

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return name_; }

private:
    const std::string name_;
    const Span span_;
};

class NamedStructTypeSpecifier : public StructTypeSpecifier {
public:
    NamedStructTypeSpecifier(Struct struct_kw,
                             const NamedStructTypeSpecifierName& name)
        : struct_kw_(struct_kw), name_(name), body_(std::nullopt) {}

    NamedStructTypeSpecifier(Struct struct_kw,
                             const NamedStructTypeSpecifierName& name,
                             StructTypeSpecifierBody body)
        : struct_kw_(struct_kw), name_(name), body_(body) {}

    const Struct& struct_kw() const {
        return struct_kw_;
        ;
    }

    const NamedStructTypeSpecifierName& name() const { return name_; }

    const std::optional<StructTypeSpecifierBody>& body() const { return body_; }

    inline StructTypeSpecifierKind struct_kind() const override {
        return StructTypeSpecifierKind::Named;
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
    const NamedStructTypeSpecifierName name_;
    const std::optional<StructTypeSpecifierBody> body_;
};

class AnonymousStructTypeSpecifier : public StructTypeSpecifier {
public:
    AnonymousStructTypeSpecifier(Struct struct_kw, StructTypeSpecifierBody body)
        : struct_kw_(struct_kw), body_(body) {}

    const Struct& struct_kw() const { return struct_kw_; }

    const StructTypeSpecifierBody& body() const { return body_; }

    inline StructTypeSpecifierKind struct_kind() const override {
        return StructTypeSpecifierKind::Anonymous;
    }

    inline Span span() const override {
        return concat_spans({struct_kw_.span(), body_.span()});
    }

    inline std::string debug() const override {
        return struct_kw_.debug() + " " + body_.debug();
    }

private:
    const Struct struct_kw_;
    const StructTypeSpecifierBody body_;
};

}  // namespace tinyc

#endif  // TINYC_PARSER_TYPE_SPECIFIER_STRUCT_H_
