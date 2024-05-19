cmake_minimum_required(VERSION 3.1)
project(tinyc C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
    src/cache.c
    src/context.c
    src/main.c
    src/panic.c
    src/report.c
    src/span.c
    src/stream.c
    src/collections/dict.c
    src/collections/string.c
    src/collections/vector.c
    src/lexer/lexer.c
    src/preprocessor/macro.c
    src/preprocessor/preprocessor.c
    src/parser/parser.c
)
