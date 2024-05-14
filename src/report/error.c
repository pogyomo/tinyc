#include "error.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stderr, "\e[1m\e[31merror: \e[0m");
    fprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(1);
}
