#ifndef TINYC_PARSER_TYPE_SPECIFIER_ENUM_H_
#define TINYC_PARSER_TYPE_SPECIFIER_ENUM_H_

#include <memory>
#include <optional>
#include <sstream>
#include <vector>

#include "../../expr.h"
#include "../../node.h"
#include "specifier.h"

namespace tinyc {

enum class EnumTypeSpecifierKind {
    Named,
    Anonymous,
};

class EnumTypeSpecifier : public TypeSpecifier {
public:
    virtual ~EnumTypeSpecifier() {}
    virtual EnumTypeSpecifierKind enum_kind() const = 0;

    TypeSpecifierKind kind() const override { return TypeSpecifierKind::Enum; }
};

class EnumeratorIdent : public Node {
public:
    EnumeratorIdent(const std::string& name, Span span)
        : name_(name), span_(span) {}

    inline const std::string& name() const { return name_; }

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return name_; }

private:
    const std::string name_;
    const Span span_;
};

class EnumeratorInit : public Node {
public:
    EnumeratorInit(Assign assign, const std::shared_ptr<Expression>& expr)
        : assign_(assign), expr_(expr) {}

    inline const Assign& assign() const { return assign_; }

    inline const std::shared_ptr<Expression>& expr() const { return expr_; }

    inline Span span() const override {
        return concat_spans({assign_.span(), expr_->span()});
    }

    inline std::string debug() const override {
        return assign_.debug() + " " + expr_->debug();
    }

private:
    const Assign assign_;
    const std::shared_ptr<Expression> expr_;
};

class Enumerator : public Node {
public:
    Enumerator(const EnumeratorIdent& ident)
        : ident_(ident), init_(std::nullopt), comma_(std::nullopt) {}

    Enumerator(const EnumeratorIdent& ident, const EnumeratorInit& init)
        : ident_(ident), init_(init), comma_(std::nullopt) {}

    Enumerator(const EnumeratorIdent& ident, Comma comma)
        : ident_(ident), init_(std::nullopt), comma_(comma) {}

    Enumerator(const EnumeratorIdent& ident, const EnumeratorInit& init,
               Comma comma)
        : ident_(ident), init_(init), comma_(comma) {}

    const EnumeratorIdent& ident() const { return ident_; }

    const std::optional<EnumeratorInit>& init() const { return init_; }

    const std::optional<Comma>& comma() const { return comma_; }

    inline Span span() const override {
        std::vector<Span> spans;
        spans.emplace_back(ident_.span());
        if (init_.has_value()) {
            spans.emplace_back(init_->span());
        }
        if (comma_.has_value()) {
            spans.emplace_back(comma_->span());
        }
        return concat_spans(spans);
    }

    inline std::string debug() const override {
        std::stringstream ss;
        ss << ident_.debug();
        if (init_.has_value()) {
            ss << " " << init_->debug();
        }
        if (comma_.has_value()) {
            ss << comma_->debug();
        }
        return ss.str();
    }

private:
    const EnumeratorIdent ident_;
    const std::optional<EnumeratorInit> init_;
    const std::optional<Comma> comma_;
};

class EnumTypeSpecifierBody : public Node {
public:
    EnumTypeSpecifierBody(LCurly lcurly,
                          const std::vector<Enumerator> enumerators,
                          RCurly rcurly)
        : lcurly_(lcurly), enumerators_(enumerators), rcurly_(rcurly) {}

    inline const LCurly& lcurly() const { return lcurly_; }

    inline const std::vector<Enumerator>& enumerators() const {
        return enumerators_;
    }

    inline const RCurly& rcurly() const { return rcurly_; }

    Span span() const override {
        std::vector<Span> spans;
        spans.emplace_back(lcurly_.span());
        for (const auto& enumerator : enumerators_) {
            spans.emplace_back(enumerator.span());
        }
        spans.emplace_back(rcurly_.span());
        return concat_spans(spans);
    }

    std::string debug() const override {
        std::stringstream ss;
        ss << lcurly_.debug();
        for (const auto& enumerator : enumerators_) {
            ss << " " << enumerator.debug();
        }
        ss << " " << rcurly_.debug();
        return ss.str();
    }

private:
    const LCurly lcurly_;
    const std::vector<Enumerator> enumerators_;
    const RCurly rcurly_;
};

class NamedEnumTypeSpecifierName : public Node {
public:
    NamedEnumTypeSpecifierName(const std::string& name, Span span)
        : name_(name), span_(span) {}

    inline const std::string& name() const { return name_; }

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return name_; }

private:
    const std::string name_;
    const Span span_;
};

class NamedEnumTypeSpecifier : public EnumTypeSpecifier {
public:
    NamedEnumTypeSpecifier(Enum enum_kw, const NamedEnumTypeSpecifierName& name)
        : enum_kw_(enum_kw), name_(name), body_(std::nullopt) {}

    NamedEnumTypeSpecifier(Enum enum_kw, const NamedEnumTypeSpecifierName& name,
                           const EnumTypeSpecifierBody& body)
        : enum_kw_(enum_kw), name_(name), body_(body) {}

    inline const Enum& enum_kw() const { return enum_kw_; }

    inline const NamedEnumTypeSpecifierName& name() const { return name_; }

    inline const std::optional<EnumTypeSpecifierBody>& body() const {
        return body_;
    }

    inline EnumTypeSpecifierKind enum_kind() const override {
        return EnumTypeSpecifierKind::Named;
    }

    Span span() const override {
        if (body_.has_value()) {
            return concat_spans({enum_kw_.span(), name_.span(), body_->span()});
        } else {
            return concat_spans({enum_kw_.span(), name_.span()});
        }
    }

    std::string debug() const override {
        if (body_.has_value()) {
            return enum_kw_.debug() + " " + name_.debug() + " " +
                   body_->debug();
        } else {
            return enum_kw_.debug() + " " + name_.debug();
        }
    }

private:
    const Enum enum_kw_;
    const NamedEnumTypeSpecifierName name_;
    const std::optional<EnumTypeSpecifierBody> body_;
};

class AnonymousEnumTypeSpecifier : public EnumTypeSpecifier {
public:
    AnonymousEnumTypeSpecifier(Enum enum_kw, EnumTypeSpecifierBody body)
        : enum_kw_(enum_kw), body_(body) {}

    const Enum& enum_kw() const { return enum_kw_; }

    const EnumTypeSpecifierBody& body() const { return body_; }

    inline EnumTypeSpecifierKind enum_kind() const override {
        return EnumTypeSpecifierKind::Anonymous;
    }

    Span span() const override {
        return concat_spans({enum_kw_.span(), body_.span()});
    }

    std::string debug() const override {
        return enum_kw_.debug() + " " + body_.debug();
    }

private:
    const Enum enum_kw_;
    const EnumTypeSpecifierBody body_;
};

}  // namespace tinyc

#endif  // TINYC_PARSER_TYPE_SPECIFIER_ENUM_H_
