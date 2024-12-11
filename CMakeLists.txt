cmake_minimum_required(VERSION 3.21)
project(tinyc
    LANGUAGES C
    DESCRIPTION "Tiny c compiler"
)

enable_testing()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

add_subdirectory(src)
add_subdirectory(test)
