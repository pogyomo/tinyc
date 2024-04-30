#ifndef TINYC_PARSER_EXPR_H_
#define TINYC_PARSER_EXPR_H_

#include <memory>
#include <sstream>
#include <vector>

#include "../span.h"
#include "node.h"
#include "type.h"

namespace tinyc {

enum class ExpressionKind {
    Unary,
    Infix,
    Postfix,
    Access,
    Indexing,
    Calling,
    Conditional,
    SizeofExpr,
    SizeofType,
    Cast,
    Integer,
    Identifier,
    Surrounded,
};

class Expression : public Node {
public:
    virtual ~Expression() {}
    virtual ExpressionKind kind() const = 0;
};

enum class UnaryExpressionOpKind {
    Inc,     // ++
    Dec,     // --
    Deref,   // *
    Ref,     // &
    Plus,    // +
    Minus,   // -
    BitInv,  // ~
    Neg,     // !
};

class UnaryExpressionOp : public Node {
public:
    UnaryExpressionOp(UnaryExpressionOpKind kind, Span span)
        : kind_(kind), span_(span) {}

    inline const UnaryExpressionOpKind& kind() const { return kind_; }

    inline Span span() const override { return span_; }

    std::string debug() const override;

private:
    const UnaryExpressionOpKind kind_;
    const Span span_;
};

class UnaryExpression : public Expression {
public:
    UnaryExpression(UnaryExpressionOp op,
                    const std::shared_ptr<Expression>& expr)
        : op_(op), expr_(expr) {}

    inline const UnaryExpressionOp& op() const { return op_; }

    inline const std::shared_ptr<Expression>& expr() const { return expr_; }

    inline ExpressionKind kind() const override {
        return ExpressionKind::Unary;
    }

    inline Span span() const override {
        return concat_spans({op_.span(), expr_->span()});
    }

    inline std::string debug() const override {
        return "(" + op_.debug() + expr_->debug() + ")";
    }

private:
    const UnaryExpressionOp op_;
    const std::shared_ptr<Expression> expr_;
};

enum class InfixExpressionOpKind {
    Comma,         // ,
    Or,            // ||
    And,           // &&
    BitOr,         // |
    BitXor,        // ^
    BitAnd,        // &
    EQ,            // ==
    NE,            // !=
    LT,            // <
    GT,            // >
    LE,            // <=
    GE,            // >=
    LShift,        // <<
    RShift,        // >>
    Add,           // +
    Sub,           // -
    Mul,           // *
    Div,           // /
    Mod,           // %
    Assign,        // =
    OrAssign,      // |=
    XorAssign,     // ^=
    AndAssign,     // &=
    LShiftAssign,  // <<=
    RShiftAssign,  // >>=
    AddAssign,     // +=
    SubAssign,     // -=
    MulAssign,     // *=
    DivAssign,     // /=
    ModAssign,     // %=
};

class InfixExpressionOp : public Node {
public:
    InfixExpressionOp(InfixExpressionOpKind kind, Span span)
        : kind_(kind), span_(span) {}

    inline const InfixExpressionOpKind& kind() const { return kind_; }

    inline Span span() const override { return span_; }

    std::string debug() const override;

private:
    const InfixExpressionOpKind kind_;
    const Span span_;
};

class InfixExpression : public Expression {
public:
    InfixExpression(const std::shared_ptr<Expression>& lhs,
                    const std::shared_ptr<Expression>& rhs,
                    InfixExpressionOp op)
        : lhs_(lhs), rhs_(rhs), op_(op) {}

    inline const std::shared_ptr<Expression>& lhs() const { return lhs_; }

    inline const std::shared_ptr<Expression>& rhs() const { return rhs_; }

    inline const InfixExpressionOp& op() const { return op_; }

    inline ExpressionKind kind() const override {
        return ExpressionKind::Infix;
    }

    inline Span span() const override {
        return concat_spans({lhs_->span(), rhs_->span(), op_.span()});
    }

