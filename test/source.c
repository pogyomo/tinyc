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

#include <assert.h>
#include <stdio.h>
#include <tinyc/source.h>

#include "tinyc/string.h"

void init_from_file(void) {
    struct tinyc_string name, content;
    tinyc_string_from(&name, "name");
    tinyc_string_from(&content, "hello\nworld\n");

    FILE *fp = tmpfile();
    for (size_t i = 0; i < content.len; i++) {
        fputc(content.cstr[i], fp);
    }
    rewind(fp);

    struct tinyc_source source;
    assert(tinyc_source_from_fs(&source, name, fp));
    assert(tinyc_string_cmp(source.name, name) == 0);
    assert(tinyc_string_cmp(source.content, content) == 0);
}

int main(void) {
    init_from_file();
}
