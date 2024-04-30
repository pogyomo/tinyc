cmake_minimum_required(VERSION 3.1)
project(tinyc CXX)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
    src/main.cpp
    src/input/cache.cpp
    src/input/input.cpp
    src/input/stream.cpp
    src/lexer/lexer.cpp
    src/lexer/stream.cpp
    src/lexer/token.cpp
    src/parser/expr.cpp
    src/report/report.cpp
)
