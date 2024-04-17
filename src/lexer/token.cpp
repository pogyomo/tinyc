#include "token.h"

#include <string>

namespace tinyc {

std::string SymbolToken::debug() const {
    switch (kind()) {
        case TokenKind::Or:
            return "||";
        case TokenKind::And:
            return "&&";
        case TokenKind::Vertical:
            return "|";
        case TokenKind::Hat:
            return "^";
        case TokenKind::Ampersand:
            return "&";
        case TokenKind::EQ:
            return "==";
        case TokenKind::NE:
            return "!=";
        case TokenKind::LT:
            return "<";
        case TokenKind::GT:
            return ">";
        case TokenKind::LE:
            return "<=";
        case TokenKind::GE:
            return ">=";
        case TokenKind::LShift:
            return "<<";
        case TokenKind::RShift:
            return ">>";
        case TokenKind::Plus:
            return "+";
        case TokenKind::PlusPlus:
            return "++";
        case TokenKind::Minus:
            return "-";
        case TokenKind::MinusMinus:
            return "--";
        case TokenKind::Star:
            return "*";
        case TokenKind::Slash:
            return "/";
        case TokenKind::Percent:
            return "%";
        case TokenKind::LParen:
            return "(";
        case TokenKind::RParen:
            return ")";
        case TokenKind::LCurly:
            return "{";
        case TokenKind::RCurly:
            return "}";
        case TokenKind::LSquare:
            return "[";
        case TokenKind::RSquare:
            return "]";
        case TokenKind::Semicolon:
            return ";";
        case TokenKind::Colon:
            return ":";
        case TokenKind::Comma:
            return ",";
        case TokenKind::Tilde:
            return "~";
        case TokenKind::Not:
            return "!";
        case TokenKind::Assign:
            return "=";
        case TokenKind::OrAssign:
            return "|=";
        case TokenKind::XorAssign:
            return "^=";
        case TokenKind::AndAssign:
            return "&=";
        case TokenKind::LShiftAssign:
            return "<<=";
        case TokenKind::RShiftAssign:
            return ">>=";
        case TokenKind::AddAssign:
            return "+=";
        case TokenKind::SubAssign:
            return "-=";
        case TokenKind::MulAssign:
            return "*=";
        case TokenKind::DivAssign:
            return "/=";
        case TokenKind::ModAssign:
            return "%=";
        default:
            return "unknown symbol token";
    }
}

template <>
std::string ValueToken<long long>::debug() const {
    switch (kind()) {
        case TokenKind::Integer:
            return std::to_string(value_);
        default:
            return "unknown value token";
    }
}

template <>
std::string ValueToken<std::string>::debug() const {
    switch (kind()) {
        case TokenKind::Identifier:
            return value_;
        default:
            return "unknown value token";
    }
}

}  // namespace tinyc
