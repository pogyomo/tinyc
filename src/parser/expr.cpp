#include "expr.h"

namespace tinyc {

std::string UnaryExpressionOp::debug() const {
    switch (kind_) {
        case UnaryExpressionOpKind::Inc:
            return "++";
        case UnaryExpressionOpKind::Dec:
            return "--";
        case UnaryExpressionOpKind::Deref:
            return "*";
        case UnaryExpressionOpKind::Ref:
            return "&";
        case UnaryExpressionOpKind::Plus:
            return "+";
        case UnaryExpressionOpKind::Minus:
            return "-";
        case UnaryExpressionOpKind::BitInv:
            return "~";
        case UnaryExpressionOpKind::Neg:
            return "!";
        default:
            return "";
    }
}

std::string InfixExpressionOp::debug() const {
    switch (kind_) {
        case InfixExpressionOpKind::Comma:
            return ",";
        case InfixExpressionOpKind::Or:
            return "||";
        case InfixExpressionOpKind::And:
            return "&&";
        case InfixExpressionOpKind::BitOr:
            return "|";
        case InfixExpressionOpKind::BitXor:
            return "^";
        case InfixExpressionOpKind::BitAnd:
            return "&";
        case InfixExpressionOpKind::EQ:
            return "==";
        case InfixExpressionOpKind::NE:
            return "!=";
        case InfixExpressionOpKind::LT:
            return "<";
        case InfixExpressionOpKind::GT:
            return ">";
        case InfixExpressionOpKind::LE:
            return "<=";
        case InfixExpressionOpKind::GE:
            return ">=";
        case InfixExpressionOpKind::LShift:
            return "<<";
        case InfixExpressionOpKind::RShift:
            return ">>";
        case InfixExpressionOpKind::Add:
            return "+";
        case InfixExpressionOpKind::Sub:
            return "-";
        case InfixExpressionOpKind::Mul:
            return "*";
        case InfixExpressionOpKind::Div:
            return "/";
        case InfixExpressionOpKind::Mod:
            return "*";
        case InfixExpressionOpKind::Assign:
            return "=";
        case InfixExpressionOpKind::OrAssign:
            return "|=";
        case InfixExpressionOpKind::XorAssign:
            return "^=";
        case InfixExpressionOpKind::AndAssign:
            return "&=";
        case InfixExpressionOpKind::LShiftAssign:
            return "<<=";
        case InfixExpressionOpKind::RShiftAssign:
            return ">>=";
        case InfixExpressionOpKind::AddAssign:
            return "+=";
        case InfixExpressionOpKind::SubAssign:
            return "-=";
        case InfixExpressionOpKind::MulAssign:
            return "*=";
        case InfixExpressionOpKind::DivAssign:
            return "/=";
        case InfixExpressionOpKind::ModAssign:
            return "%=";
        default:
            return "";
    }
}

std::string PostfixExpressionOp::debug() const {
    switch (kind_) {
        case PostfixExpressionOpKind::Inc:
            return "++";
        case PostfixExpressionOpKind::Dec:
            return "--";
        default:
            return "";
    }
}

}  // namespace tinyc
