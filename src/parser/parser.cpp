#include "parser.h"

#include <memory>
#include <optional>

#include "../lexer/lexer.h"
#include "../lexer/token.h"
#include "expr.h"
#include "stmt.h"
#include "type.h"

// TODO: complete implementation

namespace tinyc {

// Check if given token stream reach to end or not and throw error if it reach
// to end.
void check(TokenStream& ts) {
    if (ts.eos()) {
        throw ParseError("expected token after this", ts.last()->span());
    }
}

// Check if current token is expected and if it not throw error.
// `s` is used in error message.
void check(TokenStream& ts, TokenKind kind, const std::string& s) {
    check(ts);
    if (ts.token()->kind() != kind) {
        if (ts.retrest()) {
            auto span = ts.token()->span();
            ts.advance();
            throw ParseError("expected " + s + " after this", span);
        } else {
            auto span = ts.token()->span();
            throw ParseError("expected " + s + " before this", span);
        }
    }
}

// ========== type parser ==========

std::shared_ptr<Type> parse_type(TokenStream& ts);
std::shared_ptr<IntType> parse_int_type(TokenStream& ts);

std::shared_ptr<Type> parse_type(TokenStream& ts) { return parse_int_type(ts); }

std::shared_ptr<IntType> parse_int_type(TokenStream& ts) {
    check(ts);
    if (ts.token()->kind() == TokenKind::Int) {
        auto span = ts.token()->span();
        ts.advance();
        return std::make_shared<IntType>(span);
    } else {
        throw ParseError("expected int", ts.token()->span());
    }
}

// ========== expression parser ==========

std::shared_ptr<Expression> parse_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_assign_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_cond_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_logical_or_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_logical_and_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_or_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_xor_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_and_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_equality_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_relative_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_shift_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_additive_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_multiplicative_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_cast_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_unary_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_postfix_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_primary_expr(TokenStream& ts);

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

    std::shared_ptr<Expression> lhs;
    try {
        lhs = parse_unary_expr(ts);
    } catch (ParseError e) {
        ts.set_state(state);
        return parse_cond_expr(ts);
    }

    if (ts.eos()) {
        ts.set_state(state);
        return parse_cond_expr(ts);
    }
    InfixExpressionOpKind kind;
    try {
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
    } catch (ParseError e) {
        ts.set_state(state);
        return parse_cond_expr(ts);
    }
    InfixExpressionOp op(kind, ts.token()->span());
    ts.advance();

    auto rhs = parse_assign_expr(ts);
    return std::make_shared<InfixExpression>(lhs, rhs, op);
}