    inline std::string debug() const override {
        auto lhs = lhs_->debug();
        auto rhs = rhs_->debug();
        auto op = op_.debug();
        return "(" + lhs + " " + op + " " + rhs + ")";
    }

private:
    const std::shared_ptr<Expression> lhs_;
    const std::shared_ptr<Expression> rhs_;
    const InfixExpressionOp op_;
};

enum class PostfixExpressionOpKind {
    Inc,  // ++
    Dec,  // --
};

class PostfixExpressionOp : public Node {
public:
    PostfixExpressionOp(PostfixExpressionOpKind kind, Span span)
        : kind_(kind), span_(span) {}

    inline const PostfixExpressionOpKind& kind() const { return kind_; }

    inline Span span() const override { return span_; }

    std::string debug() const override;

private:
    const PostfixExpressionOpKind kind_;
    const Span span_;
};

class PostfixExpression : public Expression {
public:
    PostfixExpression(const std::shared_ptr<Expression>& expr,
                      PostfixExpressionOp op)
        : expr_(expr), op_(op) {}

    inline const std::shared_ptr<Expression>& expr() const { return expr_; }

    inline const PostfixExpressionOp& op() const { return op_; }

    inline ExpressionKind kind() const override {
        return ExpressionKind::Postfix;
    }

    inline Span span() const override {
        return concat_spans({expr_->span(), op_.span()});
    }

    inline std::string debug() const override {
        return "(" + expr_->debug() + op_.debug() + ")";
    }

private:
    const std::shared_ptr<Expression> expr_;
    const PostfixExpressionOp op_;
};

class AccessExpressionField : public Node {
public:
    AccessExpressionField(const std::string& name, Span span)
        : name_(name), span_(span) {}

    inline const std::string& name() const { return name_; }

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return name_; }

private:
    const std::string name_;
    const Span span_;
};

enum class AccessExpressionOpKind {
    Dot,
    Arrow,
};

class AccessExpressionOp : public Node {
public:
    AccessExpressionOp(AccessExpressionOpKind kind, Span span)
        : kind_(kind), span_(span) {}

    inline const AccessExpressionOpKind& kind() const { return kind_; }

    inline Span span() const override { return span_; }

    inline std::string debug() const override {
        if (kind_ == AccessExpressionOpKind::Dot) {
            return ".";
        } else {
            return "->";
        }
    }

private:
    const AccessExpressionOpKind kind_;
    const Span span_;
};

class AccessExpression : public Expression {
public:
    AccessExpression(const std::shared_ptr<Expression>& expr,
                     AccessExpressionOp op, AccessExpressionField field)
        : expr_(expr), op_(op), field_(field) {}

    inline const std::shared_ptr<Expression>& expr() const { return expr_; }

    inline const AccessExpressionOp& op() const { return op_; }

    inline const AccessExpressionField& field() const { return field_; }

    inline ExpressionKind kind() const override {
        return ExpressionKind::Access;
    }

    inline Span span() const override {
        return concat_spans({expr_->span(), op_.span(), field_.span()});
    }

    inline std::string debug() const override {
        return "(" + expr_->debug() + op_.debug() + field_.debug() + ")";
    }

private:
    const std::shared_ptr<Expression> expr_;
    const AccessExpressionOp op_;
    const AccessExpressionField field_;
};

class IndexingExpressionLSquare : public Node {
public:
    IndexingExpressionLSquare(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "["; }

private:
    const Span span_;
};

class IndexingExpressionRSquare : public Node {
public:
    IndexingExpressionRSquare(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "]"; }

private:
    const Span span_;
};

class IndexingExpression : public Expression {
public:
    IndexingExpression(const std::shared_ptr<Expression>& expr,
                       IndexingExpressionLSquare lsquare,
                       const std::shared_ptr<Expression>& index,
                       IndexingExpressionRSquare rsquare)
        : expr_(expr), lsquare_(lsquare), index_(index), rsquare_(rsquare) {}

