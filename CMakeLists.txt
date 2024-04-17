cmake_minimum_required(VERSION 3.1)
project(tinyc CXX)

file(GLOB SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})
