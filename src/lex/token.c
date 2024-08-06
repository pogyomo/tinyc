#include "token.h"

#include <stdlib.h>

#include "../panic.h"

char *token_punct_kind_to_str(enum token_punct_kind kind) {
    switch (kind) {
        case TK_PUNCT_OR:
            return "||";
        case TK_PUNCT_AND:
            return "&&";
        case TK_PUNCT_VERTICAL:
            return "|";
        case TK_PUNCT_HAT:
            return "^";
        case TK_PUNCT_AMPERSAND:
            return "&";
        case TK_PUNCT_EQ:
            return "==";
        case TK_PUNCT_NE:
            return "!=";
        case TK_PUNCT_LT:
            return "<";
        case TK_PUNCT_GT:
            return ">";
        case TK_PUNCT_LE:
            return "<=";
        case TK_PUNCT_GE:
            return ">=";
        case TK_PUNCT_LSHIFT:
            return "<<";
        case TK_PUNCT_RSHIFT:
            return ">>";
        case TK_PUNCT_PLUS:
            return "+";
        case TK_PUNCT_PLUSPLUS:
            return "++";
        case TK_PUNCT_MINUS:
            return "-";
        case TK_PUNCT_MINUSMINUS:
            return "--";
        case TK_PUNCT_STAR:
            return "*";
        case TK_PUNCT_SLASH:
            return "/";
        case TK_PUNCT_PERCENT:
            return "%";
        case TK_PUNCT_LPAREN:
            return "(";
        case TK_PUNCT_RPAREN:
            return ")";
        case TK_PUNCT_LCURLY:
            return "{";
        case TK_PUNCT_RCURLY:
            return "}";
        case TK_PUNCT_LSQUARE:
            return "[";
        case TK_PUNCT_RSQUARE:
            return "]";
        case TK_PUNCT_SEMICOLON:
            return ";";
        case TK_PUNCT_COLON:
            return ":";
        case TK_PUNCT_COMMA:
            return ",";
        case TK_PUNCT_TILDE:
            return "~";
        case TK_PUNCT_NOT:
            return "!";
        case TK_PUNCT_QUESTION:
            return "?";
        case TK_PUNCT_SHARP:
            return "#";
        case TK_PUNCT_DOT:
            return ".";
        case TK_PUNCT_DOTDOTDOT:
            return "...";
        case TK_PUNCT_ARROW:
            return "->";
        case TK_PUNCT_ASSIGN:
            return "=";
        case TK_PUNCT_ORASSIGN:
            return "|=";
        case TK_PUNCT_XORASSIGN:
            return "^=";
        case TK_PUNCT_ANDASSIGN:
            return "&=";
        case TK_PUNCT_LSHIFTASSIGN:
            return "<<=";
        case TK_PUNCT_RSHIFTASSIGN:
            return ">>=";
        case TK_PUNCT_ADDASSIGN:
            return "+=";
        case TK_PUNCT_SUBASSIGN:
            return "-=";
        case TK_PUNCT_MULASSIGN:
            return "*=";
        case TK_PUNCT_DIVASSIGN:
            return "/=";
        case TK_PUNCT_MODASSIGN:
            return "%=";
        default:
            fatal_error("unreachable");
    }
}

char *token_keyword_kind_to_str(enum token_keyword_kind kind) {
    switch (kind) {
        case TK_KEYWORD_SIGNED:
            return "signed";
        case TK_KEYWORD_UNSIGNED:
            return "unsigned";
        case TK_KEYWORD_VOID:
            return "void";
        case TK_KEYWORD_CHAR:
            return "char";
        case TK_KEYWORD_SHORT:
            return "short";
        case TK_KEYWORD_LONG:
            return "long";
        case TK_KEYWORD_INT:
            return "int";
        case TK_KEYWORD_FLOAT:
            return "float";
        case TK_KEYWORD_DOUBLE:
            return "double";
        case TK_KEYWORD__BOOL:
            return "_Bool";
        case TK_KEYWORD__COMPLEX:
            return "_Complex";
        case TK_KEYWORD__IMAGINARY:
            return "_Imaginary";
        case TK_KEYWORD_AUTO:
            return "auto";
        case TK_KEYWORD_BREAK:
            return "break";
        case TK_KEYWORD_CASE:
            return "case";
        case TK_KEYWORD_CONST:
            return "const";
        case TK_KEYWORD_CONTINUE:
            return "continue";
        case TK_KEYWORD_DEFAULT:
            return "default";
        case TK_KEYWORD_DO:
            return "do";
        case TK_KEYWORD_ELSE:
            return "else";
        case TK_KEYWORD_ENUM:
            return "enum";
        case TK_KEYWORD_EXTERN:
            return "extern";
        case TK_KEYWORD_FOR:
            return "for";
        case TK_KEYWORD_GOTO:
            return "goto";
        case TK_KEYWORD_IF:
            return "if";
        case TK_KEYWORD_INLINE:
            return "inline";
        case TK_KEYWORD_REGISTER:
            return "register";
        case TK_KEYWORD_RESTRICT:
            return "restrict";
        case TK_KEYWORD_RETURN:
            return "return";
        case TK_KEYWORD_SIZEOF:
            return "sizeof";
        case TK_KEYWORD_STATIC:
            return "static";
        case TK_KEYWORD_STRUCT:
            return "struct";
        case TK_KEYWORD_SWITCH:
            return "switch";
        case TK_KEYWORD_TYPEDEF:
            return "typedef";
        case TK_KEYWORD_UNION:
            return "union";
        case TK_KEYWORD_VOLATILE:
            return "volatile";
        case TK_KEYWORD_WHILE:
            return "while";
        default:
            fatal_error("unreachable");
    }
}

struct token *token_new(enum token_kind kind, struct span *span) {
    struct token *token = malloc(sizeof(struct token));
    if (!token) fatal_error("memory allocation failed");
    token->next = NULL;
    token->kind = kind;
    token->span = *span;
    return token;
}
