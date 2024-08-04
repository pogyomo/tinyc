cmake_minimum_required(VERSION 3.1)
project(pcc C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
    src/context.c
    src/cpp/context.c
    src/cpp/cpp.c
    src/cpp/stream.c
    src/cpp/token.c
    src/lex/lex.c
    src/lex/token.c
    src/main.c
    src/panic.c
    src/report.c
    src/span.c
    src/string.c
    src/vector.c
)
