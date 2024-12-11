add_library(tinyc-core STATIC
    span.c
    string.c
    token.c
)
target_include_directories(tinyc-core PUBLIC ../include)
