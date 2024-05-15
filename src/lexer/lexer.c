#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "../input/input.h"
#include "../input/stream.h"
#include "../panic.h"
#include "../preprocessor/preprocessor.h"
#include "../report.h"
#include "stream.h"
#include "token.h"

// Returns true if `c` is octal digit.
static bool is_octal(char c) { return '0' <= c && c <= '7'; }

// Returns true if `c` is decimal digit.
static bool is_decimal(char c) { return '0' <= c && c <= '9'; }

// Returns true if `c` is hexadecimal digit.
static bool is_hexadecimal(char c) {
    return '0' <= c && c <= '9' || 'a' <= c && c <= 'f' || 'A' <= c && c <= 'F';
}

// Create a token with neccessary infomations.
static token_t *token_new(size_t lrow, span_t span, token_kind_t kind,
                          string_t *value) {
    token_t *token = malloc(sizeof(token_t));
    if (!token) panic_internal("failed to allocate memory");
    token->lrow = lrow;
    token->span = span;
    token->kind = kind;
    token->value = value;
    return token;
}

// Skipe `//` style of comment.
static bool skip_oneline_comment(istream_t *is) {
    if (istream_eos(is)) return false;
    size_t lrow = is->lrow;
    if (istream_accept(is, "//", NULL)) {
        while (!istream_eos(is) && is->lrow == lrow) istream_advance(is);
        return true;
    } else {
        return false;
    }
}

// Skipe `/*` `*/` style of comment.
static bool skip_multiline_comment(context_t *ctx, istream_t *is,
                                   icache_id_t id) {
    if (istream_eos(is)) return false;

    position_t start = istream_pos(is);
    position_t end = istream_pos(is);
    if (istream_accept(is, "/*", &end)) {
        while (true) {
            if (istream_eos(is)) {
                span_t span = {id, start, end};
                report_info_t info = {string_from("unclosing comment"),
                                      string_new(), span};
                report(ctx, REPORT_LEVEL_ERROR, info);
            } else if (istream_accept(is, "*/", &end)) {
                return true;
            } else {
                istream_advance(is);
            }
        }
    } else {
        return false;
    }
}

// Skip all type of comments from the head of `ts`.
static void skip_comments(context_t *ctx, istream_t *is, icache_id_t id) {
    while (true) {
        bool skipped = false;
        skipped |= skip_oneline_comment(is);
        skipped |= skip_multiline_comment(ctx, is, id);
        if (!skipped) {
            break;
        }
    }
}

// Go to next line or eos if current line is last.
static void goto_nextline(istream_t *is) {
    size_t lrow = is->lrow;
    while (!istream_eos(is) && is->lrow == lrow) istream_advance(is);
}

