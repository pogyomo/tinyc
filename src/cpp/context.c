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

#include "tinyc/cpp/context.h"

void tinyc_cpp_context_init(struct tinyc_cpp_context *this) {
    this->if_count = 0;
    this->macros = NULL;
}

void tinyc_cpp_insert_macro(
    struct tinyc_cpp_context *this,
    struct tinyc_cpp_macro *macro
) {
    if (this->macros) {
        macro->next = this->macros;
        this->macros = macro;
    } else {
        this->macros = macro;
    }
}
