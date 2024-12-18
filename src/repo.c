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

#include "tinyc/repo.h"

#include <stdlib.h>

static inline tinyc_repo_id create(
    struct tinyc_repo *this,
    const struct tinyc_source *source
) {
    this->head = malloc(sizeof(struct tinyc_repo_entry));
    if (!this->head) return -1;
    this->head->id = this->next_id++;
    this->head->next = NULL;
    this->head->source = *source;
    return this->head->id;
}

static inline tinyc_repo_id append(
    struct tinyc_repo *this,
    const struct tinyc_source *source
) {
    struct tinyc_repo_entry *entry = malloc(sizeof(struct tinyc_repo_entry));
    if (!entry) return -1;
    entry->id = this->next_id++;
    entry->next = this->head;
    entry->source = *source;
    this->head = entry;
    return this->head->id;
}

bool tinyc_repo_init(struct tinyc_repo *this) {
    this->next_id = 0;
    this->head = NULL;
    return true;
}

tinyc_repo_id tinyc_repo_registory(
    struct tinyc_repo *this,
    const struct tinyc_source *source
) {
    if (this->head) {
        return append(this, source);
    } else {
        return create(this, source);
    }
}

const struct tinyc_source *tinyc_repo_query(
    const struct tinyc_repo *this,
    tinyc_repo_id id
) {
    for (struct tinyc_repo_entry *it = this->head; it; it = it->next) {
        if (it->id == id) return &it->source;
    }
    return NULL;
}
