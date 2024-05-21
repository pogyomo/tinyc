#include "preprocessor.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "../lexer/lexer.h"
#include "../panic.h"
#include "../report.h"
#include "../stream.h"
#include "macro.h"

typedef struct {
    span_t if_span;
    bool shold_skip;
} if_state_t;

typedef struct {
    vector_t states;
} if_states_t;

void if_states_init(if_states_t *states) {
    vector_init(&states->states, sizeof(if_states_t));
}

bool if_states_empty(if_states_t *states) { return states->states.len == 0; }

void if_states_push(if_states_t *states, if_state_t state) {
    vector_push(&states->states, &state);
}

void if_states_pop(if_states_t *states) { vector_pop(&states->states); }

if_state_t *if_states_top(if_states_t *states) {
    if (states->states.len == 0) panic_internal("constraint is not hold");
    return vector_at(&states->states, states->states.len - 1);
}

void skip_spaces(tstream_t *ts) {
    while (!tstream_eos(ts)) {
        if (tstream_token(ts)->kind == TK_SPACE) {
            tstream_advance(ts);
        } else {
            break;
        }
    }
}

bool parse_macro_args(context_t *ctx, tstream_t *line, vector_t *args) {
    int count;
    vector_t arg;
    span_t args_span;

    if (!tstream_is(line, TK_LPAREN)) {
        panic_internal("constraint is not hold");
    }
    args_span = tstream_token(line)->span;
    tstream_advance(line);

    count = 1;
    vector_init(args, sizeof(vector_t));
    vector_init(&arg, sizeof(token_t));
    while (true) {
        if (tstream_eos(line)) {
            string_t what, info;
            string_from(&what, "unclosing macro arguments");
            string_init(&info);
            report(ctx, REPORT_LEVEL_ERROR,
                   (report_info_t){what, info, args_span});
            return false;
        } else if (tstream_is(line, TK_LPAREN)) {
            args_span = concat_span(args_span, tstream_token(line)->span);
            count++;
            vector_push(&arg, tstream_token(line));
            tstream_advance(line);
        } else if (tstream_is(line, TK_RPAREN)) {
            args_span = concat_span(args_span, tstream_token(line)->span);
            if (--count == 0) {
                vector_push(args, &arg);
                tstream_advance(line);
                break;
            } else {
                vector_push(&arg, tstream_token(line));
                tstream_advance(line);
            }
        } else if (tstream_is(line, TK_COMMA)) {
            args_span = concat_span(args_span, tstream_token(line)->span);
            if (count == 1) {
                vector_push(args, &arg);
                vector_init(&arg, sizeof(token_t));
                tstream_advance(line);
            } else {
                vector_push(&arg, tstream_token(line));
                tstream_advance(line);
            }
        } else {
            vector_push(&arg, tstream_token(line));
            tstream_advance(line);
        }
    }
    return true;
}

