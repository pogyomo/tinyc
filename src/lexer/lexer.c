#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "../preprocessor/preprocessor.h"
#include "../report.h"
#include "../stream.h"
#include "token.h"

// Returns true if `c` is octal digit.
static bool is_octal(char c) { return '0' <= c && c <= '7'; }

// Returns true if `c` is decimal digit.
static bool is_decimal(char c) { return '0' <= c && c <= '9'; }

// Returns true if `c` is hexadecimal digit.
static bool is_hexadecimal(char c) {
    return '0' <= c && c <= '9' || 'a' <= c && c <= 'f' || 'A' <= c && c <= 'F';
}

// Convert non-printable characters to printable string and store it to `buf`.
static void to_printable(char c, char *buf) {
    if (c == 0x07) {
        strcpy(buf, "\\a");
    } else if (c == 0x08) {
        strcpy(buf, "\\b");
    } else if (c == 0x1B) {
        strcpy(buf, "\\e");
    } else if (c == 0x0C) {
        strcpy(buf, "\\f");
    } else if (c == 0x0A) {
        strcpy(buf, "\\n");
    } else if (c == 0x0D) {
        strcpy(buf, "\\r");
    } else if (c == 0x09) {
        strcpy(buf, "\\t");
    } else if (c == 0x0B) {
        strcpy(buf, "\\v");
    } else if (c == 0x5C) {
        strcpy(buf, "\\\\");
    } else if (c == 0x27) {
        strcpy(buf, "\\'");
    } else if (c == 0x22) {
        strcpy(buf, "\\\"");
    } else if (c == 0x3F) {
        strcpy(buf, "\\?");
    } else if (c == 0x00) {
        strcpy(buf, "\\0");
    } else {
        buf[0] = c;
        buf[1] = '\0';
    }
}

// Create a token with neccessary infomations.
static void token_init(token_t *token, token_kind_t kind, string_t repr,
                       size_t lrow, span_t span) {
    token->kind = kind;
    token->repr = repr;
    token->lrow = lrow;
    token->span = span;

    char buf[3];
    if (kind == TK_STRING) {
        string_init(&token->printable);
        string_push(&token->printable, '"');
        for (int i = 1; i < repr.len - 1; i++) {
            to_printable(string_at(&repr, i), buf);
            string_append(&token->printable, buf);
        }
        string_push(&token->printable, '"');
    } else if (kind == TK_CHARACTER) {
        char *s;
        string_init(&token->printable);
        string_push(&token->printable, '\'');
        for (int i = 1; i < repr.len - 1; i++) {
            to_printable(string_at(&repr, i), buf);
            string_append(&token->printable, buf);
        }
        string_push(&token->printable, '\'');
    } else {
        token->printable = repr;
    }
}

// Skipe `//` style of comment.
static bool skip_oneline_comment(istream_t *is) {
    if (istream_eos(is)) return false;
    size_t lrow = is->lrow;
    if (istream_accept(is, "//", NULL, NULL)) {
        while (!istream_eos(is) && is->lrow == lrow) istream_advance(is);
        return true;
    } else {
        return false;
    }
}