// Try to read a punctuation from `is`.
// If failed, NULL will be returned.
static token_t *read_punct(istream_t *is, icache_id_t id) {
    static struct {
        char *s;
        token_kind_t kind;
    } pairs[] = {
        {"&&", TK_AND},
        {"&=", TK_ANDASSIGN},
        {"&", TK_AMPERSAND},
        {"||", TK_OR},
        {"|=", TK_ORASSIGN},
        {"|", TK_VERTICAL},
        {"^=", TK_XORASSIGN},
        {"^", TK_HAT},
        {"==", TK_EQ},
        {"=", TK_ASSIGN},
        {"!=", TK_NE},
        {"!", TK_NOT},
        {"<<=", TK_LSHIFTASSIGN},
        {"<<", TK_LSHIFT},
        {"<=", TK_LE},
        {"<", TK_LT},
        {">>=", TK_RSHIFTASSIGN},
        {">>", TK_RSHIFT},
        {">=", TK_GE},
        {">", TK_GT},
        {"++", TK_PLUSPLUS},
        {"+=", TK_ADDASSIGN},
        {"+", TK_PLUS},
        {"->", TK_ARROW},
        {"--", TK_MINUSMINUS},
        {"-=", TK_SUBASSIGN},
        {"-", TK_MINUS},
        {"*=", TK_MULASSIGN},
        {"*", TK_STAR},
        {"/=", TK_DIVASSIGN},
        {"/", TK_SLASH},
        {"%=", TK_MODASSIGN},
        {"%", TK_PERCENT},
        {"(", TK_LPAREN},
        {")", TK_RPAREN},
        {"{", TK_LCURLY},
        {"}", TK_RCURLY},
        {"[", TK_LSQUARE},
        {"]", TK_RSQUARE},
        {";", TK_SEMICOLON},
        {":", TK_COLON},
        {",", TK_COMMA},
        {"~", TK_TILDE},
        {"?", TK_QUESTION},
        {"#", TK_SHARP},
    };
    size_t lrow = is->lrow;
    position_t start = istream_pos(is);
    position_t end = istream_pos(is);

    istream_state_t state = istream_state(is);
    if (istream_accept(is, ".", &end)) {
        if (!istream_eos(is) && is_decimal(istream_char(is))) {
            istream_set_state(is, state);
            return NULL;
        } else {
            span_t span = {id, start, end};
            return token_new(lrow, span, TK_DOT, string_from("."));
        }
    } else {
        for (int i = 0; i < sizeof(pairs) / sizeof(pairs[0]); i++) {
            if (istream_accept(is, pairs[i].s, &end)) {
                span_t span = {id, start, end};
                return token_new(lrow, span, pairs[i].kind,
                                 string_from(pairs[i].s));
            }
        }
        return NULL;
    }
}

// Try to read a identifier or keyword from `is`.
// If failed, NULL will be returned.
static token_t *read_ident_or_kw(istream_t *is, icache_id_t id) {
    static struct {
        char *s;
        token_kind_t kind;
    } pairs[] = {
        {"auto", TK_AUTO},         {"break", TK_BREAK},
        {"case", TK_CASE},         {"char", TK_CHAR},
        {"const", TK_CONST},       {"continue", TK_CONTINUE},
        {"default", TK_DEFAULT},   {"do", TK_DO},
        {"double", TK_DOUBLE},     {"else", TK_ELSE},
        {"enum", TK_ENUM},         {"extern", TK_EXTERN},
        {"float", TK_FLOAT},       {"for", TK_FOR},
        {"goto", TK_GOTO},         {"if", TK_IF},
        {"inline", TK_INLINE},     {"int", TK_INT},
        {"long", TK_LONG},         {"register", TK_REGISTER},
        {"restrict", TK_RESTRICT}, {"return", TK_RETURN},
        {"short", TK_SHORT},       {"signed", TK_SIGNED},
        {"sizeof", TK_SIZEOF},     {"static", TK_STATIC},
        {"struct", TK_STRUCT},     {"switch", TK_SWITCH},
        {"typedef", TK_TYPEDEF},   {"union", TK_UNION},
        {"unsigned", TK_UNSIGNED}, {"void", TK_VOID},
        {"volatile", TK_VOLATILE}, {"while", TK_WHILE},
    };

    if (!isalpha(istream_char(is)) && istream_char(is) != '_') {
        return NULL;
    }

    size_t lrow = is->lrow;
    position_t start = istream_pos(is);
    position_t end = istream_pos(is);
    char c;
    string_t *s = string_new();
    while (!istream_eos(is) && is->lrow == lrow) {
        c = istream_char(is);
        if (isalnum(c) || c == '_') {
            string_push(s, c);
            end = istream_pos(is);
            istream_advance(is);
        } else {
            break;
        }
    }

    span_t span = {id, start, end};
    for (int i = 0; i < sizeof(pairs) / sizeof(pairs[0]); i++) {
        if (strcmp(s->str, pairs[i].s) == 0) {
            return token_new(lrow, span, pairs[i].kind, s);
        }
    }
    return token_new(lrow, span, TK_IDENTIFIER, s);
}

