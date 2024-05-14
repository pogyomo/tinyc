#include "panic.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void panic(const char* format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stderr, "\e[1m\e[31mfatal error: \e[0m");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(1);
}

void panic_internal(const char* format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stderr, "\e[1m\e[31minternal compiler error: \e[0m");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(1);
}
