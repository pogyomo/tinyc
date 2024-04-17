#ifndef TINYC_EXPR_H_
#define TINYC_EXPR_H_

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "../span.h"

namespace tinyc {

enum class ExpressionKind {
    Unary,
    Infix,
    Postfix,
    Access,
    Indexing,
    Calling,
    Conditional,
    Integer,
    Identifier,
};

class Expression {
public:
    virtual ~Expression() {}
    virtual ExpressionKind kind() const = 0;
    virtual Span span() const = 0;
    virtual std::string debug() const = 0;
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

class UnaryExpressionOp {
public:
    UnaryExpressionOp(UnaryExpressionOpKind kind, Span span)
        : kind_(kind), span_(span) {}

    // Returns the kind of this unary operator.
    inline const UnaryExpressionOpKind& kind() const { return kind_; }

    // Returns the position of this operator in source code.
    inline const Span& span() const { return span_; }

    // Returns debugging string.
    std::string debug() const;

private:
    const UnaryExpressionOpKind kind_;
    const Span span_;
};

// A class represent an expression `op expr`.
class UnaryExpression : public Expression {
public:
    // Construct a new `UnaryExpression` with lhs, rhs and op.
    UnaryExpression(const std::shared_ptr<Expression>& expr,
                    UnaryExpressionOp op)
        : expr_(expr), op_(op) {}

    inline const std::shared_ptr<Expression>& expr() const { return expr_; }

    inline const UnaryExpressionOp& op() const { return op_; }

    // Returns `ExpressionKind::Unary`.
    inline ExpressionKind kind() const override {
        return ExpressionKind::Unary;
    }

    inline Span span() const override {
        return Span(op_.span().start(), expr_->span().end());
    }

    inline std::string debug() const override {
        auto expr = expr_->debug();
        auto op = op_.debug();
        return "(" + op + expr + ")";
    }

private:
    const std::shared_ptr<Expression> expr_;
    const UnaryExpressionOp op_;
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

class InfixExpressionOp {
public:
    InfixExpressionOp(InfixExpressionOpKind kind, Span span)
        : kind_(kind), span_(span) {}

    // Returns the kind of this infix operator.
    inline const InfixExpressionOpKind& kind() const { return kind_; }

    // Returns the position of this operator in source code.
    inline const Span& span() const { return span_; }

    // Returns debugging string.
    std::string debug() const;

private:
    const InfixExpressionOpKind kind_;
    const Span span_;
};

// A class represent an expression `lhs op rhs`.
class InfixExpression : public Expression {
public:
    // Construct a new `InfixExpression` with expr and op.
    InfixExpression(const std::shared_ptr<Expression>& lhs,
                    const std::shared_ptr<Expression>& rhs,
                    InfixExpressionOp op)
        : lhs_(lhs), rhs_(rhs), op_(op) {}

    inline const std::shared_ptr<Expression>& lhs() const { return lhs_; }

    inline const std::shared_ptr<Expression>& rhs() const { return rhs_; }

    inline const InfixExpressionOp& op() const { return op_; }

    // Returns `ExpressionKind::Infix`.
    inline ExpressionKind kind() const override {
        return ExpressionKind::Infix;
    }