// Read a character used in string or character literal and return true if
// success.
// `end` will hold a position of last character this function consume.
static bool read_character(context_t *ctx, istream_t *is, icache_id_t id,
                           char *ch, position_t *end) {
    char c = istream_char(is);
    *end = istream_pos(is);
    istream_advance(is);

    if (c == '\\') {
        if (istream_eos(is)) {
            span_t span = {id, *end, *end};
            report_info_t info = {string_from("expected character after \\"),
                                  string_new(), span};
            report(ctx, REPORT_LEVEL_ERROR, info);
            return false;
        }
        c = istream_char(is);
        *end = istream_pos(is);
        if (c == 'a') {
            c = 0x07;
        } else if (c == 'b') {
            c = 0x08;
        } else if (c == 'e') {
            c = 0x1B;
        } else if (c == 'f') {
            c = 0x0C;
        } else if (c == 'n') {
            c = 0x0A;
        } else if (c == 'r') {
            c = 0x0D;
        } else if (c == 't') {
            c = 0x09;
        } else if (c == 'v') {
            c = 0x0B;
        } else if (c == '\\') {
            c = 0x5C;
        } else if (c == '\'') {
            c = 0x27;
        } else if (c == '"') {
            c = 0x22;
        } else if (c == '?') {
            c = 0x3F;
        } else if (c == '0') {
            c = 0x00;
        } else {
            span_t span = {id, istream_pos(is), istream_pos(is)};
            report_info_t info = {string_from("unknown escape sequence"),
                                  string_new(), span};
            report(ctx, REPORT_LEVEL_ERROR, info);
            return false;
        }
        istream_advance(is);
    }
    *ch = c;
    return true;
}

// Read string literal from 'is' by assuming first character in `is` is `"`.
// If falied, report error and advance `is` to next line, then return NULL.
static token_t *read_string_literal(context_t *ctx, istream_t *is,
                                    icache_id_t id) {
    size_t lrow = is->lrow;
    position_t start = istream_pos(is);
    position_t end = istream_pos(is);
    istream_advance(is);

    string_t *s = string_new();
    while (true) {
        if (istream_eos(is) || is->lrow != lrow) {
            span_t span = {id, start, end};
            report_info_t info = {string_from("unclosing string literal"),
                                  string_new(), span};
            report(ctx, REPORT_LEVEL_ERROR, info);
            return NULL;
        } else if (istream_char(is) == '"') {
            end = istream_pos(is);
            istream_advance(is);
            break;
        }

        char c;
        if (!read_character(ctx, is, id, &c, &end)) {
            goto_nextline(is);
            return NULL;
        }
        string_push(s, c);
    }

    span_t span = {id, start, end};
    return token_new(lrow, span, TK_STRING, s);
}

// Read character literal from 'is' by assuming first character in `is` is `'`.
// If falied, report error and advance `is` to next line, then return NULL.
static token_t *read_character_literal(context_t *ctx, istream_t *is,
                                       icache_id_t id) {
    size_t lrow = is->lrow;
    position_t start = istream_pos(is);
    position_t end = istream_pos(is);
    istream_advance(is);

    string_t *s = string_new();
    while (true) {
        if (istream_eos(is) || is->lrow != lrow) {
            span_t span = {id, start, end};
            report_info_t info = {string_from("unclosing character literal"),
                                  string_new(), span};
            report(ctx, REPORT_LEVEL_ERROR, info);
            return NULL;
        } else if (istream_char(is) == '\'') {
            end = istream_pos(is);
            istream_advance(is);
            break;
        }

        char c;
        if (!read_character(ctx, is, id, &c, &end)) {
            goto_nextline(is);
            return NULL;
        }
        string_push(s, c);
    }

    span_t span = {id, start, end};
    return token_new(lrow, span, TK_STRING, s);
}

// Read octal digits from current lines and append these to `s`.
static void read_octals(istream_t *is, position_t *end, string_t *s) {
    size_t lrow = is->lrow;
    char c;
    while (!istream_eos(is) && is->lrow == lrow) {
        c = istream_char(is);
        if (is_octal(c)) {
            string_push(s, c);
            istream_advance(is);
        } else {
            break;
        }
    }
}

