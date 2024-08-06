#include "panic.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void fatal_error(const char *fmt, ...) {
    fputs("\e[31mfatal error: \e[0m", stderr);

    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    fputs("\n", stderr);

    exit(EXIT_FAILURE);
}
