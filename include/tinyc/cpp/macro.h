// Copyright 2024 pogyomo
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef TINYC_CPP_MACRO_H_
#define TINYC_CPP_MACRO_H_

#include <stddef.h>

#include "tinyc/string.h"

struct tinyc_cpp_macro {
    struct tinyc_cpp_macro *next;
    enum tinyc_cpp_macro_kind {
        TINYC_CPP_MACRO_FUNC,
        TINYC_CPP_MACRO_NORMAL,
    } kind;
    struct tinyc_string name;
    struct tinyc_token *value;  // NULL if no body exists.
};

struct tinyc_cpp_macro_func_param {
    struct tinyc_cpp_macro_func_param *next;
    struct tinyc_string name;
    size_t index;
};

struct tinyc_cpp_macro_func {
    struct tinyc_cpp_macro macro;
    struct tinyc_cpp_macro_func_param *params;
};

struct tinyc_cpp_macro_normal {
    struct tinyc_cpp_macro macro;
};

/// Create a new function-like macro arg.
/// Returns NULL if creation failed.
struct tinyc_cpp_macro_func_param *tinyc_cpp_macro_func_param_create(
    const char *name,
    size_t index
);

/// Returns n-th arguments, or NULL if not found.
struct tinyc_cpp_macro_func_param *tinyc_cpp_macro_func_params_at(
    struct tinyc_cpp_macro_func_param *params,
    size_t n
);

/// Create a new function-like macro.
/// Returns NULL if creation failed.
struct tinyc_cpp_macro *tinyc_cpp_macro_func_create(
    const char *name,
    struct tinyc_cpp_macro_func_param *params,
    struct tinyc_token *value
);

/// Create a new normal macro.
/// Returns NULL if creation failed.
struct tinyc_cpp_macro *tinyc_cpp_macro_normal_create(
    const char *name,
    struct tinyc_token *value
);

#endif  // TINYC_CPP_MACRO_H_
