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

#ifndef TINYC_REPO_H_
#define TINYC_REPO_H_

#include <stdbool.h>

#include "tinyc/source.h"

typedef long long tinyc_repo_id;

struct tinyc_repo_entry {
    tinyc_repo_id id;
    struct tinyc_repo_entry *next;
    struct tinyc_source source;
};

/// Manages sources by id.
struct tinyc_repo {
    tinyc_repo_id next_id;
    struct tinyc_repo_entry *head;
};

/// Initialize repository.
bool tinyc_repo_init(struct tinyc_repo *this);

/// Register source, return id for it.
/// Returns negative value if it failed.
tinyc_repo_id tinyc_repo_registory(
    struct tinyc_repo *this,
    struct tinyc_source source
);

/// Try to get source from repository by id.
/// Returns NULL if no such source exists.
struct tinyc_source *tinyc_repo_query(struct tinyc_repo this, tinyc_repo_id id);

#endif  // TINYC_REPO_H_
