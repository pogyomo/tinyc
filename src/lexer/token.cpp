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
        case TokenKind::Dot:
            return ".";
        case TokenKind::Arrow:
            return "->";
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

std::string KeywordToken::debug() const {
    switch (kind()) {
        case TokenKind::Auto:
            return "auto";
        case TokenKind::Break:
            return "break";
        case TokenKind::Case:
            return "case";
        case TokenKind::Char:
            return "char";
        case TokenKind::Const:
            return "const";
        case TokenKind::Continue:
            return "continue";
        case TokenKind::Default:
            return "default";
        case TokenKind::Do:
            return "do";
        case TokenKind::Double:
            return "double";
        case TokenKind::Else:
            return "else";
        case TokenKind::Enum:
            return "enum";
        case TokenKind::Extern:
            return "extern";
        case TokenKind::Float:
            return "float";
        case TokenKind::For:
            return "for";
        case TokenKind::Goto:
            return "goto";
        case TokenKind::If:
            return "if";
        case TokenKind::Inline:
            return "inline";
        case TokenKind::Int:
            return "int";
        case TokenKind::Long:
            return "long";
        case TokenKind::Register:
            return "register";
        case TokenKind::Restrict:
            return "restrict";
        case TokenKind::Return:
            return "return";
        case TokenKind::Short:
            return "short";
        case TokenKind::Signed:
            return "signed";
        case TokenKind::Sizeof:
            return "sizeof";
        case TokenKind::Static:
            return "static";
        case TokenKind::Struct:
            return "struct";
        case TokenKind::Switch:
            return "switch";
        case TokenKind::Typedef:
            return "typedef";
        case TokenKind::Union:
            return "union";
        case TokenKind::Unsigned:
            return "unsigned";
        case TokenKind::Void:
            return "void";
        case TokenKind::Volatile:
            return "volatile";
        case TokenKind::While:
            return "while";
        default:
            return "unknown keyword token";
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
