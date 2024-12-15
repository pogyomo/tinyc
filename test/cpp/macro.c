#include <assert.h>
#include <tinyc/cpp/macro.h>

void func_args_at(void) {
    struct tinyc_cpp_macro_func_param *p0, *p1, *ps;
    p0 = tinyc_cpp_macro_func_param_create("arg0", 0);
    p1 = tinyc_cpp_macro_func_param_create("arg1", 1);
    p0->next = p1;
    ps = p0;

    assert(tinyc_cpp_macro_func_params_at(ps, 0) == p0);
    assert(tinyc_cpp_macro_func_params_at(ps, 1) == p1);
    assert(tinyc_cpp_macro_func_params_at(ps, 2) == NULL);
}

int main(void) {
    func_args_at();
}
