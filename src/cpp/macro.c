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

#include "tinyc/cpp/macro.h"

#include <stdlib.h>

#include "tinyc/string.h"

struct tinyc_cpp_macro_func_param *tinyc_cpp_macro_func_param_create(
    const char *name,
    size_t index
) {
    struct tinyc_cpp_macro_func_param *arg = malloc(
        sizeof(struct tinyc_cpp_macro_func_param)
    );
    if (!arg) return NULL;
    arg->next = NULL;
    tinyc_string_from_copy(&arg->name, name);
    arg->index = index;
    return arg;
}

struct tinyc_cpp_macro_func_param *tinyc_cpp_macro_func_params_at(
    struct tinyc_cpp_macro_func_param *params,
    size_t n
) {
    struct tinyc_cpp_macro_func_param *it = params;
    while (it) {
        if (it->index == n) return it;
        it = it->next;
    }
    return NULL;
}

struct tinyc_cpp_macro *tinyc_cpp_macro_func_create(
    const char *name,
    struct tinyc_cpp_macro_func_param *args,
    struct tinyc_token *value
) {
    struct tinyc_cpp_macro_func *func = malloc(
        sizeof(struct tinyc_cpp_macro_func)
    );
    func->macro.next = NULL;
    func->macro.kind = TINYC_CPP_MACRO_FUNC;
    tinyc_string_from_copy(&func->macro.name, name);
    func->macro.value = value;
    func->params = args;
    return &func->macro;
}

struct tinyc_cpp_macro *tinyc_cpp_macro_normal_create(
    const char *name,
    struct tinyc_token *value
) {
    struct tinyc_cpp_macro_normal *normal = malloc(
        sizeof(struct tinyc_cpp_macro_normal)
    );
    normal->macro.next = NULL;
    normal->macro.kind = TINYC_CPP_MACRO_NORMAL;
    tinyc_string_from_copy(&normal->macro.name, name);
    normal->macro.value = value;
    return &normal->macro;
}
