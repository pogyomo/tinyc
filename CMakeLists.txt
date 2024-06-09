cmake_minimum_required(VERSION 3.1)
project(tinyc C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
    src/cache.c
    src/collections/dict.c
    src/collections/string.c
    src/collections/vector.c
    src/context.c
    src/lexer/lexer.c
    src/lexer/token.c
    src/main.c
    src/memory.c
    src/panic.c
    src/parser/ast.c
    src/parser/parser.c
    src/parser/parser_decl.c
    src/parser/parser_expr.c
    src/parser/parser_stmt.c
    src/parser/utils.c
    src/preprocessor/macro.c
    src/preprocessor/preprocessor.c
    src/report.c
    src/span.c
    src/stream.c
)