std::shared_ptr<Expression> parse_cond_expr(TokenStream& ts) {
    auto cond = parse_logical_or_expr(ts);

    if (ts.eos() || ts.token()->kind() != TokenKind::Not) {
        return cond;
    }
    auto exclamation = ConditionalExpressionExclamationOp(ts.token()->span());
    ts.advance();

    auto then = parse_expr(ts);

    check(ts, TokenKind::Colon, ":");
    auto colon = ConditionalExpressionColonOp(ts.token()->span());
    ts.advance();

    auto otherwise = parse_cond_expr(ts);
    return std::make_shared<ConditionalExpression>(cond, then, otherwise,
                                                   exclamation, colon);
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

std::shared_ptr<Expression> parse_logical_and_expr(TokenStream& ts) {
    auto lhs = parse_or_expr(ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::And) {
            InfixExpressionOp op(InfixExpressionOpKind::And,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_or_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, rhs, op);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_or_expr(TokenStream& ts) {
    auto lhs = parse_xor_expr(ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::Vertical) {
            InfixExpressionOp op(InfixExpressionOpKind::BitOr,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_xor_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, rhs, op);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_xor_expr(TokenStream& ts) {
    auto lhs = parse_and_expr(ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::Hat) {
            InfixExpressionOp op(InfixExpressionOpKind::BitXor,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_and_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, rhs, op);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_and_expr(TokenStream& ts) {
    auto lhs = parse_equality_expr(ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::Ampersand) {
            InfixExpressionOp op(InfixExpressionOpKind::BitAnd,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_equality_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, rhs, op);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_equality_expr(TokenStream& ts) {
    auto lhs = parse_relative_expr(ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::EQ) {
            InfixExpressionOp op(InfixExpressionOpKind::EQ, ts.token()->span());
            ts.advance();
            auto rhs = parse_relative_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, rhs, op);
        } else if (ts.token()->kind() == TokenKind::NE) {
            InfixExpressionOp op(InfixExpressionOpKind::NE, ts.token()->span());
            ts.advance();
            auto rhs = parse_relative_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, rhs, op);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_relative_expr(TokenStream& ts) {
    auto lhs = parse_shift_expr(ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::LT) {
            InfixExpressionOp op(InfixExpressionOpKind::LT, ts.token()->span());
            ts.advance();
            auto rhs = parse_shift_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, rhs, op);
        } else if (ts.token()->kind() == TokenKind::GT) {
            InfixExpressionOp op(InfixExpressionOpKind::GT, ts.token()->span());
            ts.advance();
            auto rhs = parse_shift_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, rhs, op);
        } else if (ts.token()->kind() == TokenKind::LE) {
            InfixExpressionOp op(InfixExpressionOpKind::LE, ts.token()->span());
            ts.advance();
            auto rhs = parse_shift_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, rhs, op);
        } else if (ts.token()->kind() == TokenKind::GE) {
            InfixExpressionOp op(InfixExpressionOpKind::GE, ts.token()->span());
            ts.advance();
            auto rhs = parse_shift_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, rhs, op);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_shift_expr(TokenStream& ts) {
    auto lhs = parse_additive_expr(ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::LShift) {
            InfixExpressionOp op(InfixExpressionOpKind::LShift,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_additive_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, rhs, op);
        } else if (ts.token()->kind() == TokenKind::RShift) {
            InfixExpressionOp op(InfixExpressionOpKind::RShift,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_additive_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, rhs, op);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_additive_expr(TokenStream& ts) {
    auto lhs = parse_multiplicative_expr(ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::Plus) {
            InfixExpressionOp op(InfixExpressionOpKind::Add,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_multiplicative_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, rhs, op);
        } else if (ts.token()->kind() == TokenKind::Minus) {
            InfixExpressionOp op(InfixExpressionOpKind::Sub,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_multiplicative_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, rhs, op);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_multiplicative_expr(TokenStream& ts) {
    auto lhs = parse_cast_expr(ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::Star) {
            InfixExpressionOp op(InfixExpressionOpKind::Mul,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_cast_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, rhs, op);
        } else if (ts.token()->kind() == TokenKind::Slash) {
            InfixExpressionOp op(InfixExpressionOpKind::Div,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_cast_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, rhs, op);
        } else if (ts.token()->kind() == TokenKind::Percent) {
            InfixExpressionOp op(InfixExpressionOpKind::Mod,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_cast_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, rhs, op);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_cast_expr(TokenStream& ts) {
    check(ts);
    auto state = ts.state();
    try {
        if (ts.token()->kind() != TokenKind::LParen) {
            return parse_unary_expr(ts);
        }
        CastExpressionLParen lparen(ts.token()->span());
        ts.advance();

        auto type = parse_type(ts);

        check(ts, TokenKind::RParen, ")");
        CastExpressionRParen rparen(ts.token()->span());
        ts.advance();

        auto expr = parse_unary_expr(ts);

        return std::make_shared<CastExpression>(type, expr, lparen, rparen);
    } catch (ParseError e) {
        ts.set_state(state);
        return parse_unary_expr(ts);
    }
}

std::shared_ptr<Expression> parse_unary_expr(TokenStream& ts) {
    check(ts);
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

            check(ts, TokenKind::LParen, "(");
            SizeofTypeExpressionLParen lparen(ts.token()->span());
            ts.advance();

            auto type = parse_type(ts);

            check(ts, TokenKind::RParen, ")");
            SizeofTypeExpressionRParen rparen(ts.token()->span());
            ts.advance();

            return std::make_shared<SizeofTypeExpression>(keyword, type, lparen,
                                                          rparen);
        }
    } else {
        return parse_postfix_expr(ts);
    }
}

std::shared_ptr<Expression> parse_postfix_expr(TokenStream& ts) {
    auto lhs = parse_primary_expr(ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::LSquare) {
            IndexingExpressionLSquare lsquare(ts.token()->span());
            ts.advance();

            auto index = parse_expr(ts);

            check(ts, TokenKind::RSquare, "]");
            IndexingExpressionRSquare rsquare(ts.token()->span());
            ts.advance();

            lhs = std::make_shared<IndexingExpression>(lhs, index, lsquare,
                                                       rsquare);
        } else if (ts.token()->kind() == TokenKind::LParen) {
            CallingExpressionLParen lparen(ts.token()->span());
            ts.advance();

            std::vector<std::shared_ptr<Expression>> params;
            while (true) {
                if (ts.token()->kind() == TokenKind::RParen) {
                    CallingExpressionRParen rparen(ts.token()->span());
                    lhs = std::make_shared<CallingExpression>(lhs, params,
                                                              lparen, rparen);
                    ts.advance();
                    break;
                }

                params.push_back(parse_assign_expr(ts));

                check(ts);
                if (ts.token()->kind() == TokenKind::RParen) {
                    CallingExpressionRParen rparen(ts.token()->span());
                    lhs = std::make_shared<CallingExpression>(lhs, params,
                                                              lparen, rparen);
                    ts.advance();
                    break;
                } else if (ts.token()->kind() == TokenKind::Comma) {
                    ts.advance();
                } else {
                    throw ParseError("expected ) or ,", ts.token()->span());
                }
            }
        } else if (ts.token()->kind() == TokenKind::Dot) {
            AccessExpressionOp op(AccessExpressionOpKind::Dot,
                                  ts.token()->span());
            ts.advance();

            check(ts, TokenKind::Identifier, "identifier");
            auto tk =
                std::static_pointer_cast<ValueToken<std::string>>(ts.token());
            auto name = tk->value();
            AccessExpressionField field(name, ts.token()->span());
            ts.advance();

            lhs = std::make_shared<AccessExpression>(lhs, op, field);
        } else if (ts.token()->kind() == TokenKind::Arrow) {
            AccessExpressionOp op(AccessExpressionOpKind::Arrow,
                                  ts.token()->span());
            ts.advance();

            check(ts, TokenKind::Identifier, "identifier");
            auto tk =
                std::static_pointer_cast<ValueToken<std::string>>(ts.token());
            auto name = tk->value();
            AccessExpressionField field(name, ts.token()->span());
            ts.advance();

            lhs = std::make_shared<AccessExpression>(lhs, op, field);
        } else if (ts.token()->kind() == TokenKind::PlusPlus) {
            PostfixExpressionOp op(PostfixExpressionOpKind::Inc,
                                   ts.token()->span());
            ts.advance();
            lhs = std::make_shared<PostfixExpression>(lhs, op);
        } else if (ts.token()->kind() == TokenKind::MinusMinus) {
            PostfixExpressionOp op(PostfixExpressionOpKind::Dec,
                                   ts.token()->span());
            ts.advance();
            lhs = std::make_shared<PostfixExpression>(lhs, op);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_primary_expr(TokenStream& ts) {
    check(ts);
    if (ts.token()->kind() == TokenKind::Identifier) {
        auto tk = std::static_pointer_cast<ValueToken<std::string>>(ts.token());
        auto value = tk->value();
        auto span = tk->span();
        ts.advance();
        return std::make_shared<IdentifierExpression>(value, span);
    } else if (ts.token()->kind() == TokenKind::Integer) {
        auto tk = std::static_pointer_cast<ValueToken<long long>>(ts.token());
        auto value = tk->value();
        auto span = tk->span();
        ts.advance();
        return std::make_shared<IntegerExpression>(value, span);
    } else if (ts.token()->kind() == TokenKind::LParen) {
        SurroundedExpressionLParen lparen(ts.token()->span());
        ts.advance();

        auto expr = parse_expr(ts);

        check(ts, TokenKind::RParen, ")");
        SurroundedExpressionRParen rparen(ts.token()->span());
        ts.advance();

        return std::make_shared<SurroundedExpression>(expr, lparen, rparen);
    } else {
        throw ParseError("expected one of identifier, integer or (",
                         ts.token()->span());
    }
}

// ========== statement parser ==========

std::shared_ptr<Statement> parse_stmt(TokenStream& ts);
std::shared_ptr<Statement> parse_labeled_stmt(TokenStream& ts);
std::shared_ptr<Statement> parse_case_stmt(TokenStream& ts);
std::shared_ptr<Statement> parse_default_stmt(TokenStream& ts);
std::shared_ptr<Statement> parse_block_stmt(TokenStream& ts);
std::shared_ptr<Statement> parse_if_stmt(TokenStream& ts);
std::shared_ptr<Statement> parse_switch_stmt(TokenStream& ts);
std::shared_ptr<Statement> parse_while_stmt(TokenStream& ts);
std::shared_ptr<Statement> parse_do_while_stmt(TokenStream& ts);
std::shared_ptr<Statement> parse_for_stmt(TokenStream& ts);
std::shared_ptr<Statement> parse_goto_stmt(TokenStream& ts);
std::shared_ptr<Statement> parse_continue_stmt(TokenStream& ts);
std::shared_ptr<Statement> parse_break_stmt(TokenStream& ts);
std::shared_ptr<Statement> parse_return_stmt(TokenStream& ts);
std::shared_ptr<Statement> parse_expr_stmt(TokenStream& ts);

std::shared_ptr<Statement> parse_stmt(TokenStream& ts) {
    check(ts);
    if (ts.token()->kind() == TokenKind::Identifier) {
        auto state = ts.state();
        ts.advance();
        if (!ts.eos() && ts.token()->kind() == TokenKind::Colon) {
            return parse_labeled_stmt(ts);
        } else {
            ts.set_state(state);
            return parse_expr_stmt(ts);
        }
    } else if (ts.token()->kind() == TokenKind::Case) {
        return parse_case_stmt(ts);
    } else if (ts.token()->kind() == TokenKind::Default) {
        return parse_default_stmt(ts);
    } else if (ts.token()->kind() == TokenKind::LCurly) {
        return parse_block_stmt(ts);
    } else if (ts.token()->kind() == TokenKind::If) {
        return parse_if_stmt(ts);
    } else if (ts.token()->kind() == TokenKind::Switch) {
        return parse_switch_stmt(ts);
    } else if (ts.token()->kind() == TokenKind::While) {
        return parse_while_stmt(ts);
    } else if (ts.token()->kind() == TokenKind::Do) {
        return parse_do_while_stmt(ts);
    } else if (ts.token()->kind() == TokenKind::For) {
        return parse_for_stmt(ts);
    } else if (ts.token()->kind() == TokenKind::Goto) {
        return parse_goto_stmt(ts);
    } else if (ts.token()->kind() == TokenKind::Break) {
        return parse_break_stmt(ts);
    } else if (ts.token()->kind() == TokenKind::Return) {
        return parse_return_stmt(ts);
    } else {
        return parse_expr_stmt(ts);
    }
}

std::shared_ptr<Statement> parse_labeled_stmt(TokenStream& ts) {
    check(ts, TokenKind::Identifier, "identifier");
    auto tk = std::static_pointer_cast<ValueToken<std::string>>(ts.token());
    LabeledStatementLabel label(tk->value(), tk->span());
    ts.advance();

    check(ts, TokenKind::Colon, ":");
    LabeledStatementColon colon(ts.token()->span());
    ts.advance();

    auto stmt = parse_stmt(ts);

    return std::make_shared<LabeledStatement>(label, colon, stmt);
}

std::shared_ptr<Statement> parse_case_stmt(TokenStream& ts) {
    check(ts, TokenKind::Case, "case");
    CaseStatementCaseKeyword keyword(ts.token()->span());
    ts.advance();

    auto expr = parse_expr(ts);

    check(ts, TokenKind::Colon, ":");
    CaseStatementColon colon(ts.token()->span());
    ts.advance();

    auto stmt = parse_stmt(ts);

    return std::make_shared<CaseStatement>(keyword, expr, colon, stmt);
}

std::shared_ptr<Statement> parse_default_stmt(TokenStream& ts) {
    check(ts, TokenKind::Default, "default");
    DefaultStatementDefaultKeyword keyword(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::Colon, ":");
    DefaultStatementColon colon(ts.token()->span());
    ts.advance();

    auto stmt = parse_stmt(ts);

    return std::make_shared<DefaultStatement>(keyword, colon, stmt);
}

std::shared_ptr<Statement> parse_block_stmt(TokenStream& ts) {
    check(ts, TokenKind::LCurly, "{");
    BlockStatementLCurly lcurly(ts.token()->span());
    ts.advance();

    std::vector<std::shared_ptr<Statement>> stmts;
    while (true) {
        if (ts.token()->kind() == TokenKind::RCurly) {
            BlockStatementRCurly rcurly(ts.token()->span());
            ts.advance();
            return std::make_shared<BlockStatement>(lcurly, stmts, rcurly);
        }

        stmts.push_back(parse_stmt(ts));
    }
}

std::shared_ptr<Statement> parse_if_stmt(TokenStream& ts) {
    check(ts, TokenKind::If, "if");
    IfStatementIfKeyword if_keyword(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::LParen, "(");
    IfStatementLParen lparen(ts.token()->span());
    ts.advance();

    auto cond = parse_expr(ts);

    check(ts, TokenKind::RParen, ")");
    IfStatementRParen rparen(ts.token()->span());
    ts.advance();

    auto then = parse_stmt(ts);

    if (ts.eos() || ts.token()->kind() != TokenKind::Else) {
        return std::make_shared<IfStatement>(if_keyword, lparen, cond, rparen,
                                             then, std::nullopt);
    }
    IfStatementElseKeyword else_keyword(ts.token()->span());
    ts.advance();

    auto otherwise = parse_stmt(ts);

    std::optional<IfStatementElseBody> else_body =
        IfStatementElseBody(else_keyword, otherwise);

    return std::make_shared<IfStatement>(if_keyword, lparen, cond, rparen, then,
                                         else_body);
}

std::shared_ptr<Statement> parse_switch_stmt(TokenStream& ts) {
    check(ts, TokenKind::Switch, "switch");
    SwitchStatementSwitchKeyword switch_keyword(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::LParen, "(");
    SwitchStatementLParen lparen(ts.token()->span());
    ts.advance();

    auto comp = parse_expr(ts);

    check(ts, TokenKind::RParen, ")");
    SwitchStatementRParen rparen(ts.token()->span());
    ts.advance();

    auto body = parse_stmt(ts);

    return std::make_shared<SwitchStatement>(switch_keyword, lparen, comp,
                                             rparen, body);
}

std::shared_ptr<Statement> parse_while_stmt(TokenStream& ts) {
    check(ts, TokenKind::While, "while");
    WhileStatementWhileKeyword switch_keyword(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::LParen, "(");
    WhileStatementLParen lparen(ts.token()->span());
    ts.advance();

    auto cond = parse_expr(ts);

    check(ts, TokenKind::RParen, ")");
    WhileStatementRParen rparen(ts.token()->span());
    ts.advance();

    auto body = parse_stmt(ts);

    return std::make_shared<WhileStatement>(switch_keyword, lparen, cond,
                                            rparen, body);
}

std::shared_ptr<Statement> parse_do_while_stmt(TokenStream& ts) {
    check(ts, TokenKind::Do, "do");
    DoWhileStatementDoKeyword do_keyword(ts.token()->span());
    ts.advance();

    auto body = parse_stmt(ts);

    check(ts, TokenKind::While, "while");
    DoWhileStatementWhileKeyword while_keyword(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::LParen, "(");
    DoWhileStatementLParen lparen(ts.token()->span());
    ts.advance();

    auto cond = parse_expr(ts);

    check(ts, TokenKind::RParen, ")");
    DoWhileStatementRParen rparen(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::Semicolon, ";");
    DoWhileStatementSemicolon semicolon(ts.token()->span());
    ts.advance();

    return std::make_shared<DoWhileStatement>(do_keyword, body, while_keyword,
                                              lparen, cond, rparen, semicolon);
}

std::shared_ptr<Statement> parse_for_stmt(TokenStream& ts) {
    check(ts, TokenKind::For, "for");
    ForStatementForKeyword for_keyword(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::LParen, "(");
    ForStatementLParen lparen(ts.token()->span());
    ts.advance();

    std::optional<std::shared_ptr<Expression>> init = std::nullopt;
    check(ts);
    if (ts.token()->kind() != TokenKind::Semicolon) {
        init = parse_expr(ts);
    }
    check(ts, TokenKind::Semicolon, ";");
    ForStatementSemicolon semicolon1(ts.token()->span());
    ts.advance();

    std::optional<std::shared_ptr<Expression>> cond = std::nullopt;
    check(ts);
    if (ts.token()->kind() != TokenKind::Semicolon) {
        cond = parse_expr(ts);
    }
    check(ts, TokenKind::Semicolon, ";");
    ForStatementSemicolon semicolon2(ts.token()->span());
    ts.advance();

    std::optional<std::shared_ptr<Expression>> update = std::nullopt;
    check(ts);
    if (ts.token()->kind() != TokenKind::RParen) {
        update = parse_expr(ts);
    }
    check(ts, TokenKind::RParen, ")");
    ForStatementRParen rparen(ts.token()->span());
    ts.advance();

    auto body = parse_stmt(ts);

    return std::make_shared<ForStatement>(for_keyword, lparen, init, semicolon1,
                                          cond, semicolon2, update, rparen,
                                          body);
}

std::shared_ptr<Statement> parse_goto_stmt(TokenStream& ts) {
    check(ts, TokenKind::Goto, "goto");
    GotoStatementGotoKeyword goto_keyword(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::Identifier, "identifier");
    auto tk = std::static_pointer_cast<ValueToken<std::string>>(ts.token());
    GotoStatementLabel label(tk->value(), tk->span());
    ts.advance();

    check(ts, TokenKind::Semicolon, ";");
    GotoStatementSemicolon semicolon(ts.token()->span());
    ts.advance();

    return std::make_shared<GotoStatement>(goto_keyword, label, semicolon);
}

std::shared_ptr<Statement> parse_continue_stmt(TokenStream& ts) {
    check(ts, TokenKind::Continue, "continue");
    ContinueStatementContinueKeyword continue_keyword(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::Semicolon, ";");
    ContinueStatementSemicolon semicolon(ts.token()->span());
    ts.advance();

    return std::make_shared<ContinueStatement>(continue_keyword, semicolon);
}

std::shared_ptr<Statement> parse_break_stmt(TokenStream& ts) {
    check(ts, TokenKind::Break, "break");
    BreakStatementBreakKeyword break_keyword(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::Semicolon, ";");
    BreakStatementSemicolon semicolon(ts.token()->span());
    ts.advance();

    return std::make_shared<BreakStatement>(break_keyword, semicolon);
}

std::shared_ptr<Statement> parse_return_stmt(TokenStream& ts) {
    check(ts, TokenKind::Return, "return");
    ReturnStatementReturnKeyword return_keyword(ts.token()->span());
    ts.advance();

    auto ret = parse_expr(ts);

    check(ts, TokenKind::Semicolon, ";");
    ReturnStatementSemicolon semicolon(ts.token()->span());
    ts.advance();

    return std::make_shared<ReturnStatement>(return_keyword, ret, semicolon);
}

std::shared_ptr<Statement> parse_expr_stmt(TokenStream& ts) {
    check(ts);
    if (ts.token()->kind() == TokenKind::Semicolon) {
        EmptyStatementSemicolon semicolon(ts.token()->span());
        ts.advance();
        return std::make_shared<EmptyStatement>(semicolon);
    } else {
        auto expr = parse_expr(ts);

        check(ts, TokenKind::Semicolon, ";");
        ExpressionStatementSemicolon semicolon(ts.token()->span());
        ts.advance();

        return std::make_shared<ExpressionStatement>(expr, semicolon);
    }
}

}  // namespace tinyc
