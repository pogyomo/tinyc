#include <assert.h>
#include <tinyc/span.h>

static void test_add_cross(void) {
    struct tinyc_span s1 = {
        {1, 10},
        {3, 5 }
    };
    struct tinyc_span s2 = {
        {2, 8},
        {4, 5}
    };
    struct tinyc_span s = tinyc_span_add(s1, s2);

    assert(s.start.row == 1);
    assert(s.start.offset == 10);
    assert(s.end.row == 4);
    assert(s.end.offset == 5);
}

static void test_add_surround(void) {
    struct tinyc_span s1 = {
        {1, 10},
        {4, 8 }
    };
    struct tinyc_span s2 = {
        {2, 8},
        {3, 5}
    };
    struct tinyc_span s = tinyc_span_add(s1, s2);

    assert(s.start.row == s1.start.row);
    assert(s.start.offset == s1.start.offset);
    assert(s.end.row == s1.end.row);
    assert(s.end.offset == s1.end.offset);
}

static void test_add_same(void) {
    struct tinyc_span s1 = {
        {1, 10},
        {4, 8 }
    };
    struct tinyc_span s = tinyc_span_add(s1, s1);

    assert(s.start.row == s1.start.row);
    assert(s.start.offset == s1.start.offset);
    assert(s.end.row == s1.end.row);
    assert(s.end.offset == s1.end.offset);
}

int main(void) {
    test_add_cross();
    test_add_surround();
    test_add_same();
}