    inline const std::shared_ptr<Expression>& expr() const { return expr_; }

    inline const IndexingExpressionLSquare& lsquare() const { return lsquare_; }

    inline const std::shared_ptr<Expression>& index() const { return index_; }

    inline const IndexingExpressionRSquare& rsquare() const { return rsquare_; }

    inline ExpressionKind kind() const override {
        return ExpressionKind::Indexing;
    }

    inline Span span() const override {
        return concat_spans(
            {expr_->span(), lsquare_.span(), index_->span(), rsquare_.span()});
    }

    inline std::string debug() const override {
        auto expr = expr_->debug();
        auto lsquare = lsquare_.debug();
        auto index = index_->debug();
        auto rsquare = rsquare_.debug();
        return expr + lsquare + index + rsquare;
    }

private:
    const std::shared_ptr<Expression> expr_;
    const IndexingExpressionLSquare lsquare_;
    const std::shared_ptr<Expression> index_;
    const IndexingExpressionRSquare rsquare_;
};

class CallingExpressionLParen : public Node {
public:
    CallingExpressionLParen(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "("; }

private:
    const Span span_;
};

class CallingExpressionRParen : public Node {
public:
    CallingExpressionRParen(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return ")"; }

private:
    const Span span_;
};

class CallingExpression : public Expression {
public:
    CallingExpression(const std::shared_ptr<Expression>& expr,
                      CallingExpressionLParen lparen,
                      const std::vector<std::shared_ptr<Expression>>& params,
                      CallingExpressionRParen rparen)
        : expr_(expr), lparen_(lparen), params_(params), rparen_(rparen) {}

    inline const std::shared_ptr<Expression>& expr() const { return expr_; }

    inline const std::vector<std::shared_ptr<Expression>>& params() const {
        return params_;
    }

    inline const CallingExpressionLParen& lparen() const { return lparen_; }

    inline const CallingExpressionRParen& rparen() const { return rparen_; }

    inline ExpressionKind kind() const override {
        return ExpressionKind::Calling;
    }

    Span span() const override {
        std::vector<Span> spans;
        spans.push_back(expr_->span());
        spans.push_back(lparen_.span());
        for (const auto param : params_) {
            spans.push_back(param->span());
        }
        spans.push_back(rparen_.span());
        return concat_spans(spans);
    }

    std::string debug() const override {
        if (params_.size() == 0) {
            return expr_->debug() + "()";
        }
        std::stringstream ss;
        ss << expr_->debug() + "(" << params_[0]->debug();
        for (int i = 1; i < params_.size(); i++) {
            ss << ", " << params_[i]->debug();
        }
        ss << ")";
        return ss.str();
    }

private:
    const std::shared_ptr<Expression> expr_;
    const CallingExpressionLParen lparen_;
    const std::vector<std::shared_ptr<Expression>> params_;
    const CallingExpressionRParen rparen_;
};

class ConditionalExpressionExclamationOp : public Node {
public:
    ConditionalExpressionExclamationOp(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "?"; }

private:
    Span span_;
};

class ConditionalExpressionColonOp : public Node {
public:
    ConditionalExpressionColonOp(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return ":"; }

private:
    Span span_;
};

class ConditionalExpression : public Expression {
public:
    ConditionalExpression(const std::shared_ptr<Expression>& cond,
                          ConditionalExpressionExclamationOp exclamation,
                          const std::shared_ptr<Expression>& then,
                          ConditionalExpressionColonOp colon,
                          const std::shared_ptr<Expression>& otherwise)
        : cond_(cond),
          exclamation_(exclamation),
          then_(then),
          colon_(colon),
          otherwise_(otherwise) {}

    inline const std::shared_ptr<Expression>& cond() const { return cond_; }

    inline const ConditionalExpressionExclamationOp& exclamation() const {
        return exclamation_;
    }

