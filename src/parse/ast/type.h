// Copyrignts (C) 2024 pogyomo. Released under the MIT license.

#ifndef TINYC_PARSE_AST_TYPE_H_
#define TINYC_PARSE_AST_TYPE_H_

#include <stdbool.h>

#include "../../span.h"

struct type {
    enum type_kind {
        TYPE_BUILTIN,
        TYPE_STRUCT_OR_UNION,
        TYPE_ENUM,
        TYPE_ARRAY,
        TYPE_POINTER,
        TYPE_FUNC,
        TYPE_TYPEDEF_NAME,
    } kind;
    struct span span;
    struct type_qual {
        struct type_qual *next;
        enum type_qual_kind {
            TYPE_QUAL_CONST,
            TYPE_QUAL_RESTRICT,
            TYPE_QUAL_VOLATILE,
        } kind;
        struct span span;
    } *quals;

    // Used when kind == TYPE_BUILTIN
    struct type_builtin {
        enum type_builtin_kind {
            TYPE_BUILTIN_VOID,       // `void`
            TYPE_BUILTIN_CHAR,       // `char`
            TYPE_BUILTIN_UCHAR,      // `unsigned char`
            TYPE_BUILTIN_SHORT,      // `short`
            TYPE_BUILTIN_USHORT,     // `unsigned short`
            TYPE_BUILTIN_INT,        // `int`
            TYPE_BUILTIN_UINT,       // `unsigned int`
            TYPE_BUILTIN_LONG,       // `long`
            TYPE_BUILTIN_ULONG,      // `unsigned long`
            TYPE_BUILTIN_LLONG,      // `long long`
            TYPE_BUILTIN_ULLONG,     // `unsigned long long`
            TYPE_BUILTIN_FLOAT,      // `float`
            TYPE_BUILTIN_DOUBLE,     // `double`
            TYPE_BUILTIN_LDOUBLE,    // `long double`
            TYPE_BUILTIN_BOOL,       // `_Bool`
            TYPE_BUILTIN_C_FLOAT,    // `float _Complex`
            TYPE_BUILTIN_C_DOUBLE,   // `double _Complex`
            TYPE_BUILTIN_C_LDOUBLE,  // `long double _Complex`
            TYPE_BUILTIN_I_FLOAT,    // `float _Imaginary`
            TYPE_BUILTIN_I_DOUBLE,   // `double _Imaginary`
            TYPE_BUILTIN_I_LDOUBLE,  // `long double _Imaginary`
        } kind;
    } builtin;

    // Used when kind == TYPE_STRUCT_OR_UNION
    struct type_struct_or_union {
        bool is_struct;
        struct type_struct_or_union_name {
            bool exists;  // false if no name specified.
            char *value;
            struct span span;
        } name;
        struct type_struct_or_union_decl {
            struct type_struct_or_union_decl *next;
            struct span span;
            struct type *type;
            struct type_struct_or_union_decl_name {
                char *value;
                struct span span;
            } name;
            struct expr *bit_field;  // NULL if no bit field specified.
        } *decls;
    } struct_or_union;

    // Used when kind == TYPE_ENUM
    struct type_enum {
        struct type_enum_name {
            bool exists;  // false if no name specified.
            char *value;
            struct span span;
        } name;
        struct type_enum_enumerator {
            struct type_enum_enumerator *next;
            struct span span;
            struct type_enum_enumerator_name {
                char *value;
                struct span span;
            } name;
            struct expr *value;  // NULL if no value specified.
        } *enumerators;
    } enum_;

    // Used when kind == TYPE_ARRAY
    struct type_array {
        struct type *of;
        bool has_static;
        struct type_array_size {
            enum type_array_size_kind {
                TYPE_ARRAY_SIZE_EXPR,
                TYPE_ARRAY_SIZE_STAR,
            } kind;
            struct span span;

            // Used if kind == TYPE_ARRAY_SIZE_EXPR
            struct expr *value;
        } *size;  // NULL if no size specified.
    } array;

    // Used when kind == TYPE_POINTER
    struct type_pointer {
        struct type *of;
    } pointer;

    // Used when kind == TYPE_FUNC
    struct type_func {
        struct type *ret_type;
        struct type_func_param {
            struct type_func_param *next;
            enum type_func_param_kind {
                TYPE_FUNC_PARAM_NORMAL,
                TYPE_FUNC_PARAM_VARIDIC,
            } kind;
            struct span span;

            // Used when kind == TYPE_FUNC_PARAM_NORMAL
            struct type *type;
            struct type_func_param_name {
                bool exists;  // false if no name specified.
                char *value;
                struct span span;
            } name;
        } *params;
    } func;

    // Used when kind == TYPE_TYPEDEF_NAME,
    struct type_typedef_name {
        char *value;
    } typedef_name;
};

struct type *type_new();
struct type *type_builtin_new(enum type_builtin_kind kind,
                              struct type_qual *quals, struct span *span);
struct type *type_struct_or_union_new(bool is_struct, struct type_qual *quals,
                                      struct type_struct_or_union_name *name,
                                      struct type_struct_or_union_decl *decls,
                                      struct span *span);
struct type_struct_or_union_decl *type_struct_or_union_decl_new(
    struct type *type, struct type_struct_or_union_decl_name *name,
    struct span *span, struct expr *bit_field);
struct type *type_enum_new(struct type_enum_name *name, struct type_qual *quals,
                           struct type_enum_enumerator *enumerators,
                           struct span *span);
struct type_enum_enumerator *type_enum_enumerator_new(
    struct type_enum_enumerator_name *name, struct expr *value,
    struct span *span);
struct type *type_array_new(struct type *of, bool has_static,
                            struct type_qual *quants,
                            struct type_array_size *size, struct span *span);
struct type_array_size *type_array_size_new();
struct type_array_size *type_array_size_expr_new(struct expr *value,
                                                 struct span *span);
struct type_array_size *type_array_size_star_new(struct span *span);
struct type *type_pointer_new(struct type *of, struct type_qual *quants,
                              struct span *span);
struct type *type_func_new(struct type *ret_type,
                           struct type_func_param *params, struct span *span);
struct type_func_param *type_func_param_new();
struct type_func_param *type_func_param_normal_new(
    struct type *type, struct type_func_param_name *name, struct span *span);
struct type_func_param *type_func_param_varidic_new(struct span *span);
struct type *type_typedef_name_new(char *value, struct span *span);
struct type_qual *type_qual_new(enum type_qual_kind kind, struct span *span);

#endif  // TINYC_PARSE_AST_TYPE_H_
