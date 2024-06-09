#include "parser.h"

#include "../lexer/lexer.h"
#include "../stream.h"
#include "ast.h"
#include "parser_decl.h"
#include "utils.h"

bool parse_file(context_t *ctx, char *path, vector_t *decls) {
    vector_t tokens;
    lex_file(ctx, path, &tokens);

    tstream_t ts;
    tstream_init(&ts, &tokens);
    decl_t decl;
    vector_init(decls, sizeof(decl_t));
    while (tstream_eos(&ts)) {
        TRY(parse_decl(ctx, &ts, &decl));
        vector_push(decls, &decl);
    }
    return true;
}