// Read decimal digits from current lines and append these to `s`.
static void read_decimals(istream_t *is, position_t *end, string_t *s) {
    size_t lrow = is->lrow;
    char c;
    while (!istream_eos(is) && is->lrow == lrow) {
        c = istream_char(is);
        if (is_decimal(c)) {
            string_push(s, c);
            istream_advance(is);
        } else {
            break;
        }
    }
}

// Read hexadecimal digits from current lines and append these to `s`.
static void read_hexadecimals(istream_t *is, position_t *end, string_t *s) {
    size_t lrow = is->lrow;
    char c;
    while (!istream_eos(is) && is->lrow == lrow) {
        c = istream_char(is);
        if (is_hexadecimal(c)) {
            string_push(s, c);
            istream_advance(is);
        } else {
            break;
        }
    }
}

static int_suffix_t read_int_suffix(istream_t *is, position_t *end) {
    if (istream_eos(is)) return INT_SUFFIX_NONE;
    if (istream_accept(is, "u", end) || istream_accept(is, "U", end)) {
        if (istream_accept(is, "ll", end) || istream_accept(is, "LL", end)) {
            return INT_SUFFIX_ULL;
        } else if (istream_accept(is, "l", end) ||
                   istream_accept(is, "L", end)) {
            return INT_SUFFIX_UL;
        } else {
            return INT_SUFFIX_U;
        }
    } else if (istream_accept(is, "ll", end) || istream_accept(is, "LL", end)) {
        if (istream_accept(is, "u", end) || istream_accept(is, "U", end)) {
            return INT_SUFFIX_ULL;
        } else {
            return INT_SUFFIX_LL;
        }
    } else if (istream_accept(is, "l", end) || istream_accept(is, "L", end)) {
        if (istream_accept(is, "u", end) || istream_accept(is, "U", end)) {
            return INT_SUFFIX_UL;
        } else {
            return INT_SUFFIX_L;
        }
    } else {
        return INT_SUFFIX_NONE;
    }
}

static float_suffix_t read_float_suffix(istream_t *is, position_t *end) {
    if (istream_eos(is)) return FLOAT_SUFFIX_NONE;
    if (istream_accept(is, "f", end) || istream_accept(is, "F", end)) {
        return FLOAT_SUFFIX_F;
    } else if (istream_accept(is, "l", end) || istream_accept(is, "L", end)) {
        return FLOAT_SUFFIX_L;
    } else {
        return FLOAT_SUFFIX_NONE;
    }
}