bool parse_macro_def(context_t *ctx, tstream_t *line) {
    macro_t macro;
    string_t name;
    span_t macro_span;
    if (!tstream_is(line, TK_IDENTIFIER)) {
        panic_internal("constraint is not hold");
    }
    name = tstream_token(line)->repr;
    macro_span = tstream_token(line)->span;
    tstream_advance(line);

    string_t param;
    vector_t body, params;
    vector_init(&body, sizeof(token_t));
    vector_init(&params, sizeof(string_t));
    if (tstream_is(line, TK_LPAREN)) {
        tstream_advance(line);
        while (true) {
            skip_spaces(line);
            if (tstream_eos(line)) {
                string_t what, info;
                string_from(&what, "unclosing macro definition");
                string_init(&info);
                report(ctx, REPORT_LEVEL_ERROR,
                       (report_info_t){what, info, macro_span});
                return false;
            } else if (tstream_is(line, TK_RPAREN)) {
                macro_span = concat_span(macro_span, tstream_token(line)->span);
                tstream_advance(line);
                break;
            } else if (tstream_is(line, TK_IDENTIFIER)) {
                macro_span = concat_span(macro_span, tstream_token(line)->span);
                param = tstream_token(line)->repr;
                vector_push(&params, &param);
                tstream_advance(line);
            } else {
                string_t what, info;
                string_from(&what, "invalid macro parameter");
                string_init(&info);
                report(ctx, REPORT_LEVEL_ERROR,
                       (report_info_t){what, info, tstream_token(line)->span});
                return false;
            }

            skip_spaces(line);
            if (tstream_eos(line)) {
                string_t what, info;
                string_from(&what, "unclosing macro definition");
                string_init(&info);
                report(ctx, REPORT_LEVEL_ERROR,
                       (report_info_t){what, info, macro_span});
                return false;
            } else if (tstream_is(line, TK_RPAREN)) {
                macro_span = concat_span(macro_span, tstream_token(line)->span);
                tstream_advance(line);
                break;
            } else if (tstream_is(line, TK_COMMA)) {
                macro_span = concat_span(macro_span, tstream_token(line)->span);
                tstream_advance(line);
            } else {
                string_t what, info;
                string_from(&what, "invalid macro delimiter");
                string_init(&info);
                report(ctx, REPORT_LEVEL_ERROR,
                       (report_info_t){what, info, tstream_token(line)->span});
                return false;
            }
        }
        while (!tstream_eos(line)) {
            vector_push(&body, tstream_token(line));
            tstream_advance(line);
        }
        macro_function(&macro, body, params);
        mtable_insert(&ctx->mtable, name.str, macro);
        return true;
    } else {
        while (!tstream_eos(line)) {
            vector_push(&body, tstream_token(line));
            tstream_advance(line);
        }
        macro_object(&macro, body);
        mtable_insert(&ctx->mtable, name.str, macro);
        return true;
    }
}

bool parse_normal(context_t *ctx, tstream_t *line, vector_t *output) {
    while (!tstream_eos(line)) {
        token_t *name;
        macro_t *macro;

        if (!tstream_is(line, TK_IDENTIFIER)) {
            vector_push(output, tstream_token(line));
            tstream_advance(line);
            continue;
        }
        name = tstream_token(line);
        tstream_advance(line);

        macro = mtable_find(&ctx->mtable, name->repr.str);
        if (macro == NULL) {
            vector_push(output, name);
        } else if (macro->kind == MACRO_OBJECT) {
            vector_t expanded;
            preprocess(ctx, &macro->body, &expanded);
            vector_append(output, &expanded);
        } else if (tstream_is(line, TK_LPAREN)) {
            vector_t args;
            if (!parse_macro_args(ctx, line, &args)) {
                return false;
            }
            assert(args.len == macro->params.len);

            token_t *token;
            string_t *param;
            vector_t expanded, expanded_out;
            vector_init(&expanded, sizeof(token_t));
            for (int i = 0; i < macro->body.len; i++) {
                token = vector_at(&macro->body, i);
                if (token->kind != TK_IDENTIFIER) {
                    vector_push(&expanded, token);
                    continue;
                }
                bool replaced = false;
                for (int j = 0; j < macro->params.len; j++) {
                    param = vector_at(&macro->params, j);
                    if (strcmp(param->str, token->repr.str) == 0) {
                        vector_append(&expanded, vector_at(&args, j));
                        replaced = true;
                        break;
                    }
                }
                if (!replaced) {
                    vector_push(&expanded, token);
                }
            }
            preprocess(ctx, &expanded, &expanded_out);
            vector_append(output, &expanded_out);
        } else {
            vector_push(output, name);
        }
    }
    return true;
}