    inline const std::shared_ptr<Expression>& then() const { return then_; }

    inline const ConditionalExpressionColonOp& colon() const { return colon_; }

    inline const std::shared_ptr<Expression>& otherwise() const {
        return otherwise_;
    }

    inline ExpressionKind kind() const override {
        return ExpressionKind::Conditional;
    };

    inline Span span() const override {
        return concat_spans({cond_->span(), exclamation_.span(), then_->span(),
                             colon_.span(), otherwise_->span()});
    }

    inline std::string debug() const override {
        std::stringstream ss;
        ss << "(" << cond_->debug() << " " << exclamation_.debug() << " "
           << then_->debug() << " " << colon_.debug() << " "
           << otherwise_->debug() << ")";
        return ss.str();
    }

private:
    const std::shared_ptr<Expression> cond_;
    const ConditionalExpressionExclamationOp exclamation_;
    const std::shared_ptr<Expression> then_;
    const ConditionalExpressionColonOp colon_;
    const std::shared_ptr<Expression> otherwise_;
};

class SizeofExprExpressionSizeofKeyword : public Node {
public:
    SizeofExprExpressionSizeofKeyword(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "sizeof"; }

private:
    const Span span_;
};

class SizeofExprExpression : public Expression {
public:
    SizeofExprExpression(SizeofExprExpressionSizeofKeyword sizeof_keyword,
                         std::shared_ptr<Expression> expr)
        : sizeof_keyword_(sizeof_keyword), expr_(expr) {}

    inline const SizeofExprExpressionSizeofKeyword& sizeof_keyword() const {
        return sizeof_keyword_;
    }

    inline const std::shared_ptr<Expression>& expr() const { return expr_; };

    inline ExpressionKind kind() const override {
        return ExpressionKind::SizeofExpr;
    }

    inline Span span() const override {
        return concat_spans({sizeof_keyword_.span(), expr_->span()});
    }

    inline std::string debug() const override {
        return "(" + sizeof_keyword_.debug() + " " + expr_->debug() + ")";
    }

private:
    const SizeofExprExpressionSizeofKeyword sizeof_keyword_;
    const std::shared_ptr<Expression> expr_;
};

class SizeofTypeExpressionSizeofKeyword : public Node {
public:
    SizeofTypeExpressionSizeofKeyword(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "sizeof"; }

private:
    const Span span_;
};

class SizeofTypeExpressionLParen : public Node {
public:
    SizeofTypeExpressionLParen(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "("; }

private:
    const Span span_;
};

class SizeofTypeExpressionRParen : public Node {
public:
    SizeofTypeExpressionRParen(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return ")"; }

private:
    const Span span_;
};

class SizeofTypeExpression : public Expression {
public:
    SizeofTypeExpression(SizeofTypeExpressionSizeofKeyword sizeof_keyword,
                         SizeofTypeExpressionLParen lparen,
                         std::shared_ptr<Type> type,
                         SizeofTypeExpressionRParen rparen)
        : sizeof_keyword_(sizeof_keyword),
          lparen_(lparen),
          type_(type),
          rparen_(rparen) {}

    inline const SizeofTypeExpressionSizeofKeyword& sizeof_keyword() const {
        return sizeof_keyword_;
    }

    inline const SizeofTypeExpressionLParen& lparen() const { return lparen_; }

    inline const std::shared_ptr<Type>& type() const { return type_; };

    inline const SizeofTypeExpressionRParen& rparen() const { return rparen_; }

    inline ExpressionKind kind() const override {
        return ExpressionKind::SizeofType;
    }

    inline Span span() const override {
        return concat_spans({sizeof_keyword_.span(), lparen_.span(),
                             type_->span(), rparen_.span()});
    }

