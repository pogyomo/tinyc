#include "lex.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../cpp/cpp.h"
#include "../cpp/token.h"
#include "../panic.h"
#include "../report.h"
#include "token.h"

static struct {
    char *name;
    enum token_keyword_kind kind;
} keywords[] = {
    {"auto", TK_KEYWORD_AUTO},
    {"break", TK_KEYWORD_BREAK},
    {"case", TK_KEYWORD_CASE},
    {"char", TK_KEYWORD_CHAR},
    {"const", TK_KEYWORD_CONST},
    {"continue", TK_KEYWORD_CONTINUE},
    {"default", TK_KEYWORD_DEFAULT},
    {"do", TK_KEYWORD_DO},
    {"double", TK_KEYWORD_DOUBLE},
    {"else", TK_KEYWORD_ELSE},
    {"enum", TK_KEYWORD_ENUM},
    {"extern", TK_KEYWORD_EXTERN},
    {"float", TK_KEYWORD_FLOAT},
    {"for", TK_KEYWORD_FOR},
    {"goto", TK_KEYWORD_GOTO},
    {"if", TK_KEYWORD_IF},
    {"inline", TK_KEYWORD_INLINE},
    {"int", TK_KEYWORD_INT},
    {"long", TK_KEYWORD_LONG},
    {"register", TK_KEYWORD_REGISTER},
    {"restrict", TK_KEYWORD_RESTRICT},
    {"return", TK_KEYWORD_RETURN},
    {"short", TK_KEYWORD_SHORT},
    {"signed", TK_KEYWORD_SIGNED},
    {"sizeof", TK_KEYWORD_SIZEOF},
    {"static", TK_KEYWORD_STATIC},
    {"struct", TK_KEYWORD_STRUCT},
    {"switch", TK_KEYWORD_SWITCH},
    {"typedef", TK_KEYWORD_TYPEDEF},
    {"union", TK_KEYWORD_UNION},
    {"unsigned", TK_KEYWORD_UNSIGNED},
    {"void", TK_KEYWORD_VOID},
    {"volatile", TK_KEYWORD_VOLATILE},
    {"while", TK_KEYWORD_WHILE},
    {"_Bool", TK_KEYWORD__BOOL},
    {"_Complex", TK_KEYWORD__COMPLEX},
    {"_Imaginary", TK_KEYWORD__IMAGINARY},
};

// Checks beginning of `s` is `pat`.
static bool startwith(const char *s, const char *pat) {
    for (size_t i = 0; pat[i]; i++) {
        if (!s[i] || s[i] != pat[i]) return false;
    }
    return true;
}

// Returns true if `c` is octadecimal digit.
static bool isoct(char c) { return '0' <= c && c <= '7'; }

// Returns true if `c` is decimal digit.
static bool isdec(char c) { return '0' <= c && c <= '9'; }

// Returns true if `c` is hexadecimal digit.
static bool ishex(char c) {
    return '0' <= c && c <= '9' || 'a' <= c && c <= 'f' || 'A' <= c && c <= 'F';
}

// Convert hexadecimal digit to decimal number.
static char hex_to_num(char c) {
    if ('0' <= c && c <= '9') return c - '0';
    if ('a' <= c && c <= 'f') return 10 + c - 'a';
    if ('A' <= c && c <= 'F') return 10 + c - 'A';
    fatal_error("out of range character");
    return 0;
}