static token_t *read_number_literal(context_t *ctx, istream_t *is,
                                    icache_id_t id) {
    size_t lrow = is->lrow;
    position_t start = istream_pos(is);
    position_t end = istream_pos(is);

    char c;
    string_t *s = string_new();
    int radix;
    if (istream_accept(is, "0x", &end)) {
        string_append(s, "0x");
        read_hexadecimals(is, &end, s);

        if (istream_eos(is) || istream_char(is) != '.') {
            int_suffix_t suffix = read_int_suffix(is, &end);
            span_t span = {id, start, end};
            token_t *token = token_new(lrow, span, TK_INTEGER, s);
            token->int_suffix = suffix;
            return token;
        }
        string_push(s, istream_char(is));
        istream_advance(is);

        read_hexadecimals(is, &end, s);

        if (!istream_eos(is)) {
            c = istream_char(is);
            if (istream_accept(is, "p", &end) ||
                istream_accept(is, "P", &end)) {
                string_push(s, c);
                c = istream_char(is);
                if (istream_accept(is, "+", &end) ||
                    istream_accept(is, "-", &end)) {
                    string_push(s, c);
                }
                read_hexadecimals(is, &end, s);
            }
        }

        float_suffix_t suffix = read_float_suffix(is, &end);
        span_t span = {id, start, end};
        token_t *token = token_new(lrow, span, TK_FLOATING, s);
        token->float_suffix = suffix;
        return token;
    } else if (istream_accept(is, "0", &end)) {
        string_push(s, '0');
        read_octals(is, &end, s);
        int_suffix_t suffix = read_int_suffix(is, &end);
        span_t span = {id, start, end};
        token_t *token = token_new(lrow, span, TK_INTEGER, s);
        token->int_suffix = suffix;
        return token;
    } else if (istream_accept(is, ".", &end)) {
        string_push(s, '.');

        read_decimals(is, &end, s);

        if (!istream_eos(is)) {
            c = istream_char(is);
            if (istream_accept(is, "e", &end) ||
                istream_accept(is, "E", &end)) {
                string_push(s, c);
                c = istream_char(is);
                if (istream_accept(is, "+", &end) ||
                    istream_accept(is, "-", &end)) {
                    string_push(s, c);
                }
                read_decimals(is, &end, s);
            }
        }

        float_suffix_t suffix = read_float_suffix(is, &end);
        span_t span = {id, start, end};
        token_t *token = token_new(lrow, span, TK_FLOATING, s);
        token->float_suffix = suffix;
        return token;
    } else {
        read_decimals(is, &end, s);

        if (istream_eos(is) || istream_char(is) != '.') {
            int_suffix_t suffix = read_int_suffix(is, &end);
            span_t span = {id, start, end};
            token_t *token = token_new(lrow, span, TK_INTEGER, s);
            token->int_suffix = suffix;
            return token;
        }
        string_push(s, istream_char(is));
        istream_advance(is);

        read_decimals(is, &end, s);

        if (!istream_eos(is)) {
            c = istream_char(is);
            if (istream_accept(is, "p", &end) ||
                istream_accept(is, "P", &end)) {
                string_push(s, c);
                c = istream_char(is);
                if (istream_accept(is, "+", &end) ||
                    istream_accept(is, "-", &end)) {
                    string_push(s, c);
                }
                read_decimals(is, &end, s);
            }
        }

        float_suffix_t suffix = read_float_suffix(is, &end);
        span_t span = {id, start, end};
        token_t *token = token_new(lrow, span, TK_FLOATING, s);
        token->float_suffix = suffix;
        return token;
    }
}

tstream_t *lex_file(context_t *ctx, char *path) {
    input_t *input = input_from_file(path);
    icache_id_t id = icache_cache(ctx->icache, input);
    istream_t *is = istream_new(input);

    bool error_happen = false;
    token_t *token = NULL;
    vector_t *tokens = vector_new();
    skip_comments(ctx, is, id);
    while (!istream_eos(is)) {
        if ((token = read_punct(is, id))) {
            vector_push(tokens, token);
            skip_comments(ctx, is, id);
            continue;
        };
        if ((token = read_ident_or_kw(is, id))) {
            vector_push(tokens, token);
            skip_comments(ctx, is, id);
            continue;
        }
        if (istream_char(is) == '"') {
            if ((token = read_string_literal(ctx, is, id))) {
                vector_push(tokens, token);
            }
        } else if (istream_char(is) == '\'') {
            if ((token = read_character_literal(ctx, is, id))) {
                vector_push(tokens, token);
            }
        } else if (isdigit(istream_char(is)) || istream_char(is) == '.') {
            if ((token = read_number_literal(ctx, is, id))) {
                vector_push(tokens, token);
            }
        } else if (isspace(istream_char(is))) {
            int lrow = is->lrow;
            span_t span = {id, istream_pos(is), istream_pos(is)};
            string_t *s = string_new();
            string_push(s, istream_char(is));
            istream_advance(is);
            vector_push(tokens, token_new(lrow, span, TK_SPACE, s));
        } else {
            position_t start = istream_pos(is);
            position_t end = istream_pos(is);
            span_t span = {id, start, end};
            report_info_t info = {string_from("unrecognizable character found"),
                                  string_new(), span};
            report(ctx, REPORT_LEVEL_ERROR, info);
            istream_advance(is);
        }
        skip_comments(ctx, is, id);
    }
    if (error_happen) {
        return NULL;
    } else {
        return preprocess(ctx, tstream_new(tokens));
    }
}