bool parse_directive(context_t *ctx, tstream_t *line, if_states_t *states,
                     vector_t *output) {
    if (!tstream_is(line, TK_SHARP)) {
        panic_internal("constraint is not hold");
    }
    span_t directive_span = tstream_token(line)->span;
    tstream_advance(line);
    skip_spaces(line);

    if (!tstream_is(line, TK_IDENTIFIER) && !tstream_is(line, TK_ELSE)) {
        string_t what, info;
        string_from(&what, "expected directive name after #");
        string_init(&info);
        report(ctx, REPORT_LEVEL_ERROR,
               (report_info_t){what, info, directive_span});
        return false;
    }
    directive_span = concat_span(directive_span, tstream_token(line)->span);
    string_t directive_name = tstream_token(line)->repr;
    tstream_advance(line);

    if (strcmp(directive_name.str, "include") == 0) {
        if (!if_states_empty(states) && if_states_top(states)->shold_skip)
            return true;

        skip_spaces(line);
        vector_t path_list;
        vector_init(&path_list, sizeof(string_t));
        if (tstream_is(line, TK_LT)) {
            directive_span =
                concat_span(directive_span, tstream_token(line)->span);
            tstream_advance(line);

            string_t path_base;
            string_init(&path_base);
            while (true) {
                if (tstream_eos(line)) {
                    string_t what, info;
                    string_from(&what, "unclosing include");
                    string_init(&info);
                    report(ctx, REPORT_LEVEL_ERROR,
                           (report_info_t){what, info, directive_span});
                    return false;
                } else if (tstream_is(line, TK_GT)) {
                    directive_span =
                        concat_span(directive_span, tstream_token(line)->span);
                    tstream_advance(line);
                    break;
                } else {
                    directive_span =
                        concat_span(directive_span, tstream_token(line)->span);
                    string_append(&path_base, tstream_token(line)->repr.str);
                    tstream_advance(line);
                }
            }
            string_t path;
            string_format(&path, "/usr/include/%s", path_base.str);
            vector_push(&path_list, &path);
        } else if (tstream_is(line, TK_STRING)) {
            directive_span =
                concat_span(directive_span, tstream_token(line)->span);
            string_t path = token_string_inner(tstream_token(line));
            vector_push(&path_list, &path);
        } else {
            string_t what, info;
            string_from(&what, "expected \" or < after include");
            string_init(&info);
            report(ctx, REPORT_LEVEL_ERROR,
                   (report_info_t){what, info, directive_span});
            return false;
        }

        FILE *fp;
        string_t *path;
        for (int i = 0; i < path_list.len; i++) {
            path = vector_at(&path_list, i);
            fp = fopen(path->str, "r");
            if (!fp) continue;
            fclose(fp);
            vector_t included;
            if (!lex_file(ctx, path->str, &included)) {
                return false;
            }
            vector_append(output, &included);
            return true;
        }
        string_t what, info;
        string_format(&what, "%s", strerror(errno));
        string_init(&info);
        report(ctx, REPORT_LEVEL_ERROR,
               (report_info_t){what, info, directive_span});
        return false;
    } else if (strcmp(directive_name.str, "define") == 0) {
        if (!if_states_empty(states) && if_states_top(states)->shold_skip)
            return true;

        skip_spaces(line);
        if (!tstream_is(line, TK_IDENTIFIER)) {
            string_t what, info;
            string_from(&what, "expected macro name after define");
            string_init(&info);
            report(ctx, REPORT_LEVEL_ERROR,
                   (report_info_t){what, info, directive_span});
            return false;
        }
        return parse_macro_def(ctx, line);
    } else if (strcmp(directive_name.str, "ifndef") == 0) {
        if (!if_states_empty(states) && if_states_top(states)->shold_skip) {
            if_states_push(states, (if_state_t){directive_span, true});
            return true;
        }

        skip_spaces(line);
        if (!tstream_is(line, TK_IDENTIFIER)) {
            string_t what, info;
            string_from(&what, "expected macro name after ifndef");
            string_init(&info);
            report(ctx, REPORT_LEVEL_ERROR,
                   (report_info_t){what, info, directive_span});
            return false;
        }
        directive_span = concat_span(directive_span, tstream_token(line)->span);
        string_t macro_name = tstream_token(line)->repr;
        tstream_advance(line);

        bool shold_skip = mtable_find(&ctx->mtable, macro_name.str) != NULL;
        if_states_push(states, (if_state_t){directive_span, shold_skip});
        return true;
    } else if (strcmp(directive_name.str, "ifdef") == 0) {
        if (!if_states_empty(states) && if_states_top(states)->shold_skip) {
            if_states_push(states, (if_state_t){directive_span, true});
            return true;
        }

        skip_spaces(line);
        if (!tstream_is(line, TK_IDENTIFIER)) {
            string_t what, info;
            string_from(&what, "expected macro name after ifndef");
            string_init(&info);
            report(ctx, REPORT_LEVEL_ERROR,
                   (report_info_t){what, info, directive_span});
            return false;
        }
        directive_span = concat_span(directive_span, tstream_token(line)->span);
        string_t macro_name = tstream_token(line)->repr;
        tstream_advance(line);

        bool shold_skip = mtable_find(&ctx->mtable, macro_name.str) == NULL;
        if_states_push(states, (if_state_t){directive_span, shold_skip});
        return true;
    } else if (strcmp(directive_name.str, "else") == 0) {
        if (if_states_empty(states)) {
            string_t what, info;
            string_from(&what, "stray else");
            string_init(&info);
            report(ctx, REPORT_LEVEL_ERROR,
                   (report_info_t){what, info, directive_span});
            return false;
        } else {
            bool shold_skip = !if_states_top(states)->shold_skip;
            if_states_pop(states);
            if_states_push(states, (if_state_t){directive_span, shold_skip});
            return true;
        }
    } else if (strcmp(directive_name.str, "endif") == 0) {
        if (if_states_empty(states)) {
            string_t what, info;
            string_from(&what, "stray endif");
            string_init(&info);
            report(ctx, REPORT_LEVEL_ERROR,
                   (report_info_t){what, info, directive_span});
            return false;
        } else {
            if_states_pop(states);
            return true;
        }
    } else if (strcmp(directive_name.str, "error") == 0) {
        if (!if_states_empty(states) && if_states_top(states)->shold_skip)
            return true;

        string_t what, info;
        string_init(&what);
        string_init(&info);
        while (!tstream_eos(line)) {
            string_append(&what, tstream_token(line)->repr.str);
            tstream_advance(line);
        }
        report(ctx, REPORT_LEVEL_ERROR,
               (report_info_t){what, info, directive_span});
        return false;
    } else {
        if (!if_states_empty(states) && if_states_top(states)->shold_skip)
            return true;

        string_t what, info;
        string_from(&what, "unknown directive name");
        string_format(&info, "`%s` is invalid", directive_name.str);
        report(ctx, REPORT_LEVEL_ERROR,
               (report_info_t){what, info, directive_span});
        return false;
    }
}

