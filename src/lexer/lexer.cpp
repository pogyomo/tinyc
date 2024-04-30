#include "lexer.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include "../input/stream.h"
#include "../span.h"
#include "error.h"
#include "stream.h"
#include "token.h"

namespace tinyc {

// Advance `is` until it reach to non-whitespace character, or advance to end
// if `is` has only whitespaces.
void skip_whitespaces(InputStream& is) {
    while (!is.eos()) {
        if (std::isspace(is.ch())) {
            is.advance();
        } else {
            break;
        }
    }
}

// Extract a toke from non-empty `is`.
// If unknown character found, throw `LexError`.
std::shared_ptr<Token> token(InputStream& is) {
    Span span = Span(is.input().id(), is.pos(), is.pos());
    if (is.accept("&&", span)) {
        return std::make_shared<SymbolToken>(TokenKind::And, span);
    } else if (is.accept("&=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::AndAssign, span);
    } else if (is.accept('&', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Ampersand, span);
    } else if (is.accept("||", span)) {
        return std::make_shared<SymbolToken>(TokenKind::Or, span);
    } else if (is.accept("|=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::OrAssign, span);
    } else if (is.accept('|', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Vertical, span);
    } else if (is.accept("^=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::XorAssign, span);
    } else if (is.accept('^', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Hat, span);
    } else if (is.accept("==", span)) {
        return std::make_shared<SymbolToken>(TokenKind::EQ, span);
    } else if (is.accept('=', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Assign, span);
    } else if (is.accept("!=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::NE, span);
    } else if (is.accept('!', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Not, span);
    } else if (is.accept("<<=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::LShiftAssign, span);
    } else if (is.accept("<<", span)) {
        return std::make_shared<SymbolToken>(TokenKind::LShift, span);
    } else if (is.accept("<=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::LE, span);
    } else if (is.accept('<', span)) {
        return std::make_shared<SymbolToken>(TokenKind::LT, span);
    } else if (is.accept(">>=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::RShiftAssign, span);
    } else if (is.accept(">>", span)) {
        return std::make_shared<SymbolToken>(TokenKind::RShift, span);
    } else if (is.accept(">=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::GE, span);
    } else if (is.accept('>', span)) {
        return std::make_shared<SymbolToken>(TokenKind::GT, span);
    } else if (is.accept("++", span)) {
        return std::make_shared<SymbolToken>(TokenKind::PlusPlus, span);
    } else if (is.accept("+=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::AddAssign, span);
    } else if (is.accept('+', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Plus, span);
    } else if (is.accept("--", span)) {
        return std::make_shared<SymbolToken>(TokenKind::MinusMinus, span);
    } else if (is.accept("-=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::SubAssign, span);
    } else if (is.accept("->", span)) {
        return std::make_shared<SymbolToken>(TokenKind::Arrow, span);
    } else if (is.accept('-', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Minus, span);
    } else if (is.accept("*=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::MulAssign, span);
    } else if (is.accept('*', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Star, span);
    } else if (is.accept("/=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::DivAssign, span);
    } else if (is.accept('/', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Slash, span);
    } else if (is.accept("%=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::ModAssign, span);
    } else if (is.accept('%', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Percent, span);
    } else if (is.accept('(', span)) {
        return std::make_shared<SymbolToken>(TokenKind::LParen, span);
    } else if (is.accept(')', span)) {
        return std::make_shared<SymbolToken>(TokenKind::RParen, span);
    } else if (is.accept('{', span)) {
        return std::make_shared<SymbolToken>(TokenKind::LCurly, span);
    } else if (is.accept('}', span)) {
        return std::make_shared<SymbolToken>(TokenKind::RCurly, span);
    } else if (is.accept('[', span)) {
        return std::make_shared<SymbolToken>(TokenKind::LSquare, span);
    } else if (is.accept(']', span)) {
        return std::make_shared<SymbolToken>(TokenKind::RSquare, span);
    } else if (is.accept(';', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Semicolon, span);
    } else if (is.accept(':', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Colon, span);
    } else if (is.accept(',', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Comma, span);
    } else if (is.accept('~', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Tilde, span);
    } else if (is.accept('?', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Question, span);
    } else if (is.accept('.', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Dot, span);
    } else if (is.accept("auto", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Auto, span);
    } else if (is.accept("break", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Break, span);
    } else if (is.accept("case", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Case, span);
    } else if (is.accept("char", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Char, span);
    } else if (is.accept("const", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Const, span);
    } else if (is.accept("continue", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Continue, span);
    } else if (is.accept("default", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Default, span);
    } else if (is.accept("do", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Do, span);
    } else if (is.accept("double", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Double, span);
    } else if (is.accept("else", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Else, span);
    } else if (is.accept("enum", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Enum, span);
    } else if (is.accept("extern", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Extern, span);
    } else if (is.accept("float", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Float, span);
    } else if (is.accept("for", span)) {
        return std::make_shared<KeywordToken>(TokenKind::For, span);
    } else if (is.accept("goto", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Goto, span);
    } else if (is.accept("if", span)) {
        return std::make_shared<KeywordToken>(TokenKind::If, span);
    } else if (is.accept("inline", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Inline, span);
    } else if (is.accept("int", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Int, span);
    } else if (is.accept("long", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Long, span);
    } else if (is.accept("register", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Register, span);
    } else if (is.accept("restrict", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Restrict, span);
    } else if (is.accept("return", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Return, span);
    } else if (is.accept("short", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Short, span);
    } else if (is.accept("signed", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Signed, span);
    } else if (is.accept("sizeof", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Sizeof, span);
    } else if (is.accept("static", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Static, span);
    } else if (is.accept("struct", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Struct, span);
    } else if (is.accept("switch", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Switch, span);
    } else if (is.accept("typedef", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Typedef, span);
    } else if (is.accept("union", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Union, span);
    } else if (is.accept("unsigned", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Unsigned, span);
    } else if (is.accept("void", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Void, span);
    } else if (is.accept("volatile", span)) {
        return std::make_shared<KeywordToken>(TokenKind::Volatile, span);
    } else if (is.accept("while", span)) {
        return std::make_shared<KeywordToken>(TokenKind::While, span);
    } else if (std::isdigit(is.ch())) {
        std::string buf;
        Position start = is.pos();
        Position end = is.pos();
        while (!is.eos()) {
            if (std::isdigit(is.ch())) {
                end = is.pos();
                buf.push_back(is.ch());
                is.advance();
            } else {
                break;
            }
        }
        auto value = std::stoll(buf);
        return std::make_shared<ValueToken<long long>>(
            TokenKind::Integer, value, Span(is.input().id(), start, end));
    } else if (std::isalpha(is.ch())) {
        std::string buf;
        Position start = is.pos();
        Position end = is.pos();
        while (!is.eos()) {
            if (std::isalnum(is.ch()) || is.ch() == '_') {
                end = is.pos();
                buf.push_back(is.ch());
                is.advance();
            } else {
                break;
            }
        }
        return std::make_shared<ValueToken<std::string>>(
            TokenKind::Identifier, buf, Span(is.input().id(), start, end));
    } else {
        std::stringstream ss;
        ss << "unexpected character '" << is.ch() << "' found";
        is.advance();
        throw LexError(ss.str(), span);
    }
}

TokenStream lex(Context& ctx, std::istream& is, std::vector<LexError>& es) {
    int id = ctx.input_cache().cache(is);
    auto input = ctx.input_cache().fetch(id);
    auto is_ = InputStream(input);

    std::vector<std::shared_ptr<Token>> ts;
    skip_whitespaces(is_);
    while (!is_.eos()) {
        try {
            ts.push_back(token(is_));
        } catch (LexError e) {
            es.emplace_back(e);
        }
        skip_whitespaces(is_);
    }
    return ts;
}

}  // namespace tinyc
