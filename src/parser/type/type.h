#ifndef TINYC_PARSER_TYPE_H_
#define TINYC_PARSER_TYPE_H_

#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../node.h"
#include "specifier/specifier.h"

namespace tinyc {

class Expression;

enum class TypeKind {
    Concrete,
    Pointer,
    Array,
    Function,
};

class Type : public Node {
public:
    virtual ~Type() {}
    virtual TypeKind kind() const = 0;
};

class ConcreteType : public Type {
public:
    ConcreteType(const std::shared_ptr<TypeSpecifier>& specifier,
                 const std::optional<Const>& const_kw,
                 const std::optional<Volatile>& volatile_kw)
        : specifier_(specifier),
          const_kw_(const_kw),
          volatile_kw_(volatile_kw) {}

    inline const std::shared_ptr<TypeSpecifier>& specifier() const {
        return specifier_;
    }

    inline const std::optional<Const>& const_kw() { return const_kw_; }

    inline const std::optional<Volatile>& volatile_kw() { return volatile_kw_; }

    bool is_const() const { return const_kw_.has_value(); }

    bool is_volatile() const { return volatile_kw_.has_value(); }

    inline TypeKind kind() const override { return TypeKind::Concrete; }

    Span span() const override {
        std::vector<Span> spans;
        spans.emplace_back(specifier_->span());
        if (const_kw_.has_value()) spans.emplace_back(const_kw_->span());
        if (volatile_kw_.has_value()) spans.emplace_back(volatile_kw_->span());
        return concat_spans(spans);
    }

    std::string debug() const override {
        std::stringstream ss;
        if (const_kw_.has_value()) ss << const_kw_->debug() << " ";
        if (volatile_kw_.has_value()) ss << volatile_kw_->debug() << " ";
        ss << specifier_->debug();
        return ss.str();
    }

private:
    const std::shared_ptr<TypeSpecifier> specifier_;
    const std::optional<Const> const_kw_;
    const std::optional<Volatile> volatile_kw_;
};

class PointerType : public Type {
public:
    PointerType(Star star, const std::optional<Const>& const_kw,
                const std::optional<Volatile>& volatile_kw,
                const std::shared_ptr<Type>& of)
        : star_(star),
          const_kw_(const_kw),
          volatile_kw_(volatile_kw),
          of_(of) {}

    inline const Star& star() const { return star_; }

    inline const std::optional<Const>& const_kw() { return const_kw_; }

    inline const std::optional<Volatile>& volatile_kw() { return volatile_kw_; }

    inline const std::shared_ptr<Type>& of() const { return of_; }

    bool is_const() const { return const_kw_.has_value(); }

    bool is_volatile() const { return volatile_kw_.has_value(); }

    inline TypeKind kind() const override { return TypeKind::Pointer; }

    inline Span span() const override {
        std::vector<Span> spans;
        spans.emplace_back(star_.span());
        if (const_kw_.has_value()) spans.emplace_back(const_kw_->span());
        if (volatile_kw_.has_value()) spans.emplace_back(volatile_kw_->span());
        spans.emplace_back(of_->span());
        return concat_spans(spans);
    }

    std::string debug() const override {
        std::stringstream ss;
        ss << "(";
        ss << of_->debug() + " ";
        ss << star_.debug();
        if (const_kw_.has_value()) ss << " " << const_kw_->debug();
        if (volatile_kw_.has_value()) ss << " " << volatile_kw_->debug();
        ss << ")";
        return ss.str();
    }

private:
    const Star star_;
    const std::optional<Const> const_kw_;
    const std::optional<Volatile> volatile_kw_;
    const std::shared_ptr<Type> of_;
};

class ArrayType : public Type {
public:
    ArrayType(const std::shared_ptr<Type>& of, LSquare lsquare, RSquare rsquare)
        : of_(of), lsquare_(lsquare), size_(std::nullopt), rsquare_(rsquare) {}

    ArrayType(const std::shared_ptr<Type>& of, LSquare lsquare,
              const std::shared_ptr<Expression>& size, RSquare rsquare)
        : of_(of), lsquare_(lsquare), size_(size), rsquare_(rsquare) {}

    inline const std::shared_ptr<Type>& of() const { return of_; }

    inline const LSquare& lsquare() const { return lsquare_; }

    inline const std::optional<std::shared_ptr<Expression>>& size() const {
        return size_;
    }

    inline const RSquare& rsquare() const { return rsquare_; }

    inline TypeKind kind() const override { return TypeKind::Array; }

    Span span() const override;

    std::string debug() const override;

private:
    const std::shared_ptr<Type> of_;
    const LSquare lsquare_;
    const std::optional<std::shared_ptr<Expression>> size_;
    const RSquare rsquare_;
};

class FunctionParamName : public Node {
public:
    FunctionParamName(const std::string& name, Span span)
        : name_(name), span_(span) {}

    inline const std::string& name() const { return name_; }

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return name_; }

private:
    const std::string name_;
    const Span span_;
};

class FunctionParam : public Node {
public:
    FunctionParam(const std::shared_ptr<Type>& type)
        : type_(type), name_(std::nullopt) {}

    FunctionParam(const std::shared_ptr<Type>& type,
                  const FunctionParamName& name)
        : type_(type), name_(name) {}

    inline const std::shared_ptr<Type>& type() const { return type_; }

    inline const std::optional<FunctionParamName>& name() const {
        return name_;
    }

    Span span() const override {
        if (name_.has_value())
            return concat_spans({type_->span(), name_->span()});
        else
            return type_->span();
    }

    std::string debug() const override {
        if (name_.has_value())
            return type_->debug() + " " + name_->debug();
        else
            return type_->debug();
    }

private:
    const std::shared_ptr<Type> type_;
    const std::optional<FunctionParamName> name_;
};

class FunctionType : public Type {
public:
    FunctionType(const std::shared_ptr<Type>& ret_type, LParen lparen,
                 const std::vector<FunctionParam>& params, RParen rparen)
        : ret_type_(ret_type),
          lparen_(lparen),
          params_(params),
          rparen_(rparen) {}

    inline const std::shared_ptr<Type>& ret_type() const { return ret_type_; }

    inline const LParen& lparen() const { return lparen_; }

    inline const std::vector<FunctionParam>& params() const { return params_; }

    inline const RParen& rparen() const { return rparen_; }

    inline TypeKind kind() const override { return TypeKind::Function; }

    Span span() const override {
        std::vector<Span> spans;
        spans.emplace_back(ret_type_->span());
        spans.emplace_back(lparen_.span());
        for (const auto& param : params_) spans.emplace_back(param.span());
        spans.emplace_back(rparen_.span());
        return concat_spans(spans);
    }

    inline std::string debug() const override {
        std::stringstream ss;
        ss << ret_type_->debug() << " " << lparen_.debug();
        if (params_.empty()) {
            ss << rparen_.debug();
        } else {
            ss << params_[0].debug();
            for (int i = 1; i < params_.size(); i++)
                ss << ", " << params_[i].debug();
            ss << rparen_.debug();
        }
        return ss.str();
    }

private:
    const std::shared_ptr<Type> ret_type_;
    const LParen lparen_;
    const std::vector<FunctionParam> params_;
    const RParen rparen_;
};

}  // namespace tinyc

#endif  // TINYC_PARSER_TYPE_H_
