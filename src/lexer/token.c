#include "token.h"

#include "../panic.h"

string_t token_integer_body(token_t *token) {
    if (token->kind != TK_INTEGER) panic_internal("constraint is not hold");
    string_t result;
    string_from(&result, token->repr.str);
    int suffix_size;
    if (token->int_.suffix == INT_SUFFIX_NONE) {
        suffix_size = 0;
    } else if (token->int_.suffix == INT_SUFFIX_U ||
               token->int_.suffix == INT_SUFFIX_L) {
        suffix_size = 1;
    } else if (token->int_.suffix == INT_SUFFIX_LL ||
               token->int_.suffix == INT_SUFFIX_UL) {
        suffix_size = 2;
    } else {
        suffix_size = 3;
    }
    for (int i = 0; i < suffix_size; i++) {
        string_pop(&result);
    }
    return result;
}

string_t token_floating_body(token_t *token) {
    if (token->kind != TK_FLOATING) panic_internal("constraint is not hold");
    string_t result;
    string_from(&result, token->repr.str);
    int suffix_size;
    if (token->float_.suffix == FLOAT_SUFFIX_NONE) {
        suffix_size = 0;
    } else {
        suffix_size = 1;
    }
    for (int i = 0; i < suffix_size; i++) {
        string_pop(&result);
    }
    return result;
}

string_t token_string_body(token_t *token) {
    if (token->kind != TK_STRING) panic_internal("constraint is not hold");
    string_t result;
    string_init(&result);
    for (int i = 1; i < token->repr.len - 1; i++) {
        string_push(&result, string_at(&token->repr, i));
    }
    return result;
}

string_t token_character_body(token_t *token) {
    if (token->kind != TK_CHARACTER) panic_internal("constraint is not hold");
    string_t result;
    string_init(&result);
    for (int i = 1; i < token->repr.len - 1; i++) {
        string_push(&result, string_at(&token->repr, i));
    }
    return result;
}

char *token_kind_to_string(token_kind_t kind) {
    switch (kind) {
        case TK_OR:
            return "||";
        case TK_AND:
            return "&&";
        case TK_VERTICAL:
            return "|";
        case TK_HAT:
            return "^";
        case TK_AMPERSAND:
            return "&";
        case TK_EQ:
            return "==";
        case TK_NE:
            return "!=";
        case TK_LT:
            return "<";
        case TK_GT:
            return ">";
        case TK_LE:
            return "<=";
        case TK_GE:
            return ">=";
        case TK_LSHIFT:
            return "<<";
        case TK_RSHIFT:
            return ">>";
        case TK_PLUS:
            return "+";
        case TK_PLUSPLUS:
            return "++";
        case TK_MINUS:
            return "-";
        case TK_MINUSMINUS:
            return "--";
        case TK_STAR:
            return "*";
        case TK_SLASH:
            return "/";
        case TK_PERCENT:
            return "%";
        case TK_LPAREN:
            return "(";
        case TK_RPAREN:
            return ")";
        case TK_LCURLY:
            return "{";
        case TK_RCURLY:
            return "}";
        case TK_LSQUARE:
            return "[";
        case TK_RSQUARE:
            return "]";
        case TK_SEMICOLON:
            return ";";
        case TK_COLON:
            return ":";
        case TK_COMMA:
            return ",";
        case TK_TILDE:
            return "~";
        case TK_NOT:
            return "!";
        case TK_QUESTION:
            return "?";
        case TK_SHARP:
            return "#";
        case TK_DOT:
            return ".";
        case TK_ARROW:
            return "->";
        case TK_ASSIGN:
            return "=";
        case TK_ORASSIGN:
            return "|=";
        case TK_XORASSIGN:
            return "^=";
        case TK_ANDASSIGN:
            return "&=";
        case TK_LSHIFTASSIGN:
            return "<<=";
        case TK_RSHIFTASSIGN:
            return ">>=";
        case TK_ADDASSIGN:
            return "+=";
        case TK_SUBASSIGN:
            return "-=";
        case TK_MULASSIGN:
            return "*=";
        case TK_DIVASSIGN:
            return "/=";
        case TK_MODASSIGN:
            return "%=";
        case TK_AUTO:
            return "auto";
        case TK_BREAK:
            return "break";
        case TK_CASE:
            return "case";
        case TK_CHAR:
            return "char";
        case TK_CONST:
            return "const";
        case TK_CONTINUE:
            return "continue";
        case TK_DEFAULT:
            return "default";
        case TK_DO:
            return "do";
        case TK_DOUBLE:
            return "double";
        case TK_ELSE:
            return "else";
        case TK_ENUM:
            return "enum";
        case TK_EXTERN:
            return "extern";
        case TK_FLOAT:
            return "float";
        case TK_FOR:
            return "for";
        case TK_GOTO:
            return "goto";
        case TK_IF:
            return "if";
        case TK_INLINE:
            return "inline";
        case TK_INT:
            return "int";
        case TK_LONG:
            return "long";
        case TK_REGISTER:
            return "register";
        case TK_RESTRICT:
            return "restrict";
        case TK_RETURN:
            return "return";
        case TK_SHORT:
            return "short";
        case TK_SIGNED:
            return "signed";
        case TK_SIZEOF:
            return "sizeof";
        case TK_STATIC:
            return "static";
        case TK_STRUCT:
            return "struct";
        case TK_SWITCH:
            return "switch";
        case TK_TYPEDEF:
            return "typedef";
        case TK_UNION:
            return "union";
        case TK_UNSIGNED:
            return "unsigned";
        case TK_VOID:
            return "void";
        case TK_VOLATILE:
            return "volatile";
        case TK_WHILE:
            return "while";
        case TK_INTEGER:
            return "integer";
        case TK_FLOATING:
            return "floating";
        case TK_IDENTIFIER:
            return "identifier";
        case TK_STRING:
            return "string";
        case TK_CHARACTER:
            return "character";
        case TK_SPACE:
            return "space";
        case TK_UNKNOWN:
            return "unknown";
    }
}
