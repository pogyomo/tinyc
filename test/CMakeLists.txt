add_executable(test-string string.c)
target_link_libraries(test-string tinyc-core)
add_test(NAME test-string COMMAND test-string)