    inline Span span() const override {
        return Span(lhs_->span().start(), rhs_->span().end());
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

class PostfixExpressionOp {
public:
    PostfixExpressionOp(PostfixExpressionOpKind kind, Span span)
        : kind_(kind), span_(span) {}

    // Returns the kind of this postfix operator.
    inline const PostfixExpressionOpKind& kind() const { return kind_; }

    // Returns the position of this operator in source code.
    inline const Span& span() const { return span_; }

    // Returns debugging string.
    std::string debug() const;

private:
    const PostfixExpressionOpKind kind_;
    const Span span_;
};

// A class represent an expression `expr op`.
class PostfixExpression : public Expression {
public:
    // Construct a new `PostfixExpression` with expr and op.
    PostfixExpression(const std::shared_ptr<Expression>& expr,
                      PostfixExpressionOp op)
        : expr_(expr), op_(op) {}

    inline const std::shared_ptr<Expression>& expr() const { return expr_; }

    inline const PostfixExpressionOp& op() const { return op_; }

    // Returns `ExpressionKind::Postfix`.
    inline ExpressionKind kind() const override {
        return ExpressionKind::Postfix;
    }

    inline Span span() const override {
        return Span(expr_->span().start(), op_.span().end());
    }

    inline std::string debug() const override {
        auto expr = expr_->debug();
        auto op = op_.debug();
        return "(" + expr + op + ")";
    }

private:
    const std::shared_ptr<Expression> expr_;
    const PostfixExpressionOp op_;
};

class AccessExpressionField {
public:
    // Construct a new `AccessExpressionField` with its name and span.
    AccessExpressionField(const std::string& name, Span span)
        : name_(name), span_(span) {}

    // Returns this field's name.
    inline const std::string& name() const { return name_; }

    // Returns this field's span.
    inline const Span& span() const { return span_; }

    // Returns debugging string.
    inline std::string debug() const { return name_; }

private:
    const std::string name_;
    const Span span_;
};

enum class AccessExpressionOpKind {
    Dot,
    Arrow,
};

class AccessExpressionOp {
public:
    // Construct a new `AccessExpressionOp` with its kind and span.
    AccessExpressionOp(AccessExpressionOpKind kind, Span span)
        : kind_(kind), span_(span) {}

    // Returns its kind.
    inline const AccessExpressionOpKind& kind() const { return kind_; }

    // Returns its span.
    inline const Span& span() const { return span_; }

    // Returns debugging string.
    inline std::string debug() const {
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

// A class represent an expression `expr.field`.
class AccessExpression : public Expression {
public:
    // Construct a new `AccessExpression` with expr and accessing field.
    AccessExpression(const std::shared_ptr<Expression>& expr,
                     AccessExpressionOp op, AccessExpressionField field)
        : expr_(expr), op_(op), field_(field) {}

    // Returns expr in `expr.field`.
    const std::shared_ptr<Expression>& expr() const { return expr_; }

    // Returns . in `expr.field`.
    const AccessExpressionOp& op() const { return op_; }

    // Returns field in `expr.field`.
    const AccessExpressionField& field() const { return field_; }

    // Returns `ExpressionKind::Access`.
    inline ExpressionKind kind() const override {
        return ExpressionKind::Access;
    }

    inline Span span() const override {
        return Span(expr_->span().start(), field_.span().end());
    }

    inline std::string debug() const override {
        return "(" + expr_->debug() + op_.debug() + field_.debug() + ")";
    }

private:
    const std::shared_ptr<Expression> expr_;
    const AccessExpressionOp op_;
    const AccessExpressionField field_;
};

class IndexingExpressionLSquare {
public:
    IndexingExpressionLSquare(Span span) : span_(span) {}

    const Span& span() const { return span_; }

private:
    const Span span_;
};

class IndexingExpressionRSquare {
public:
    IndexingExpressionRSquare(Span span) : span_(span) {}

    const Span& span() const { return span_; }

private:
    const Span span_;
};

class IndexingExpression : public Expression {
public:
    IndexingExpression(const std::shared_ptr<Expression>& expr,
                       const std::shared_ptr<Expression>& index,
                       IndexingExpressionLSquare lsquare,
                       IndexingExpressionRSquare rsquare)
        : expr_(expr), index_(index), lsquare_(lsquare), rsquare_(rsquare) {}

    // Returns expr in `expr[index]`.
    const std::shared_ptr<Expression>& expr() const { return expr_; }

    // Returns index in `expr[index]`.
    const std::shared_ptr<Expression>& index() const { return index_; }

    // Returns [ in `expr[index]`.
    const IndexingExpressionLSquare& lsquare() const { return lsquare_; }

    // Returns ] in `expr[index]`.
    const IndexingExpressionRSquare& rsquare() const { return rsquare_; }

    // Returns `ExpressionKind::Indexing`.
    inline ExpressionKind kind() const override {
        return ExpressionKind::Indexing;
    }

    inline Span span() const override {
        return Span(expr_->span().start(), rsquare_.span().end());
    }

    inline std::string debug() const override {
        return expr_->debug() + "[" + index_->debug() + "]";
    }

private:
    const std::shared_ptr<Expression> expr_;
    const std::shared_ptr<Expression> index_;
    const IndexingExpressionLSquare lsquare_;
    const IndexingExpressionRSquare rsquare_;
};

class CallingExpressionLParen {
public:
    CallingExpressionLParen(Span span) : span_(span) {}

    const Span& span() const { return span_; }

private:
    const Span span_;
};

class CallingExpressionRParen {
public:
    CallingExpressionRParen(Span span) : span_(span) {}

    const Span& span() const { return span_; }

private:
    const Span span_;
};

// A class represent an expression `expr(params)`.
class CallingExpression : public Expression {
public:
    CallingExpression(const std::shared_ptr<Expression>& expr,
                      const std::vector<std::shared_ptr<Expression>>& params,
                      CallingExpressionLParen lparen,
                      CallingExpressionRParen rparen)
        : expr_(expr), params_(params), lparen_(lparen), rparen_(rparen) {}

    // Returns expr in `expr(params)`.
    const std::shared_ptr<Expression>& expr() const { return expr_; }

    // Returns params in `expr(params)`.
    const std::vector<std::shared_ptr<Expression>>& params() const {
        return params_;
    }

    // Returns ( in `expr(params)`.
    const CallingExpressionLParen& lparen() const { return lparen_; }

    // Returns ) in `expr(params)`.
    const CallingExpressionRParen& rparen() const { return rparen_; }

    // Returns `ExpressionKind::Calling`.
    inline ExpressionKind kind() const override {
        return ExpressionKind::Calling;
    }

    inline Span span() const override {
        return Span(expr_->span().start(), rparen_.span().end());
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
    const std::vector<std::shared_ptr<Expression>> params_;
    const CallingExpressionLParen lparen_;
    const CallingExpressionRParen rparen_;
};

class ConditionalExpressionExclamationOp {
public:
    ConditionalExpressionExclamationOp(Span span) : span_(span) {}

    const Span& span() const { return span_; }

private:
    Span span_;
};

class ConditionalExpressionColonOp {
public:
    ConditionalExpressionColonOp(Span span) : span_(span) {}

    const Span& span() const { return span_; }

private:
    Span span_;
};

class ConditionalExpression : public Expression {
public:
    ConditionalExpression(const std::shared_ptr<Expression>& cond,
                          const std::shared_ptr<Expression>& then,
                          const std::shared_ptr<Expression>& otherwise,
                          ConditionalExpressionExclamationOp exclamation,
                          ConditionalExpressionColonOp colon)
        : cond_(cond),
          then_(then),
          otherwise_(otherwise),
          exclamation_(exclamation),
          colon_(colon) {}

    // Returns cond in `cond ? then : otherwise`.
    const std::shared_ptr<Expression>& cond() const { return cond_; }

    // Returns then in `cond ? then : otherwise`.
    const std::shared_ptr<Expression>& then() const { return then_; }

    // Returns otherwise in `cond ? then : otherwise`.
    const std::shared_ptr<Expression>& otherwise() const { return otherwise_; }

    inline ExpressionKind kind() const override {
        return ExpressionKind::Conditional;
    };

    inline Span span() const override {
        return Span(cond_->span().start(), otherwise_->span().end());
    }

    inline std::string debug() const override {
        auto cond = cond_->debug();
        auto then = then_->debug();
        auto otherwise = otherwise_->debug();
        return "(" + cond + " ? " + then + " : " + otherwise + ")";
    }

private:
    const std::shared_ptr<Expression> cond_;
    const std::shared_ptr<Expression> then_;
    const std::shared_ptr<Expression> otherwise_;
    const ConditionalExpressionExclamationOp exclamation_;
    const ConditionalExpressionColonOp colon_;
};

// A class represent an integer expression.
class IntegerExpression : public Expression {
public:
    // Construct a new `IntegerExpression` with associated value and its span.
    IntegerExpression(long long value, Span span)
        : value_(value), span_(span) {}

    // Returns associated integer.
    inline long long value() const { return value_; }

    // Returns `ExpressionKind::Integer`.
    inline ExpressionKind kind() const override {
        return ExpressionKind::Integer;
    }

    // Returns span of this integer in source code.
    inline Span span() const override { return span_; }

    // Returns debugging string.
    inline std::string debug() const override { return std::to_string(value_); }

private:
    const long long value_;
    const Span span_;
};

// A class represent an identifier expression.
class IdentifierExpression : public Expression {
public:
    // Construct a new `IdentifierExpression` with associated value and its
    // span.
    IdentifierExpression(const std::string& value, Span span)
        : value_(value), span_(span) {}

    // Returns string represent this identifier.
    inline const std::string& value() const { return value_; }

    // Returns `ExpressionKind::Identifier`.
    inline ExpressionKind kind() const override {
        return ExpressionKind::Identifier;
    }

    // Returns span of this identifier in source code.
    inline Span span() const override { return span_; }

    // Returns debugging string.
    inline std::string debug() const override { return value_; }

private:
    const std::string value_;
    const Span span_;
};

}  // namespace tinyc

#endif  // TINYC_EXPR_H_
