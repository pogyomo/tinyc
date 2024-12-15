add_library(tinyc-core STATIC
    cpp.c
    cpp/context.c
    cpp/define.c
    cpp/helper.c
    cpp/macro.c
    diag.c
    repo.c
    source.c
    span.c
    string.c
    token.c
)
target_include_directories(tinyc-core PUBLIC ../include)
set_target_properties(tinyc-core PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF
)
