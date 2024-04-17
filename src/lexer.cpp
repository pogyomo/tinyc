#include "lexer.h"

#include <cctype>
#include <memory>
#include <sstream>
#include <string>

#include "input/input.h"
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
std::shared_ptr<Token> token(Input& input) {
    auto start = input.pos();
    if (input.ch() == '&') {
        input.advance();
        if (!input.eoi() && input.ch() == '&') {
            input.advance();
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::And,
                                                 Span(start, end));
        } else if (!input.eoi() && input.ch() == '=') {
            input.advance();
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::AndAssign,
                                                 Span(start, end));
        } else {
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::Ampersand,
                                                 Span(start, end));
        }
    } else if (input.ch() == '|') {
        input.advance();
        if (!input.eoi() && input.ch() == '|') {
            input.advance();
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::Or,
                                                 Span(start, end));
        } else if (!input.eoi() && input.ch() == '=') {
            input.advance();
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::OrAssign,
                                                 Span(start, end));
        } else {
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::Vertical,
                                                 Span(start, end));
        }
    } else if (input.ch() == '^') {
        if (!input.eoi() && input.ch() == '=') {
            input.advance();
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::XorAssign,
                                                 Span(start, end));
        } else {
            input.advance();
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::Hat,
                                                 Span(start, end));
        }
    } else if (input.ch() == '=') {
        input.advance();
        if (!input.eoi() && input.ch() == '=') {
            input.advance();
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::EQ,
                                                 Span(start, end));
        } else {
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::Assign,
                                                 Span(start, end));
        }
    } else if (input.ch() == '!') {
        input.advance();
        if (!input.eoi() && input.ch() == '=') {
            input.advance();
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::EQ,
                                                 Span(start, end));
        } else {
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::Not,
                                                 Span(start, end));
        }
    } else if (input.ch() == '<') {
        input.advance();
        if (!input.eoi() && input.ch() == '=') {
            input.advance();
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::LE,
                                                 Span(start, end));
        } else if (!input.eoi() && input.ch() == '<') {
            input.advance();
            if (!input.eoi() && input.ch() == '=') {
                input.advance();
                auto end = input.pos();
                return std::make_shared<SymbolToken>(TokenKind::LShiftAssign,
                                                     Span(start, end));
            } else {
                auto end = input.pos();
                return std::make_shared<SymbolToken>(TokenKind::LShift,
                                                     Span(start, end));
            }
        } else {
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::LT,
                                                 Span(start, end));
        }
    } else if (input.ch() == '>') {
        input.advance();
        if (!input.eoi() && input.ch() == '=') {
            input.advance();
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::GE,
                                                 Span(start, end));
        } else if (!input.eoi() && input.ch() == '>') {
            input.advance();
            if (!input.eoi() && input.ch() == '=') {
                input.advance();
                auto end = input.pos();
                return std::make_shared<SymbolToken>(TokenKind::RShiftAssign,
                                                     Span(start, end));
            } else {
                auto end = input.pos();
                return std::make_shared<SymbolToken>(TokenKind::RShift,
                                                     Span(start, end));
            }
        } else {
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::GT,
                                                 Span(start, end));
        }
    } else if (input.ch() == '+') {
        if (!input.eoi() && input.ch() == '=') {
            input.advance();
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::AddAssign,
                                                 Span(start, end));
        } else if (!input.eoi() && input.ch() == '+') {
            input.advance();
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::PlusPlus,
                                                 Span(start, end));
        } else {
            input.advance();
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::Plus,
                                                 Span(start, end));
        }
    } else if (input.ch() == '-') {
        if (!input.eoi() && input.ch() == '=') {
            input.advance();
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::SubAssign,
                                                 Span(start, end));
        } else if (!input.eoi() && input.ch() == '-') {
            input.advance();
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::MinusMinus,
                                                 Span(start, end));
        } else {
            input.advance();
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::Minus,
                                                 Span(start, end));
        }
    } else if (input.ch() == '*') {
        if (!input.eoi() && input.ch() == '=') {
            input.advance();
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::MulAssign,
                                                 Span(start, end));
        } else {
            input.advance();
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::Star,
                                                 Span(start, end));
        }
    } else if (input.ch() == '/') {
        if (!input.eoi() && input.ch() == '=') {
            input.advance();
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::DivAssign,
                                                 Span(start, end));
        } else {
            input.advance();
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::Slash,
                                                 Span(start, end));
        }
    } else if (input.ch() == '%') {
        if (!input.eoi() && input.ch() == '=') {
            input.advance();
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::ModAssign,
                                                 Span(start, end));
        } else {
            input.advance();
            auto end = input.pos();
            return std::make_shared<SymbolToken>(TokenKind::Percent,
                                                 Span(start, end));
        }
    } else if (input.ch() == '(') {
        input.advance();
        auto end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::LParen,
                                             Span(start, end));
    } else if (input.ch() == ')') {
        input.advance();
        auto end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::RParen,
                                             Span(start, end));
    } else if (input.ch() == ';') {
        input.advance();
        auto end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Semicolon,
                                             Span(start, end));
    } else if (input.ch() == ':') {
        input.advance();
        auto end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Colon,
                                             Span(start, end));
    } else if (input.ch() == ',') {
        input.advance();
        auto end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Comma,
                                             Span(start, end));
    } else if (input.ch() == ',') {
        input.advance();
        auto end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Tilde,
                                             Span(start, end));
    } else if (input.ch() == '!') {
        input.advance();
        auto end = input.pos();
        return std::make_shared<SymbolToken>(TokenKind::Not, Span(start, end));
    } else if (std::isdigit(input.ch())) {
        std::string buf;
        while (!input.eoi()) {
            if (std::isdigit(input.ch())) {
                buf.push_back(input.ch());
                input.advance();
            } else {
                break;
            }
        }
        auto end = input.pos();
        auto value = std::stoll(buf);
        return std::make_shared<ValueToken<long long>>(TokenKind::Integer,
                                                       value, Span(start, end));
    } else if (std::isalpha(input.ch())) {
        std::string buf;
        while (!input.eoi()) {
            if (std::isalnum(input.ch()) || input.ch() == '_') {
                buf.push_back(input.ch());
                input.advance();
            } else {
                break;
            }
        }
        auto end = input.pos();
        return std::make_shared<ValueToken<std::string>>(TokenKind::Identifier,
                                                         buf, Span(start, end));
    } else {
        std::stringstream ss;
        ss << "unexpected character '" << input.ch() << "' found";
        input.advance();
        auto end = input.pos();
        throw LexError(ss.str(), Span(start, end));
    }
}

TokenStream lex(Input& input, std::vector<LexError>& es) {
    std::vector<std::shared_ptr<Token>> ts;
    skip_whitespaces(input);
    while (!input.eoi()) {
        try {
            ts.push_back(token(input));
        } catch (LexError e) {
            es.push_back(e);
        }
        skip_whitespaces(input);
    }
    return TokenStream(ts);
}

}  // namespace tinyc
