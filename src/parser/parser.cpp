#include "parser.h"

#include <memory>

#include "../lexer/lexer.h"
#include "../lexer/token.h"
#include "expr.h"
#include "type.h"

// TODO: complete implementation

namespace tinyc {

#define EXPECT(ts, kind)

std::shared_ptr<Type> parse_type(TokenStream& ts) {
    if (ts.token()->kind() == TokenKind::Int) {
        auto span = ts.token()->span();
        ts.advance();
        return std::make_shared<IntType>(span);
    } else {
        throw ParseError("unexpected type", ts.token()->span());
    }
}

std::shared_ptr<Expression> parse_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_assign_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_cond_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_unary_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_logical_or_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_cast_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_postfix_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_logical_and_expr(TokenStream& ts);

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
    auto cond = parse_logical_or_expr(ts);

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

std::shared_ptr<Expression> parse_unary_expr(TokenStream& ts) {
    if (ts.eos()) {
        throw ParseError("expected token after this", ts.last()->span());
    }
    if (ts.token()->kind() == TokenKind::PlusPlus) {
        UnaryExpressionOp op(UnaryExpressionOpKind::Inc, ts.token()->span());
        ts.advance();
        auto expr = parse_unary_expr(ts);
        return std::make_shared<UnaryExpression>(expr, op);
    } else if (ts.token()->kind() == TokenKind::MinusMinus) {
        UnaryExpressionOp op(UnaryExpressionOpKind::Dec, ts.token()->span());
        ts.advance();
        auto expr = parse_unary_expr(ts);
        return std::make_shared<UnaryExpression>(expr, op);
    } else if (ts.token()->kind() == TokenKind::Ampersand) {
        UnaryExpressionOp op(UnaryExpressionOpKind::Ref, ts.token()->span());
        ts.advance();
        auto expr = parse_cast_expr(ts);
        return std::make_shared<UnaryExpression>(expr, op);
    } else if (ts.token()->kind() == TokenKind::Star) {
        UnaryExpressionOp op(UnaryExpressionOpKind::Deref, ts.token()->span());
        ts.advance();
        auto expr = parse_cast_expr(ts);
        return std::make_shared<UnaryExpression>(expr, op);
    } else if (ts.token()->kind() == TokenKind::Plus) {
        UnaryExpressionOp op(UnaryExpressionOpKind::Plus, ts.token()->span());
        ts.advance();
        auto expr = parse_cast_expr(ts);
        return std::make_shared<UnaryExpression>(expr, op);
    } else if (ts.token()->kind() == TokenKind::Minus) {
        UnaryExpressionOp op(UnaryExpressionOpKind::Minus, ts.token()->span());
        ts.advance();
        auto expr = parse_cast_expr(ts);
        return std::make_shared<UnaryExpression>(expr, op);
    } else if (ts.token()->kind() == TokenKind::Tilde) {
        UnaryExpressionOp op(UnaryExpressionOpKind::BitInv, ts.token()->span());
        ts.advance();
        auto expr = parse_cast_expr(ts);
        return std::make_shared<UnaryExpression>(expr, op);
    } else if (ts.token()->kind() == TokenKind::Not) {
        UnaryExpressionOp op(UnaryExpressionOpKind::Neg, ts.token()->span());
        ts.advance();
        auto expr = parse_cast_expr(ts);
        return std::make_shared<UnaryExpression>(expr, op);
    } else if (ts.token()->kind() == TokenKind::Sizeof) {
        SizeofKeyword keyword(ts.token()->span());
        ts.advance();
        auto state = ts.state();
        try {
            auto expr = parse_unary_expr(ts);
            return std::make_shared<SizeofExpressionExpression>(keyword, expr);
        } catch (ParseError e) {
            ts.set_state(state);
            if (ts.eos()) {
                throw ParseError("expected token after this",
                                 ts.last()->span());
            }
            if (ts.token()->kind() != TokenKind::LParen) {
                throw ParseError("expected (", ts.last()->span());
            }
            SizeofTypeExpressionLParen lparen(ts.token()->span());
            ts.advance();

            auto type = parse_type(ts);

            if (ts.eos()) {
                throw ParseError("expected token after this",
                                 ts.last()->span());
            }
            if (ts.token()->kind() != TokenKind::RParen) {
                throw ParseError("expected )", ts.last()->span());
            }
            SizeofTypeExpressionRParen rparen(ts.token()->span());
            ts.advance();

            return std::make_shared<SizeofTypeExpression>(keyword, type, lparen,
                                                          rparen);
        }
    } else {
        return parse_postfix_expr(ts);
    }
}

std::shared_ptr<Expression> parse_logical_or_expr(TokenStream& ts) {
    auto lhs = parse_logical_and_expr(ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::Or) {
            InfixExpressionOp op(InfixExpressionOpKind::Or, ts.token()->span());
            ts.advance();
            auto rhs = parse_logical_and_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, rhs, op);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_cast_expr(TokenStream& ts) {
    if (ts.eos()) {
        throw ParseError("expected token after this", ts.last()->span());
    }
    if (ts.token()->kind() != TokenKind::LParen) {
        throw ParseError("expected (", ts.token()->span());
    }
    CastExpressionLParen lparen(ts.token()->span());
    ts.advance();

    auto type = parse_type(ts);

    if (ts.eos()) {
        throw ParseError("expected token after this", ts.last()->span());
    }
    if (ts.token()->kind() != TokenKind::RParen) {
        throw ParseError("expected )", ts.token()->span());
    }
    CastExpressionRParen rparen(ts.token()->span());
    ts.advance();

    auto expr = parse_unary_expr(ts);

    return std::make_shared<CastExpression>(type, expr, lparen, rparen);
}

std::shared_ptr<Expression> parse_postfix_expr(TokenStream& ts) { return NULL; }

std::shared_ptr<Expression> parse_logical_and_expr(TokenStream& ts) {
    return NULL;
}

}  // namespace tinyc
