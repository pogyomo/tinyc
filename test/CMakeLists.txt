add_executable(test-string string.c)
target_link_libraries(test-string tinyc-core)
add_test(NAME test-string COMMAND test-string)

add_executable(test-span span.c)
target_link_libraries(test-span tinyc-core)
add_test(NAME test-span COMMAND test-span)