// Convert escape sequence in `s` into a character, then store result to `buf`.
static void escape(const char *s, struct string *buf) {
    string_init(buf);
    for (size_t i = 0; s[i]; i++) {
        if (s[i] != '\\') {
            string_push(buf, s[i]);
            continue;
        }

        i += 1;
        if (s[i] == '\'') {
            string_push(buf, '\'');
        } else if (s[i] == '"') {
            string_push(buf, '"');
        } else if (s[i] == '?') {
            string_push(buf, '\?');
        } else if (s[i] == '\\') {
            string_push(buf, '\\');
        } else if (s[i] == '\a') {
            string_push(buf, '\a');
        } else if (s[i] == '\b') {
            string_push(buf, '\b');
        } else if (s[i] == '\f') {
            string_push(buf, '\f');
        } else if (s[i] == '\n') {
            string_push(buf, '\n');
        } else if (s[i] == '\r') {
            string_push(buf, '\r');
        } else if (s[i] == '\t') {
            string_push(buf, '\t');
        } else if (s[i] == '\v') {
            string_push(buf, '\v');
        } else if (isoct(s[i])) {
            char c = s[i] - '0';
            if (s[i + 1] && isoct(s[i + 1])) {
                c = c * 8 + s[++i] - '0';
                if (s[i + 1] && isoct(s[i + 1])) {
                    c = c * 8 + s[++i] - '0';
                }
            }
            string_push(buf, c);
        } else if (s[i] == 'x') {
            char c = 0;
            while (true) {
                if (s[i + 1] && ishex(s[i + 1]))
                    c = c * 16 + hex_to_num(s[++i]);
                else
                    break;
            }
            string_push(buf, c);
        } else if (s[i] == 'u' || s[i] == 'U') {
            fatal_error("\\u or \\U is not yet implemented");
        } else {
            string_push(buf, s[i]);
        }
    }
}

// Convert pp number to integer or floating number.
static struct token *convert_pp_number(struct context *ctx,
                                       struct cpp_token *cpp_token) {
    // HACK: Separate functions to shorten code.

    assert(cpp_token->kind == CPP_TK_PP_NUMBER);

    // At first, try to parse pp-number as integer.

    char *s = cpp_token->pp_number_val.str;
    int radix = 0;
    if (startwith(s, "0x") || startwith(s, "0X")) {
        radix = 16;
    } else if (*s == '0') {
        radix = 8;
    } else {
        radix = 10;
    }

    unsigned long long int_val = strtoull(s, &s, radix);

    enum token_int_suffix int_suffix;
    if (startwith(s, "ull") || startwith(s, "uLL") || startwith(s, "Ull") ||
        startwith(s, "ULL") || startwith(s, "llu") || startwith(s, "LLu") ||
        startwith(s, "llU") || startwith(s, "LLU") || startwith(s, "ull")) {
        int_suffix = TK_INT_ULL;
        s += 3;
    } else if (startwith(s, "ll") || startwith(s, "LL")) {
        int_suffix = TK_INT_LL;
        s += 2;
    } else if (startwith(s, "ul") || startwith(s, "uL") || startwith(s, "Ul") ||
               startwith(s, "UL") || startwith(s, "lu") || startwith(s, "Lu") ||
               startwith(s, "lU") || startwith(s, "LU") || startwith(s, "ul")) {
        int_suffix = TK_INT_UL;
        s += 2;
    } else if (startwith(s, "l") || startwith(s, "L")) {
        int_suffix = TK_INT_L;
        s += 1;
    } else if (startwith(s, "u") || startwith(s, "U")) {
        int_suffix = TK_INT_U;
        s += 1;
    } else {
        int_suffix = TK_INT_NONE;
    }

    if (cpp_token->pp_number_val.str + cpp_token->pp_number_val.len == s) {
        // `strtoull` parses integer correctly.
        struct token *token = token_new(TK_INT, &cpp_token->span);
        token->int_val = int_val;
        token->int_radix = radix == 16 ? TK_INT_HEX
                           : 10        ? TK_INT_DEC
                                       : TK_INT_OCT;
        token->int_suffix = int_suffix;
        return token;
    } else {
        bool is_suffix = true;
        while (*s) {
            is_suffix &= 'a' <= *s && *s <= 'z' || 'A' <= *s && *s <= 'Z';
            s++;
        }
        if (is_suffix) {
            // Only suffix is invalid.
            struct report_info info = {REPORT_ERROR, cpp_token->span,
                                       "invalid suffix", ""};
            report(ctx, &info);
            return NULL;
        }
    }

    // If pp-number is not a integer, let's start to parse it as floating
    // number.

    // Decimal floating constant can start with '0'.
    radix = radix == 8 ? 10 : radix;

    s = cpp_token->pp_number_val.str;

    long double float_val = strtold(s, &s);

    enum token_float_suffix float_suffix;
    if (startwith(s, "f") || startwith(s, "F")) {
        float_suffix = TK_FLOAT_F;
    } else if (startwith(s, "l") || startwith(s, "L")) {
        float_suffix = TK_FLOAT_L;
    } else {
        float_suffix = TK_FLOAT_NONE;
    }

    if (cpp_token->pp_number_val.str + cpp_token->pp_number_val.len == s) {
        // `strtold` parses floating number correctly.
        struct token *token = token_new(TK_FLOAT, &cpp_token->span);
        token->float_val = float_val;
        token->float_radix = radix == 16 ? TK_FLOAT_HEX : TK_FLOAT_DEC;
        token->float_suffix = float_suffix;
        return token;
    } else {
        bool is_suffix = true;
        while (*s) {
            is_suffix &= 'a' <= *s && *s <= 'z' || 'A' <= *s && *s <= 'Z';
            s++;
        }
        if (is_suffix) {
            // Only suffix is invalid.
            struct report_info info = {REPORT_ERROR, cpp_token->span,
                                       "invalid suffix", ""};
            report(ctx, &info);
            return NULL;
        } else {
            struct report_info info = {REPORT_ERROR, cpp_token->span,
                                       "invalid floating number", ""};
            report(ctx, &info);
            return NULL;
        }
    }
}

