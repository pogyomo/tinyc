#include "lexer.h"

#include <cctype>
#include <memory>
#include <sstream>
#include <string>

#include "../input/cache.h"
#include "../input/input.h"
#include "token.h"

namespace tinyc {

// Advance `input` until it reach to non-whitespace character, or advance to end
// if `input` has only whitespaces.
void skip_whitespaces(Input& input) {
    while (!input.eoi()) {
        if (std::isspace(input.ch())) {
            input.advance();
        } else {
            break;
        }
    }
}

// Extract a toke from non-empty `input`.
// It's undefined behavior to call this when `input` is empty.
std::shared_ptr<Token> token(Input& input, InputCache::cacheid_t id) {
    auto start = input.pos();
    auto end = input.pos();
    if (input.ch() == '&') {
        input.advance();
        if (!input.eoi() && input.ch() == '&') {
            end = input.pos();
            input.advance();
            return std::make_shared<SymbolToken>(TokenKind::And,
                                                 Span(start, end, id));
        } else if (!input.eoi() && input.ch() == '=') {
            end = input.pos();
            input.advance();
            return std::make_shared<SymbolToken>(TokenKind::AndAssign,
                                                 Span(start, end, id));
        } else {
            return std::make_shared<SymbolToken>(TokenKind::Ampersand,
                                                 Span(start, end, id));
        }
    } else if (input.ch() == '|') {
        input.advance();
        if (!input.eoi() && input.ch() == '|') {
            end = input.pos();
            input.advance();
            return std::make_shared<SymbolToken>(TokenKind::Or,
                                                 Span(start, end, id));
        } else if (!input.eoi() && input.ch() == '=') {
            end = input.pos();
            input.advance();
            return std::make_shared<SymbolToken>(TokenKind::OrAssign,
                                                 Span(start, end, id));
        } else {
            return std::make_shared<SymbolToken>(TokenKind::Vertical,
                                                 Span(start, end, id));
        }
    } else if (input.ch() == '^') {
        input.advance();
        if (!input.eoi() && input.ch() == '=') {
            end = input.pos();
            input.advance();
            return std::make_shared<SymbolToken>(TokenKind::XorAssign,
                                                 Span(start, end, id));
        } else {
            return std::make_shared<SymbolToken>(TokenKind::Hat,
                                                 Span(start, end, id));
        }
    } else if (input.ch() == '=') {
        input.advance();
        if (!input.eoi() && input.ch() == '=') {
            end = input.pos();
            input.advance();
            return std::make_shared<SymbolToken>(TokenKind::EQ,
                                                 Span(start, end, id));
        } else {
            return std::make_shared<SymbolToken>(TokenKind::Assign,
                                                 Span(start, end, id));
        }
    } else if (input.ch() == '!') {
        input.advance();
        if (!input.eoi() && input.ch() == '=') {
            end = input.pos();
            input.advance();
            return std::make_shared<SymbolToken>(TokenKind::EQ,
                                                 Span(start, end, id));
        } else {
            return std::make_shared<SymbolToken>(TokenKind::Not,
                                                 Span(start, end, id));
        }
    } else if (input.ch() == '<') {
        input.advance();
        if (!input.eoi() && input.ch() == '=') {
            end = input.pos();
            input.advance();
            return std::make_shared<SymbolToken>(TokenKind::LE,
                                                 Span(start, end, id));
        } else if (!input.eoi() && input.ch() == '<') {
            end = input.pos();
            input.advance();
            if (!input.eoi() && input.ch() == '=') {
                end = input.pos();
                input.advance();
                return std::make_shared<SymbolToken>(TokenKind::LShiftAssign,
                                                     Span(start, end, id));
            } else {
                return std::make_shared<SymbolToken>(TokenKind::LShift,
                                                     Span(start, end, id));
            }
        } else {
            return std::make_shared<SymbolToken>(TokenKind::LT,
                                                 Span(start, end, id));
        }
    } else if (input.ch() == '>') {
        input.advance();
        if (!input.eoi() && input.ch() == '=') {
            end = input.pos();
            input.advance();
            return std::make_shared<SymbolToken>(TokenKind::GE,
                                                 Span(start, end, id));
        } else if (!input.eoi() && input.ch() == '>') {
            end = input.pos();
            input.advance();
            if (!input.eoi() && input.ch() == '=') {
                end = input.pos();
                input.advance();
                return std::make_shared<SymbolToken>(TokenKind::RShiftAssign,
                                                     Span(start, end, id));
            } else {
                return std::make_shared<SymbolToken>(TokenKind::RShift,
                                                     Span(start, end, id));
            }
        } else {
            return std::make_shared<SymbolToken>(TokenKind::GT,
                                                 Span(start, end, id));
        }
    } else if (input.ch() == '+') {
        input.advance();
        if (!input.eoi() && input.ch() == '=') {
            end = input.pos();
            input.advance();
            return std::make_shared<SymbolToken>(TokenKind::AddAssign,
                                                 Span(start, end, id));
        } else if (!input.eoi() && input.ch() == '+') {
            end = input.pos();
            input.advance();
            return std::make_shared<SymbolToken>(TokenKind::PlusPlus,
                                                 Span(start, end, id));
        } else {
            return std::make_shared<SymbolToken>(TokenKind::Plus,
                                                 Span(start, end, id));
        }
    } else if (input.ch() == '-') {
        input.advance();
        if (!input.eoi() && input.ch() == '=') {
            end = input.pos();
            input.advance();
            return std::make_shared<SymbolToken>(TokenKind::SubAssign,
                                                 Span(start, end, id));
        } else if (!input.eoi() && input.ch() == '-') {
            end = input.pos();
            input.advance();
            return std::make_shared<SymbolToken>(TokenKind::MinusMinus,
                                                 Span(start, end, id));
        } else {
            return std::make_shared<SymbolToken>(TokenKind::Minus,
                                                 Span(start, end, id));
        }
    } else if (input.ch() == '*') {
        input.advance();
        if (!input.eoi() && input.ch() == '=') {
            end = input.pos();
            input.advance();
            return std::make_shared<SymbolToken>(TokenKind::MulAssign,
                                                 Span(start, end, id));
        } else {
            return std::make_shared<SymbolToken>(TokenKind::Star,
                                                 Span(start, end, id));
        }
    } else if (input.ch() == '/') {
        input.advance();
        if (!input.eoi() && input.ch() == '=') {
            end = input.pos();
            input.advance();
            return std::make_shared<SymbolToken>(TokenKind::DivAssign,
                                                 Span(start, end, id));
        } else {
            return std::make_shared<SymbolToken>(TokenKind::Slash,
                                                 Span(start, end, id));
        }
    } else if (input.ch() == '%') {
        input.advance();
        if (!input.eoi() && input.ch() == '=') {
            end = input.pos();
            input.advance();
            return std::make_shared<SymbolToken>(TokenKind::ModAssign,
                                                 Span(start, end, id));
        } else {
            return std::make_shared<SymbolToken>(TokenKind::Percent,
                                                 Span(start, end, id));
        }
    } else if (input.ch() == '(') {
        input.advance();
        return std::make_shared<SymbolToken>(TokenKind::LParen,
                                             Span(start, end, id));
    } else if (input.ch() == ')') {
        input.advance();
        return std::make_shared<SymbolToken>(TokenKind::RParen,
                                             Span(start, end, id));
    } else if (input.ch() == ';') {
        input.advance();
        return std::make_shared<SymbolToken>(TokenKind::Semicolon,
                                             Span(start, end, id));
    } else if (input.ch() == ':') {
        input.advance();
        return std::make_shared<SymbolToken>(TokenKind::Colon,
                                             Span(start, end, id));
    } else if (input.ch() == ',') {
        input.advance();
        return std::make_shared<SymbolToken>(TokenKind::Comma,
                                             Span(start, end, id));
    } else if (input.ch() == ',') {
        input.advance();
        return std::make_shared<SymbolToken>(TokenKind::Tilde,
                                             Span(start, end, id));
    } else if (input.ch() == '!') {
        input.advance();
        return std::make_shared<SymbolToken>(TokenKind::Not,
                                             Span(start, end, id));
    } else if (input.accept("auto")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Auto,
                                             Span(start, end, id));
    } else if (input.accept("break")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Break,
                                             Span(start, end, id));
    } else if (input.accept("case")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Case,
                                             Span(start, end, id));
    } else if (input.accept("char")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Char,
                                             Span(start, end, id));
    } else if (input.accept("const")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Const,
                                             Span(start, end, id));
    } else if (input.accept("continue")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Continue,
                                             Span(start, end, id));
    } else if (input.accept("default")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Default,
                                             Span(start, end, id));
    } else if (input.accept("do")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Do,
                                             Span(start, end, id));
    } else if (input.accept("double")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Double,
                                             Span(start, end, id));
    } else if (input.accept("else")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Else,
                                             Span(start, end, id));
    } else if (input.accept("enum")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Enum,
                                             Span(start, end, id));
    } else if (input.accept("extern")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Extern,
                                             Span(start, end, id));
    } else if (input.accept("float")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Float,
                                             Span(start, end, id));
    } else if (input.accept("for")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::For,
                                             Span(start, end, id));
    } else if (input.accept("goto")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Goto,
                                             Span(start, end, id));
    } else if (input.accept("if")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::If,
                                             Span(start, end, id));
    } else if (input.accept("inline")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Inline,
                                             Span(start, end, id));
    } else if (input.accept("int")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Int,
                                             Span(start, end, id));
    } else if (input.accept("long")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Long,
                                             Span(start, end, id));
    } else if (input.accept("register")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Register,
                                             Span(start, end, id));
    } else if (input.accept("restrict")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Restrict,
                                             Span(start, end, id));
    } else if (input.accept("return")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Return,
                                             Span(start, end, id));
    } else if (input.accept("short")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Short,
                                             Span(start, end, id));
    } else if (input.accept("signed")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Signed,
                                             Span(start, end, id));
    } else if (input.accept("sizeof")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Sizeof,
                                             Span(start, end, id));
    } else if (input.accept("static")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Static,
                                             Span(start, end, id));
    } else if (input.accept("struct")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Struct,
                                             Span(start, end, id));
    } else if (input.accept("switch")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Switch,
                                             Span(start, end, id));
    } else if (input.accept("typedef")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Typedef,
                                             Span(start, end, id));
    } else if (input.accept("union")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Union,
                                             Span(start, end, id));
    } else if (input.accept("unsigned")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Unsigned,
                                             Span(start, end, id));
    } else if (input.accept("void")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Void,
                                             Span(start, end, id));
    } else if (input.accept("volatile")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Volatile,
                                             Span(start, end, id));
    } else if (input.accept("while")) {
        end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::While,
                                             Span(start, end, id));
    } else if (std::isdigit(input.ch())) {
        std::string buf;
        while (!input.eoi()) {
            if (std::isdigit(input.ch())) {
                end = input.pos();
                buf.push_back(input.ch());
                input.advance();
            } else {
                break;
            }
        }
        auto value = std::stoll(buf);
        return std::make_shared<ValueToken<long long>>(
            TokenKind::Integer, value, Span(start, end, id));
    } else if (std::isalpha(input.ch())) {
        std::string buf;
        while (!input.eoi()) {
            if (std::isalnum(input.ch()) || input.ch() == '_') {
                end = input.pos();
                buf.push_back(input.ch());
                input.advance();
            } else {
                break;
            }
        }
        return std::make_shared<ValueToken<std::string>>(
            TokenKind::Identifier, buf, Span(start, end, id));
    } else {
        std::stringstream ss;
        ss << "unexpected character '" << input.ch() << "' found";
        input.advance();
        throw LexError(ss.str(), Span(start, end, id));
    }
}

TokenStream lex(InputCache& cache, InputCache::cacheid_t id,
                std::vector<LexError>& es) {
    auto input = cache.fetch_input(id);
    std::vector<std::shared_ptr<Token>> ts;
    skip_whitespaces(input);
    while (!input.eoi()) {
        try {
            ts.push_back(token(input, id));
        } catch (LexError e) {
            es.push_back(e);
        }
        skip_whitespaces(input);
    }
    return TokenStream(ts);
}

}  // namespace tinyc
