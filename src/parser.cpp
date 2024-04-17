#include "parser.h"

#include <memory>

#include "expr.h"
#include "lexer.h"
#include "token.h"

// TODO: complete implementation

namespace tinyc {

std::shared_ptr<Expression> parse_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_assign_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_cond_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_unary_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_logical_or(TokenStream& ts);

std::shared_ptr<Expression> parse_expr(TokenStream& ts) {
    auto lhs = parse_assign_expr(ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::Comma) {
            InfixExpressionOp op(InfixExpressionOpKind::Comma,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_assign_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, rhs, op);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_assign_expr(TokenStream& ts) {
    auto state = ts.state();
    try {
        return parse_cond_expr(ts);
    } catch (ParseError e) {
        auto lhs = parse_unary_expr(ts);
        if (ts.eos()) {
            throw ParseError("expected token after this", ts.last()->span());
        }
        InfixExpressionOpKind kind;
        switch (ts.token()->kind()) {
            case TokenKind::Assign:
                kind = InfixExpressionOpKind::Assign;
                break;
            case TokenKind::OrAssign:
                kind = InfixExpressionOpKind::OrAssign;
                break;
            case TokenKind::XorAssign:
                kind = InfixExpressionOpKind::XorAssign;
                break;
            case TokenKind::AndAssign:
                kind = InfixExpressionOpKind::AndAssign;
                break;
            case TokenKind::LShiftAssign:
                kind = InfixExpressionOpKind::LShiftAssign;
                break;
            case TokenKind::RShiftAssign:
                kind = InfixExpressionOpKind::RShiftAssign;
                break;
            case TokenKind::AddAssign:
                kind = InfixExpressionOpKind::AddAssign;
                break;
            case TokenKind::SubAssign:
                kind = InfixExpressionOpKind::SubAssign;
                break;
            case TokenKind::MulAssign:
                kind = InfixExpressionOpKind::MulAssign;
                break;
            case TokenKind::DivAssign:
                kind = InfixExpressionOpKind::DivAssign;
                break;
            case TokenKind::ModAssign:
                kind = InfixExpressionOpKind::ModAssign;
                break;
            default:
                throw ParseError("expected assign operator",
                                 ts.token()->span());
        }
        InfixExpressionOp op(kind, ts.token()->span());
        ts.advance();
        auto rhs = parse_assign_expr(ts);
        return std::make_shared<InfixExpression>(lhs, rhs, op);
    }
}

std::shared_ptr<Expression> parse_cond_expr(TokenStream& ts) {
    auto cond = parse_logical_or(ts);

    if (ts.eos()) {
        throw ParseError("expected token after this", ts.last()->span());
    }
    if (ts.token()->kind() != TokenKind::Not) {
        throw ParseError("expected !", ts.token()->span());
    }
    auto exclamation = ConditionalExpressionExclamationOp(ts.token()->span());
    ts.advance();

    auto then = parse_expr(ts);
    if (ts.eos()) {
        throw ParseError("expected token after this", ts.last()->span());
    }
    if (ts.token()->kind() != TokenKind::Colon) {
        throw ParseError("expected :", ts.token()->span());
    }
    auto colon = ConditionalExpressionColonOp(ts.token()->span());
    ts.advance();

    auto otherwise = parse_cond_expr(ts);
    return std::make_shared<ConditionalExpression>(cond, then, otherwise,
                                                   exclamation, colon);
}

}  // namespace tinyc