// Convert `cpp_token` into `struct token`.
// Returns NULL if the conversion failed.
static struct token *convert_cpp_token_to_token(struct context *ctx,
                                                struct cpp_token *cpp_token) {
    struct token *token = NULL;
    if (cpp_token->kind == CPP_TK_IDENT) {
        for (size_t i = 0; i < sizeof keywords / sizeof keywords[0]; i++) {
            size_t len = strlen(keywords[i].name);
            if (!startwith(cpp_token->ident_val.str, keywords[i].name))
                continue;

            token = token_new(TK_KEYWORD, &cpp_token->span);
            token->keyword_kind = keywords[i].kind;
            return token;
        }
        token = token_new(TK_IDENT, &cpp_token->span);
        token->ident_val = cpp_token->ident_val;
        return token;
    } else if (cpp_token->kind == CPP_TK_PP_NUMBER) {
        return convert_pp_number(ctx, cpp_token);
    } else if (cpp_token->kind == CPP_TK_CHAR) {
        token = token_new(TK_CHAR, &cpp_token->span);
        escape(cpp_token->char_val.str, &token->char_val);
        return token;
    } else if (cpp_token->kind == CPP_TK_STRING) {
        if (cpp_token->has_backslash) {
            struct report_info info = {REPORT_ERROR, cpp_token->span,
                                       "floating backslash exists", ""};
            report(ctx, &info);
            return NULL;
        }
        token = token_new(TK_STRING, &cpp_token->span);
        escape(cpp_token->string_val.str, &token->string_val);
        return token;
    } else if (cpp_token->kind == CPP_TK_PUNCT) {
        struct token *token = token_new(TK_PUNCT, &cpp_token->span);
        token->punct_kind = cpp_token->punct_kind;
        return token;
    } else {
        struct report_info info = {REPORT_ERROR, cpp_token->span,
                                   "unknown character", ""};
        report(ctx, &info);
        return NULL;
    }
}

bool lex_file(struct context *ctx, const char *path, struct token **tokens) {
    struct cpp_token *cpp_tokens;
    if (!cpp_file(ctx, path, &cpp_tokens)) return false;

    struct token head = {NULL};
    struct token *prev = &head;

    cpp_token_iter(cpp_token, cpp_tokens) {
        if (cpp_token->kind == CPP_TK_SPACE ||
            cpp_token->kind == CPP_TK_NEWLINE)
            continue;

        prev->next = convert_cpp_token_to_token(ctx, cpp_token);
        if (!prev->next) return NULL;
        prev = prev->next;
    }

    *tokens = head.next;
    return true;
}
