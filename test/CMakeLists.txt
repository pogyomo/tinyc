add_executable(test-string string.c)
target_link_libraries(test-string tinyc-core)
add_test(NAME test-string COMMAND test-string)

add_executable(test-span span.c)
target_link_libraries(test-span tinyc-core)
add_test(NAME test-span COMMAND test-span)

add_executable(test-token token.c)
target_link_libraries(test-token tinyc-core)
add_test(NAME test-token COMMAND test-token)

add_executable(test-repo repo.c)
target_link_libraries(test-repo tinyc-core)
add_test(NAME test-repo COMMAND test-repo)

add_executable(test-source source.c)
target_link_libraries(test-source tinyc-core)
add_test(NAME test-source COMMAND test-source)

add_executable(test-cpp-define cpp/define.c)
target_link_libraries(test-cpp-define tinyc-core)
add_test(NAME test-cpp-define COMMAND test-cpp-define)

add_executable(test-cpp-macro cpp/macro.c)
target_link_libraries(test-cpp-macro tinyc-core)
add_test(NAME test-cpp-macro COMMAND test-cpp-macro)
