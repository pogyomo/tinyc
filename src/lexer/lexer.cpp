#include "lexer.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "../input/stream.h"
#include "../preprocessor/preprocessor.h"
#include "../report/report.h"
#include "../span.h"
#include "error.h"
#include "stream.h"
#include "token.h"

namespace tinyc {

// Advance `is` until it reach to non-whitespace character, or advance to end
// if `is` has only whitespaces.
// Returns true if it consume whitespaces.
bool skip_whitespaces(InputStream& is) {
    bool skipped = false;
    while (!is.eos()) {
        if (std::isspace(is.ch())) {
            skipped = true;
            is.advance();
        } else {
            break;
        }
    }
    return skipped;
}

// Advance `is` to next line or eos if `is` start with `//`.
// Returns true if it consume comment.
bool skip_oneline_comment(InputStream& is) {
    int row = is.pos().row();
    if (is.accept("//")) {
        while (!is.eos() && is.pos().row() == row) {
            is.advance();
        }
        return true;
    } else {
        return false;
    }
}

// Advance `is` to consume the characters between `/*` and `*/`.
// Returns true if it consume comment.
bool skip_multiline_comment(InputStream& is) {
    if (is.accept("/*")) {
        while (!is.accept("*/")) {
            is.advance();
        }
        return true;
    } else {
        return false;
    }
}

// Skip comments and whitespaces.
void skips(InputStream& is) {
    while (true) {
        if (skip_whitespaces(is)) {
            continue;
        }
        if (skip_oneline_comment(is)) {
            continue;
        }
        if (skip_multiline_comment(is)) {
            continue;
        }
        break;
    }
}

// Extract a toke from non-empty `is`.
// If unknown character found, throw `LexError` and skip the character.
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
    } else if (is.accept('#', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Sharp, span);
    } else if (is.accept('.', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Dot, span);
    } else if (is.accept('"', span)) {
        int row = is.pos().row();

        std::string s;
        Position end = is.pos();
        while (true) {
            if (is.eos() || is.pos().row() != row) {
                throw LexError("unclosing string literal",
                               Span(is.input().id(), span.start(), end));
            }
            end = is.pos();
            if (is.ch() == '"') {
                is.advance();
                break;
            } else {
                s.push_back(is.ch());
                is.advance();
            }
        }
        return std::make_shared<ValueToken<std::string>>(
            TokenKind::String, s, Span(is.input().id(), span.start(), end));
    } else if (is.accept("0x", span)) {
        int row = span.start().row();

        std::string buf;
        Position end = is.pos();
        while (!is.eos() && is.pos().row() == row) {
            if ('0' <= is.ch() && is.ch() <= '9' ||
                'a' <= is.ch() && is.ch() <= 'f' ||
                'A' <= is.ch() && is.ch() <= 'F') {
                end = is.pos();
                buf.push_back(is.ch());
                is.advance();
            } else {
                break;
            }
        }

        long long value = std::stoll(buf, nullptr, 16);
        return std::make_shared<ValueToken<long long>>(
            TokenKind::Integer, value,
            Span(is.input().id(), span.start(), end));
    } else if (is.accept('0', span)) {
        int row = span.start().row();

        std::string buf;
        Position end = is.pos();
        while (!is.eos() && is.pos().row() == row) {
            if ('0' <= is.ch() && is.ch() <= '7') {
                end = is.pos();
                buf.push_back(is.ch());
                is.advance();
            } else {
                break;
            }
        }

        if (buf.empty()) {
            return std::make_shared<ValueToken<long long>>(TokenKind::Integer,
                                                           0, span);
        } else {
            long long value = std::stoll(buf, nullptr, 8);
            return std::make_shared<ValueToken<long long>>(
                TokenKind::Integer, value,
                Span(is.input().id(), span.start(), end));
        }
    } else if (std::isdigit(is.ch())) {
        int row = is.pos().row();

        std::string buf;
        Position start = is.pos();
        Position end = is.pos();
        while (!is.eos() && is.pos().row() == row) {
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
        int row = is.pos().row();

        std::string buf;
        Position start = is.pos();
        Position end = is.pos();
        while (!is.eos() && is.pos().row() == row) {
            if (std::isalnum(is.ch()) || is.ch() == '_') {
                end = is.pos();
                buf.push_back(is.ch());
                is.advance();
            } else {
                break;
            }
        }

        Span span(is.input().id(), start, end);
        if (buf == "auto") {
            return std::make_shared<KeywordToken>(TokenKind::Auto, span);
        } else if (buf == "break") {
            return std::make_shared<KeywordToken>(TokenKind::Break, span);
        } else if (buf == "case") {
            return std::make_shared<KeywordToken>(TokenKind::Case, span);
        } else if (buf == "char") {
            return std::make_shared<KeywordToken>(TokenKind::Char, span);
        } else if (buf == "const") {
            return std::make_shared<KeywordToken>(TokenKind::Const, span);
        } else if (buf == "continue") {
            return std::make_shared<KeywordToken>(TokenKind::Continue, span);
        } else if (buf == "default") {
            return std::make_shared<KeywordToken>(TokenKind::Default, span);
        } else if (buf == "do") {
            return std::make_shared<KeywordToken>(TokenKind::Do, span);
        } else if (buf == "double") {
            return std::make_shared<KeywordToken>(TokenKind::Double, span);
        } else if (buf == "else") {
            return std::make_shared<KeywordToken>(TokenKind::Else, span);
        } else if (buf == "enum") {
            return std::make_shared<KeywordToken>(TokenKind::Enum, span);
        } else if (buf == "extern") {
            return std::make_shared<KeywordToken>(TokenKind::Extern, span);
        } else if (buf == "float") {
            return std::make_shared<KeywordToken>(TokenKind::Float, span);
        } else if (buf == "for") {
            return std::make_shared<KeywordToken>(TokenKind::For, span);
        } else if (buf == "goto") {
            return std::make_shared<KeywordToken>(TokenKind::Goto, span);
        } else if (buf == "if") {
            return std::make_shared<KeywordToken>(TokenKind::If, span);
        } else if (buf == "inline") {
            return std::make_shared<KeywordToken>(TokenKind::Inline, span);
        } else if (buf == "int") {
            return std::make_shared<KeywordToken>(TokenKind::Int, span);
        } else if (buf == "long") {
            return std::make_shared<KeywordToken>(TokenKind::Long, span);
        } else if (buf == "register") {
            return std::make_shared<KeywordToken>(TokenKind::Register, span);
        } else if (buf == "restrict") {
            return std::make_shared<KeywordToken>(TokenKind::Restrict, span);
        } else if (buf == "return") {
            return std::make_shared<KeywordToken>(TokenKind::Return, span);
        } else if (buf == "short") {
            return std::make_shared<KeywordToken>(TokenKind::Short, span);
        } else if (buf == "signed") {
            return std::make_shared<KeywordToken>(TokenKind::Signed, span);
        } else if (buf == "sizeof") {
            return std::make_shared<KeywordToken>(TokenKind::Sizeof, span);
        } else if (buf == "static") {
            return std::make_shared<KeywordToken>(TokenKind::Static, span);
        } else if (buf == "struct") {
            return std::make_shared<KeywordToken>(TokenKind::Struct, span);
        } else if (buf == "switch") {
            return std::make_shared<KeywordToken>(TokenKind::Switch, span);
        } else if (buf == "typedef") {
            return std::make_shared<KeywordToken>(TokenKind::Typedef, span);
        } else if (buf == "union") {
            return std::make_shared<KeywordToken>(TokenKind::Union, span);
        } else if (buf == "unsigned") {
            return std::make_shared<KeywordToken>(TokenKind::Unsigned, span);
        } else if (buf == "void") {
            return std::make_shared<KeywordToken>(TokenKind::Void, span);
        } else if (buf == "volatile") {
            return std::make_shared<KeywordToken>(TokenKind::Volatile, span);
        } else if (buf == "while") {
            return std::make_shared<KeywordToken>(TokenKind::While, span);
        } else {
            return std::make_shared<ValueToken<std::string>>(
                TokenKind::Identifier, buf, span);
        }
    } else {
        std::stringstream ss;
        ss << "unexpected character '" << is.ch() << "' found";
        is.advance();
        throw LexError(ss.str(), span);
    }
}

std::optional<TokenStream> lex(Context& ctx, std::istream& is,
                               const std::string& name) {
    int id = ctx.input_cache().cache(is, name);
    auto input = ctx.input_cache().fetch(id);
    auto is_ = InputStream(input);

    bool has_error = false;
    std::vector<std::shared_ptr<Token>> ts;
    skips(is_);
    while (!is_.eos()) {
        try {
            ts.push_back(token(is_));
        } catch (LexError e) {
            report(ctx, e);
            has_error = true;
        }
        skips(is_);
    }

    if (has_error) {
        return std::nullopt;
    } else {
        return preprocess(ctx, ts);
    }
}

}  // namespace tinyc
