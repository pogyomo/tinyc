cmake_minimum_required(VERSION 3.1)
project(tinyc CXX)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
    src/main.cpp
    src/lexer/token.cpp
    src/lexer/lexer.cpp
    src/parser/expr.cpp
    src/parser/parser.cpp
)