bool parse_line(context_t *ctx, tstream_t *line, if_states_t *states,
                vector_t *output) {
    tstream_state_t state = tstream_state(line);
    skip_spaces(line);

    if (tstream_eos(line) || tstream_token(line)->kind != TK_SHARP) {
        if (if_states_empty(states) || !if_states_top(states)->shold_skip) {
            tstream_set_state(line, state);
            return parse_normal(ctx, line, output);
        } else {
            return true;
        }
    } else {
        return parse_directive(ctx, line, states, output);
    }
}

bool preprocess(context_t *ctx, vector_t *restrict input,
                vector_t *restrict output) {
    if (input->len == 0) {
        *output = *input;
        return true;
    }
    vector_init(output, sizeof(token_t));

    size_t lrow;
    token_t *token;
    vector_t lines, line;
    vector_init(&lines, sizeof(vector_t));
    vector_init(&line, sizeof(token_t));
    token = vector_at(input, 0);
    lrow = token->lrow;
    for (int i = 0; i < input->len; i++) {
        token = vector_at(input, i);
        if (token->lrow != lrow) {
            lrow = token->lrow;
            vector_push(&lines, &line);
            vector_init(&line, sizeof(token_t));
        }
        vector_push(&line, token);
    }
    if (line.len != 0) {
        vector_push(&lines, &line);
    }

    bool success = true;
    tstream_t ts;
    if_states_t states;
    if_states_init(&states);
    for (int i = 0; i < lines.len; i++) {
        line = *(vector_t *)vector_at(&lines, i);
        tstream_init(&ts, line);
        success &= parse_line(ctx, &ts, &states, output);
    }
    return success;
}