    std::string debug() const override {
        auto sizeof_keyword = sizeof_keyword_.debug();
        auto lparen = lparen_.debug();
        auto type = type_->debug();
        auto rparen = rparen_.debug();
        return "(" + sizeof_keyword + " " + lparen + type + rparen + ")";
    }

private:
    const SizeofTypeExpressionSizeofKeyword sizeof_keyword_;
    const SizeofTypeExpressionLParen lparen_;
    const std::shared_ptr<Type> type_;
    const SizeofTypeExpressionRParen rparen_;
};

class CastExpressionLParen : public Node {
public:
    CastExpressionLParen(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "("; }

private:
    const Span span_;
};

class CastExpressionRParen : public Node {
public:
    CastExpressionRParen(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return ")"; }

private:
    const Span span_;
};

class CastExpression : public Expression {
public:
    CastExpression(CastExpressionLParen lparen,
                   const std::shared_ptr<Type> type,
                   CastExpressionRParen rparen,
                   const std::shared_ptr<Expression> expr)
        : lparen_(lparen), type_(type), rparen_(rparen), expr_(expr) {}

    inline const CastExpressionLParen& lparen() const { return lparen_; }

    inline const std::shared_ptr<Type>& type() const { return type_; }

    inline const CastExpressionRParen& rparen() const { return rparen_; }

    inline const std::shared_ptr<Expression>& expr() const { return expr_; }

    inline ExpressionKind kind() const override { return ExpressionKind::Cast; }

    inline Span span() const override {
        return concat_spans(
            {lparen_.span(), type_->span(), rparen_.span(), expr_->span()});
    }

    std::string debug() const override {
        auto lparen = lparen_.debug();
        auto type = type_->debug();
        auto rparen = rparen_.debug();
        auto expr = expr_->debug();
        return lparen + type + rparen + expr;
    }

private:
    const CastExpressionLParen lparen_;
    const std::shared_ptr<Type> type_;
    const CastExpressionRParen rparen_;
    const std::shared_ptr<Expression> expr_;
};

class IntegerExpression : public Expression {
public:
    IntegerExpression(long long value, Span span)
        : value_(value), span_(span) {}

    inline long long value() const { return value_; }

    inline ExpressionKind kind() const override {
        return ExpressionKind::Integer;
    }

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return std::to_string(value_); }

private:
    const long long value_;
    const Span span_;
};

class IdentifierExpression : public Expression {
public:
    IdentifierExpression(const std::string& value, Span span)
        : value_(value), span_(span) {}

    inline const std::string& value() const { return value_; }

    inline ExpressionKind kind() const override {
        return ExpressionKind::Identifier;
    }

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return value_; }

private:
    const std::string value_;
    const Span span_;
};

class SurroundedExpressionLParen : public Node {
public:
    SurroundedExpressionLParen(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return "("; }

private:
    const Span span_;
};

class SurroundedExpressionRParen : public Node {
public:
    SurroundedExpressionRParen(Span span) : span_(span) {}

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return ")"; }

private:
    const Span span_;
};

class SurroundedExpression : public Expression {
public:
    SurroundedExpression(SurroundedExpressionLParen lparen,
                         const std::shared_ptr<Expression>& expr,
                         SurroundedExpressionRParen rparen)
        : lparen_(lparen), expr_(expr), rparen_(rparen) {}

    const SurroundedExpressionLParen& lparen() const { return lparen_; }

    const std::shared_ptr<Expression>& expr() const { return expr_; }

    const SurroundedExpressionRParen& rparen() const { return rparen_; }

    inline ExpressionKind kind() const override {
        return ExpressionKind::Surrounded;
    }

    inline Span span() const override {
        return concat_spans({lparen_.span(), expr_->span(), rparen_.span()});
    }

    inline std::string debug() const override {
        return "(" + expr_->debug() + ")";
    }

private:
    const SurroundedExpressionLParen lparen_;
    const std::shared_ptr<Expression> expr_;
    const SurroundedExpressionRParen rparen_;
};

}  // namespace tinyc

#endif  // TINYC_PARSER_EXPR_H_