// Skipe `/*` `*/` style of comment.
static bool skip_multiline_comment(context_t *ctx, istream_t *is,
                                   cache_id_t id) {
    if (istream_eos(is)) return false;

    position_t start = istream_pos(is);
    position_t end = istream_pos(is);
    if (istream_accept(is, "/*", &end, NULL)) {
        while (true) {
            if (istream_eos(is)) {
                span_t span = {id, start, end};
                string_t what, info;
                string_from(&what, "unclosing comment");
                string_init(&info);
                report(ctx, REPORT_LEVEL_ERROR,
                       (report_info_t){what, info, span});
            } else if (istream_accept(is, "*/", &end, NULL)) {
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
static void skip_comments(context_t *ctx, istream_t *is, cache_id_t id) {
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

// Try to read a punctuation from `is` and initialize `token` with it.
// If failed, return false.
static bool read_punct(istream_t *is, cache_id_t id, token_t *token) {
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
    if (istream_accept(is, ".", &end, NULL)) {
        if (!istream_eos(is) && is_decimal(istream_char(is))) {
            istream_set_state(is, state);
            return NULL;
        } else {
            span_t span = {id, start, end};
            string_t repr;
            string_from(&repr, ".");
            token_init(token, TK_DOT, repr, lrow, span);
            return true;
        }
    } else {
        for (int i = 0; i < sizeof(pairs) / sizeof(pairs[0]); i++) {
            if (istream_accept(is, pairs[i].s, &end, NULL)) {
                span_t span = {id, start, end};
                string_t repr;
                string_from(&repr, pairs[i].s);
                token_init(token, pairs[i].kind, repr, lrow, span);
                return true;
            }
        }
        return false;
    }
}

// Try to read a identifier or keyword from `is` and initialize `toeken` with
// it.
// If failed, return false.
static bool read_ident_or_kw(istream_t *is, cache_id_t id, token_t *token) {
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
    string_t repr;
    string_init(&repr);
    while (!istream_eos(is) && is->lrow == lrow) {
        c = istream_char(is);
        if (isalnum(c) || c == '_') {
            string_push(&repr, c);
            end = istream_pos(is);
            istream_advance(is);
        } else {
            break;
        }
    }

    span_t span = {id, start, end};
    for (int i = 0; i < sizeof(pairs) / sizeof(pairs[0]); i++) {
        if (strcmp(repr.str, pairs[i].s) == 0) {
            token_init(token, pairs[i].kind, repr, lrow, span);
            return true;
        }
    }
    token_init(token, TK_IDENTIFIER, repr, lrow, span);
    return true;
}

// Read a character used in string or character literal and store it to `ch`.
// `end` will hold a position of last character this function consume.
// Returns true if success.
static bool read_character(context_t *ctx, istream_t *is, cache_id_t id,
                           char *ch, position_t *end) {
    char c = istream_char(is);
    *end = istream_pos(is);
    istream_advance(is);

    if (c == '\\') {
        if (istream_eos(is)) {
            span_t span = {id, *end, *end};
            string_t what, info;
            string_from(&what, "expected character after \\");
            string_init(&info);
            report(ctx, REPORT_LEVEL_ERROR, (report_info_t){what, info, span});
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
            string_t what, info;
            string_from(&what, "unknown escape sequence");
            string_init(&info);
            report(ctx, REPORT_LEVEL_ERROR, (report_info_t){what, info, span});
            return false;
        }
        istream_advance(is);
    }
    *ch = c;
    return true;
}

// Read string literal from 'is' by assuming first character in `is` is `"` and
// initialize `token` with it.
// If falied, report error and advance `is` to next line, then return false.
static bool read_string_literal(context_t *ctx, istream_t *is, cache_id_t id,
                                token_t *token) {
    size_t lrow = is->lrow;
    position_t start = istream_pos(is);
    position_t end = istream_pos(is);
    istream_advance(is);

    string_t repr;
    string_from(&repr, "\"");
    while (true) {
        if (istream_eos(is) || is->lrow != lrow) {
            span_t span = {id, start, end};
            string_t what, info;
            string_from(&what, "unclosing string literal");
            string_init(&info);
            report(ctx, REPORT_LEVEL_ERROR, (report_info_t){what, info, span});
            return false;
        } else if (istream_char(is) == '"') {
            string_push(&repr, '"');
            end = istream_pos(is);
            istream_advance(is);
            break;
        }

        char c;
        if (!read_character(ctx, is, id, &c, &end)) {
            goto_nextline(is);
            return false;
        }
        string_push(&repr, c);
    }

    span_t span = {id, start, end};
    token_init(token, TK_STRING, repr, lrow, span);
    return true;
}

// Read character literal from 'is' by assuming first character in `is` is `'`
// and initialize `token` with it.
// If falied, report error and advance `is` to next line, then return false.
static bool read_character_literal(context_t *ctx, istream_t *is, cache_id_t id,
                                   token_t *token) {
    size_t lrow = is->lrow;
    position_t start = istream_pos(is);
    position_t end = istream_pos(is);
    istream_advance(is);

    string_t repr;
    string_from(&repr, "'");
    while (true) {
        if (istream_eos(is) || is->lrow != lrow) {
            span_t span = {id, start, end};
            string_t what, info;
            string_from(&what, "unclosing character literal");
            string_init(&info);
            report(ctx, REPORT_LEVEL_ERROR, (report_info_t){what, info, span});
            return false;
        } else if (istream_char(is) == '\'') {
            string_push(&repr, '\'');
            end = istream_pos(is);
            istream_advance(is);
            break;
        }

        char c;
        if (!read_character(ctx, is, id, &c, &end)) {
            goto_nextline(is);
            return false;
        }
        string_push(&repr, c);
    }

    span_t span = {id, start, end};
    token_init(token, TK_CHARACTER, repr, lrow, span);
    return true;
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

static int_suffix_t read_int_suffix(istream_t *is, position_t *end,
                                    string_t *s) {
    if (istream_eos(is)) return INT_SUFFIX_NONE;
    if (istream_accept(is, "u", end, s) || istream_accept(is, "U", end, s)) {
        if (istream_accept(is, "ll", end, s) ||
            istream_accept(is, "LL", end, s)) {
            return INT_SUFFIX_ULL;
        } else if (istream_accept(is, "l", end, s) ||
                   istream_accept(is, "L", end, s)) {
            return INT_SUFFIX_UL;
        } else {
            return INT_SUFFIX_U;
        }
    } else if (istream_accept(is, "ll", end, s) ||
               istream_accept(is, "LL", end, s)) {
        if (istream_accept(is, "u", end, s) ||
            istream_accept(is, "U", end, s)) {
            return INT_SUFFIX_ULL;
        } else {
            return INT_SUFFIX_LL;
        }
    } else if (istream_accept(is, "l", end, s) ||
               istream_accept(is, "L", end, s)) {
        if (istream_accept(is, "u", end, s) ||
            istream_accept(is, "U", end, s)) {
            return INT_SUFFIX_UL;
        } else {
            return INT_SUFFIX_L;
        }
    } else {
        return INT_SUFFIX_NONE;
    }
}

static float_suffix_t read_float_suffix(istream_t *is, position_t *end,
                                        string_t *s) {
    if (istream_eos(is)) return FLOAT_SUFFIX_NONE;
    if (istream_accept(is, "f", end, s) || istream_accept(is, "F", end, s)) {
        return FLOAT_SUFFIX_F;
    } else if (istream_accept(is, "l", end, s) ||
               istream_accept(is, "L", end, s)) {
        return FLOAT_SUFFIX_L;
    } else {
        return FLOAT_SUFFIX_NONE;
    }
}

// Read integer or floating literal from `is` and initialize `token` with it.
// If failed, return false.
static bool read_number_literal(context_t *ctx, istream_t *is, cache_id_t id,
                                token_t *token) {
    size_t lrow = is->lrow;
    position_t start = istream_pos(is);
    position_t end = istream_pos(is);

    char c;
    string_t repr;
    string_init(&repr);
    int radix;
    if (istream_accept(is, "0x", &end, &repr) ||
        istream_accept(is, "0X", &end, &repr)) {
        read_hexadecimals(is, &end, &repr);

        if (istream_eos(is) || istream_char(is) != '.') {
            if (repr.len == 2 &&
                (strcmp(repr.str, "0x") == 0 || strcmp(repr.str, "0X") == 0)) {
                span_t span = {id, start, end};
                string_t what, info;
                string_from(&what, "unclosing character literal");
                string_init(&info);
                report(ctx, REPORT_LEVEL_ERROR,
                       (report_info_t){what, info, span});
                return false;
            }
            int_suffix_t suffix = read_int_suffix(is, &end, &repr);
            span_t span = {id, start, end};
            token_init(token, TK_INTEGER, repr, lrow, span);
            token->int_.suffix = suffix;
            return true;
        }
        string_push(&repr, istream_char(is));
        istream_advance(is);

        read_hexadecimals(is, &end, &repr);

        if (!istream_eos(is)) {
            c = istream_char(is);
            if (istream_accept(is, "p", &end, NULL) ||
                istream_accept(is, "P", &end, NULL)) {
                string_push(&repr, c);
                c = istream_char(is);
                if (istream_accept(is, "+", &end, NULL) ||
                    istream_accept(is, "-", &end, NULL)) {
                    string_push(&repr, c);
                }
                read_hexadecimals(is, &end, &repr);
            }
        }

        float_suffix_t suffix = read_float_suffix(is, &end, &repr);
        span_t span = {id, start, end};
        token_init(token, TK_FLOATING, repr, lrow, span);
        token->float_.suffix = suffix;
        return true;
    } else if (istream_accept(is, "0", &end, &repr)) {
        read_octals(is, &end, &repr);
        int_suffix_t suffix = read_int_suffix(is, &end, &repr);
        span_t span = {id, start, end};
        token_init(token, TK_INTEGER, repr, lrow, span);
        token->int_.suffix = suffix;
        return true;
    } else if (istream_accept(is, ".", &end, &repr)) {
        read_decimals(is, &end, &repr);

        if (!istream_eos(is)) {
            c = istream_char(is);
            if (istream_accept(is, "e", &end, NULL) ||
                istream_accept(is, "E", &end, NULL)) {
                string_push(&repr, c);
                c = istream_char(is);
                if (istream_accept(is, "+", &end, NULL) ||
                    istream_accept(is, "-", &end, NULL)) {
                    string_push(&repr, c);
                }
                read_decimals(is, &end, &repr);
            }
        }

        float_suffix_t suffix = read_float_suffix(is, &end, &repr);
        span_t span = {id, start, end};
        token_init(token, TK_FLOATING, repr, lrow, span);
        token->float_.suffix = suffix;
        return true;
    } else {
        read_decimals(is, &end, &repr);

        if (istream_eos(is) || istream_char(is) != '.') {
            int_suffix_t suffix = read_int_suffix(is, &end, &repr);
            span_t span = {id, start, end};
            token_init(token, TK_INTEGER, repr, lrow, span);
            token->int_.suffix = suffix;
            return true;
        }
        string_push(&repr, istream_char(is));
        istream_advance(is);

        read_decimals(is, &end, &repr);

        if (!istream_eos(is)) {
            c = istream_char(is);
            if (istream_accept(is, "p", &end, NULL) ||
                istream_accept(is, "P", &end, NULL)) {
                string_push(&repr, c);
                c = istream_char(is);
                if (istream_accept(is, "+", &end, NULL) ||
                    istream_accept(is, "-", &end, NULL)) {
                    string_push(&repr, c);
                }
                read_decimals(is, &end, &repr);
            }
        }

        float_suffix_t suffix = read_float_suffix(is, &end, &repr);
        span_t span = {id, start, end};
        token_init(token, TK_FLOATING, repr, lrow, span);
        token->float_.suffix = suffix;
        return true;
    }
}

bool lex_file(context_t *ctx, char *path, vector_t *tokens) {
    cache_id_t id = cache_file(&ctx->cache, path);
    input_t *input = cache_fetch(&ctx->cache, id);
    vector_init(tokens, sizeof(token_t));

    bool success = true;
    istream_t is;
    istream_init(&is, input);

    skip_comments(ctx, &is, id);
    token_t token;
    while (!istream_eos(&is)) {
        if (read_punct(&is, id, &token)) {
            vector_push(tokens, &token);
            skip_comments(ctx, &is, id);
            continue;
        };
        if (read_ident_or_kw(&is, id, &token)) {
            vector_push(tokens, &token);
            skip_comments(ctx, &is, id);
            continue;
        }
        if (istream_char(&is) == '"') {
            if (read_string_literal(ctx, &is, id, &token)) {
                vector_push(tokens, &token);
            }
        } else if (istream_char(&is) == '\'') {
            if (read_character_literal(ctx, &is, id, &token)) {
                vector_push(tokens, &token);
            }
        } else if (isdigit(istream_char(&is)) || istream_char(&is) == '.') {
            if (read_number_literal(ctx, &is, id, &token)) {
                vector_push(tokens, &token);
            }
        } else if (isspace(istream_char(&is))) {
            int lrow = is.lrow;
            span_t span = {id, istream_pos(&is), istream_pos(&is)};
            string_t repr;
            string_init(&repr);
            string_push(&repr, istream_char(&is));
            istream_advance(&is);
            token_init(&token, TK_SPACE, repr, lrow, span);
            vector_push(tokens, &token);
        } else {
            int lrow = is.lrow;
            span_t span = {id, istream_pos(&is), istream_pos(&is)};
            string_t repr;
            string_init(&repr);
            string_push(&repr, istream_char(&is));
            istream_advance(&is);
            token_init(&token, TK_UNKNOWN, repr, lrow, span);
            vector_push(tokens, &token);
        }
        skip_comments(ctx, &is, id);
    }
    if (success) {
        vector_t output;
        success = preprocess(ctx, tokens, &output);
        *tokens = output;
        return success;
    } else {
        return false;
    }
}
