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
#include <tinyc/repo.h>

#include "tinyc/source.h"
#include "tinyc/string.h"

static inline struct tinyc_source generate_source(char *name, char *content) {
    struct tinyc_string name_, content_;
    tinyc_string_from(&name_, name);
    tinyc_string_from(&content_, content);
    return (struct tinyc_source){name_, content_};
}

static inline bool query_expect(
    const struct tinyc_repo *repo,
    tinyc_repo_id id,
    const struct tinyc_source *source
) {
    struct tinyc_source *queried = tinyc_repo_query(repo, id);
    return queried != NULL &&
           tinyc_string_cmp(&queried->name, &source->name) == 0 &&
           tinyc_string_cmp(&queried->content, &source->content) == 0;
}

void register_query(void) {
    struct tinyc_repo repo;
    assert(tinyc_repo_init(&repo));
    assert(tinyc_repo_query(&repo, 0) == NULL);

    struct tinyc_source source1 = generate_source("name1", "content1");
    tinyc_repo_id id1 = tinyc_repo_registory(&repo, &source1);
    assert(query_expect(&repo, id1, &source1));
    assert(tinyc_repo_query(&repo, id1 + 1) == NULL);

    struct tinyc_source source2 = generate_source("name2", "content2");
    tinyc_repo_id id2 = tinyc_repo_registory(&repo, &source2);
    assert(query_expect(&repo, id2, &source2));
    assert(tinyc_repo_query(&repo, id2 + 1) == NULL);
}

int main(void) {
    register_query();
}
