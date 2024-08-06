#include "context.h"
#include "parse/ast/unit.h"
#include "parse/parse.h"

int main() {
    struct context ctx;
    context_init(&ctx);

    struct trans_unit *units;
    if (!parse_file(&ctx, "main.c", &units)) return 1;
}
