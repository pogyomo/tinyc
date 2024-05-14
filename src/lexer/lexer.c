#include "lexer.h"

#include "../input/input.h"
#include "../input/stream.h"
#include "stream.h"

token_t *token(istream_t *is, icache_id_t id) {
    position_t start = istream_pos(is);
    position_t end = istream_pos(is);
    if (istream_accept(is, '+', &end)) {
    } else {
    }
}

tstream_t *lex_file(context_t *ctx, string_t *path) {
    input_t *input = input_from_file(path);
    icache_id_t id = icache_cache(ctx->cache, input);
    istream_t *is = istream_new(input);

    vector_t *tokens = vector_new();
    while (!istream_eos(is)) {
        vector_push(tokens, token(is, id));
    }
    return tstream_new(tokens);
}
