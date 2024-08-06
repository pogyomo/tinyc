#include "parse.h"

#include <assert.h>
#include <stdio.h>

#include "../lex/lex.h"
#include "ast/decl.h"
#include "ast/fun_def.h"
#include "ast/unit.h"
#include "context.h"
#include "decl.h"
#include "fun_def.h"
#include "stream.h"

bool parse_file(struct context *ctx, const char *path,
                struct trans_unit **units) {
    assert(units != NULL);

    struct token *tokens;
    if (!lex_file(ctx, path, &tokens)) return false;
    if (tokens == NULL) {
        *units = NULL;
        return true;
    }

    struct parse_context parse_ctx;
    parse_context_init(&parse_ctx, ctx, false);

    struct trans_unit head = {NULL};
    struct trans_unit *prev = &head;
    struct tstream ts;
    tstream_init(&ts, tokens);
    while (!tstream_eos(&ts)) {
        struct span span = tstream_curr(&ts)->span;

        struct fun_def *def;
        struct decl *decl;
        struct tstream ts_save = ts;
        bool fallback;
        if (parse_fun_def(&parse_ctx, &ts, &def, &fallback)) {
            merge_span(&span, &tstream_last(&ts)->span, &span);
            prev->next = tran_unit_fun_def_new(def, &span);
            prev = prev->next;
        } else {
            if (!fallback) {
                return false;
            }
            ts = ts_save;
            TRY(parse_decl(&parse_ctx, &ts, &decl));
            merge_span(&span, &tstream_last(&ts)->span, &span);
            prev->next = tran_unit_decl_new(decl, &span);
            prev = prev->next;
        }
    }

    *units = head.next;
    return true;
}
