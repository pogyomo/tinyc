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

#ifndef TINYC_DIAG_H_
#define TINYC_DIAG_H_

#include "tinyc/repo.h"
#include "tinyc/span.h"

enum tinyc_diag_severity {
    TINYC_DIAG_INFO,
    TINYC_DIAG_WARN,
    TINYC_DIAG_ERROR,
};

/// Show diagnostic to stdout.
void tinyc_diag(
    enum tinyc_diag_severity severity,
    const struct tinyc_repo *repo,
    const struct tinyc_span *span,
    const char *what,
    const char *message
);

/// Write diagnostic to file stream.
/// Unlike tinyc_diag, it doesn't attach any color to output.
void tinyc_diag_fs(
    FILE *fs,
    enum tinyc_diag_severity severity,
    const struct tinyc_repo *repo,
    const struct tinyc_span *span,
    const char *what,
    const char *message
);

#endif  // TINYC_DIAG_H_
