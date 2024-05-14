cmake_minimum_required(VERSION 3.1)
project(tinyc C)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
    src/context.c
    src/main.c
    src/panic.c
    src/report.c
    src/span.c
    src/collections/string.c
    src/collections/vector.c
    src/input/cache.c
    src/input/input.c
    src/input/stream.c
    src/lexer/lexer.c
    src/lexer/stream.c
    src/lexer/token.c
)
