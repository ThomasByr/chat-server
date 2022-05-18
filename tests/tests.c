#include "dict.h"

#include "m.h"

unsigned long _no_asserts = 0;

/* arbitrary large structure in memory */

struct foo_s {
    char *id;
    int a;
    int b;
};
typedef struct foo_s foo_t;

foo_t *foo_new(int id) {
    foo_t *foo = (foo_t *)malloc(sizeof(foo_t));
    foo->id = (char *)malloc(sizeof(char) * 1000000);
    foo->a = 0;
    foo->b = 0;
    snprintf_s(foo->id, sizeof(char) * 1000000, "%d", id);

    return foo;
}

void foo_free(foo_t *foo) {
    free(foo->id);
    free(foo);
}

void foo_free_void(void *foo) {
    foo_t *f = (foo_t *)foo;
    foo_free(f);
}

void *foo_cmp_a_void(void *foo, void *a) {
    foo_t *f = (foo_t *)foo;
    int *i = (int *)a;
    assert_eq(f->a, *i);
    return NULL;
}

/* dictionary tests */

void dict_test_0(void) {
    dict_t *dict = dict_new();
    dict_push(dict, (void *)"foo1", (void *)"bar");       // foo1 -> bar
    dict_push(dict, (void *)"foo2", (void *)"baz");       // foo2 -> baz
    assert_eq(dict_nitems(dict), 2);                      // 2 items
    char *value = (char *)dict_get(dict, (void *)"foo1"); // get foo1 value
    assert_eq(strcmp(value, "bar"), 0);                   // compare value
    value = (char *)dict_get(dict, (void *)"foo2");       // get foo2 value
    assert_eq(strcmp(value, "baz"), 0);                   // compare value
    dict_free(dict);
}

void dict_test_1(void) {
    dict_t *dict = dict_new();
    dict_itr_t *itr = dict_itr_new(dict);
    int n = 1000;
    for (int i = 0; i < n; i++) {
        foo_t *foo = foo_new(i);           // make a random large object
        foo->a = 42;                       // set a to 42
        int x = dict_push(dict, foo, foo); // push foo with foo as key
        assert_eq(x, 1);                   // we pushed 1 item
    }
    assert_eq(dict_nitems(dict), n); // we pushed n items in total

    size_t count = 0;
    while (dict_itr_has_next(itr)) {
        count++;                                   // count each item
        foo_t *foo = (foo_t *)dict_itr_value(itr); // iterate over each item
        assert_eq(foo->a, 42);                     // a is 42
        foo->a = 1;                                // set a to 1 (for next test)
        dict_push(dict, foo, foo);                 // push foo with foo as key
        dict_itr_next(itr);
    }
    assert_eq(count, n);             // we iterated over n items
    assert_eq(dict_nitems(dict), n); // we did not lose or add any items
    dict_itr_reset(itr);             // reset the iterator

    while (dict_itr_has_next(itr)) {
        foo_t *foo = (foo_t *)dict_itr_value(itr); // iterate over each item
        foo_free(foo);                             // free each item
        dict_itr_next(itr);
    }
    dict_itr_free(itr);
    dict_free(dict);
}

void dict_test_2(void) {
    dict_t *dict = dict_new();
    char *str0 = "foo0";
    char *str1 = "foo1";

    dict_push(dict, (void *)str0, (void *)str0); // foo0 -> foo0
    dict_push(dict, (void *)str1, (void *)str1); // foo1 -> foo1
    assert_eq(dict_nitems(dict), 2);             // 2 items

    char *value = (char *)dict_get(dict, (void *)str0); // get foo0 value
    assert_eq(strcmp(value, str0), 0);                  // compare value

    value = (char *)dict_get(dict, (void *)str1); // get foo1 value
    assert_eq(strcmp(value, str1), 0);            // compare value

    dict_discard(dict, (void *)str0); // discard foo0
    assert_eq(dict_nitems(dict), 1);  // 1 item left

    dict_push(dict, (void *)str0, (void *)str0); // foo0 -> foo0
    assert_eq(dict_nitems(dict), 2);             // 2 items

    dict_free(dict);
}

void dict_test_3(void) {
    dict_t *dict = dict_new();
    dict_itr_t *itr = dict_itr_new(dict);
    int n = 1000;
    for (int i = 0; i < n; i++) {
        foo_t *foo1 = foo_new(i);            // make a random large object
        foo_t *foo2 = foo_new(i);            // make a random large object
        foo1->a = 42;                        // set a of foo1 to 42
        foo2->a = 24;                        // set a of foo2 to 24
        int x = dict_push(dict, foo1, foo2); // foo1 -> foo2
        assert_eq(x, 1);                     // we pushed 1 item
    }
    assert_eq(dict_nitems(dict), n); // we pushed n items in total

    int a = 42;       // a is 42
    void *data1 = &a; // data1 is a pointer to a
    int b = 24;       // b is 24
    void *data2 = &b; // data2 is a pointer to b

    // iterate over each item using callback
    dict_itr_for_each(itr, foo_cmp_a_void, data1, foo_cmp_a_void, data2);
    // free keys and values
    dict_itr_discard_all(itr, foo_free_void, foo_free_void);
    dict_itr_free(itr);
    dict_free(dict);
}

void dict_test_4(void) {
    dict_t *dict1 = dict_new();
    dict_t *dict2 = dict_new();
    dict_itr_t *itr = dict_itr_new(dict1);
    int n = 1000;
    for (int i = 0; i < n; i++) {
        foo_t *foo1 = foo_new(i);             // make a random large object
        foo_t *foo2 = foo_new(i);             // make a random large object
        foo1->a = 42;                         // set a of foo1 to 42
        foo2->a = 24;                         // set a of foo2 to 24
        int x = dict_push(dict1, foo1, foo2); // foo1 -> foo2
        assert_eq(x, 1);                      // we pushed 1 item
        x = dict_push(dict2, foo1, foo2);     // foo1 -> foo2
        assert_eq(x, 1);                      // we pushed 1 item
    }
    assert_eq(dict_nitems(dict1), n); // we pushed n items in total
    assert_eq(dict_nitems(dict2), n); // we pushed n items in total

    size_t rv = dict_merge(dict1, dict2); // merge dict1 and dict2 into dict1
    assert_eq(rv, 0);                     // no items were added
    assert_eq(dict_nitems(dict1), n);     // dict1 has n items
    assert_eq(dict_nitems(dict2), n);     // dict2 has n items

    dict_itr_discard_all(itr, foo_free_void, foo_free_void);
    dict_itr_free(itr);
    dict_free(dict1);
    dict_free(dict2);
}

void dict_test_5(void) {
    dict_t *dict1 = dict_new();
    dict_t *dict2 = dict_new();
    dict_itr_t *itr = dict_itr_new(dict1);
    int n = 1000;
    for (int i = 0; i < n; i++) {
        foo_t *foo1 = foo_new(i);             // make a random large object
        foo_t *foo2 = foo_new(i);             // make a random large object
        foo1->a = 42;                         // set a of foo1 to 42
        foo2->a = 24;                         // set a of foo2 to 24
        int x = dict_push(dict1, foo1, foo1); // foo1 -> foo1
        assert_eq(x, 1);                      // we pushed 1 item
        x = dict_push(dict2, foo2, foo2);     // foo2 -> foo2
        assert_eq(x, 1);
    }
    assert_eq(dict_nitems(dict1), n); // we pushed n items in total
    assert_eq(dict_nitems(dict2), n); // we pushed n items in total

    size_t rv = dict_merge(dict1, dict2); // merge dict1 and dict2 into dict1
    assert_eq(rv, n);                     // n items were added
    assert_eq(dict_nitems(dict1), 2 * n); // dict1 has 2n items
    assert_eq(dict_nitems(dict2), n);     // dict2 has n items

    dict_itr_discard_all(itr, foo_free_void, NULL);
    dict_itr_free(itr);
    dict_free(dict1);
    dict_free(dict2);
}

void dict_test_6(void) {
    dict_t *dict1 = dict_new();
    dict_itr_t *itr = dict_itr_new(dict1);
    int n = 1000;
    for (int i = 0; i < n; i++) {
        foo_t *foo = foo_new(i);            // make a random large object
        foo->a = 42;                        // set a to 42
        int x = dict_push(dict1, foo, foo); // foo -> foo
        assert_eq(x, 1);                    // we pushed 1 item
    }
    assert_eq(dict_nitems(dict1), n); // we pushed n items in total

    dict_t *dict2 = dict_copy(dict1); // copy dict1 into dict2
    assert_eq(dict_nitems(dict2), n); // dict2 has n items

    for (int i = n; i < 2 * n; i++) {
        foo_t *foo = foo_new(i);            // make a random large object
        foo->a = 42;                        // set a to 42
        int x = dict_push(dict1, foo, foo); // foo -> foo
        assert_eq(x, 1);                    // we pushed 1 item
    }
    assert_eq(dict_nitems(dict1), 2 * n); // we pushed n new items in total
    assert_eq(dict_nitems(dict2), n);     // dict2 still has n items

    dict_itr_discard_all(itr, foo_free_void, NULL);
    dict_itr_free(itr);
    dict_free(dict1);
    dict_free(dict2);
}

void dict_test_7(void) {
    dict_t *dict1 = dict_new();
    dict_t *dict2 = dict_new();
    dict_itr_t *itr = dict_itr_new(dict1);
    int n = 1000;
    for (int i = 0; i < n; i++) {
        foo_t *foo1 = foo_new(i);             // make a random large object
        foo_t *foo2 = foo_new(i);             // make a random large object
        foo1->a = 42;                         // set a of foo1 to 42
        foo2->a = 24;                         // set a of foo2 to 24
        int x = dict_push(dict1, foo1, foo1); // foo1 -> foo1
        assert_eq(x, 1);                      // we pushed 1 item
        x = dict_push(dict2, foo1, foo1);     // foo1 -> foo1
        assert_eq(x, 1);                      // we pushed 1 item
        x = dict_push(dict2, foo2, foo2);     // foo2 -> foo2
        assert_eq(x, 1);                      // we pushed 1 item
    }
    assert_eq(dict_nitems(dict1), n);     // we pushed n items in total
    assert_eq(dict_nitems(dict2), 2 * n); // we pushed 2n items in total

    size_t rv = dict_merge(dict1, dict2); // merge dict1 and dict2 into dict1
    assert_eq(rv, n);                     // n items were added
    assert_eq(dict_nitems(dict1), 2 * n); // dict1 has 2n items
    assert_eq(dict_nitems(dict2), 2 * n); // dict2 has 2n items

    dict_itr_discard_all(itr, foo_free_void, NULL);
    dict_itr_free(itr);
    dict_free(dict1);
    dict_free(dict2);
}

void dict_test_8(void) {
    dict_t *dict0 = dict_new();
    dict_t *dict1 = dict_new(1); // dict that hashes content instead of pointers
    dict_itr_t *itr = dict_itr_new(dict0);
    int n = 1000;

    char *str = "foo";            // a string that will be used as a key
    char *str1 = strdup(str);     // a copy of str
    dict_push(dict0, str1, str1); // str -> str
    dict_push(dict1, str1, str1); // str -> str

    for (int i = 0; i < n; i++) {
        str1 = strdup(str); // make a copy of str

        size_t s0 = dict_get(dict0, str1);      // get str from dict0
        size_t s1 = dict_get(dict1, str1);      // get str from dict1
        assert_eq(s0, 0);                       // str is not in dict0
        assert_gt(s1, 0);                       // str is already in dict1
        int rv0 = dict_push(dict0, str1, str1); // str -> str
        int rv1 = dict_push(dict1, str1, str1); // str -> str
        assert_eq(rv0, 1);                      // we pushed 1 item
        assert_eq(rv1, 0);                      // str content already in dict1
        s0 = dict_get(dict0, str1);             // get str from dict0
        s1 = dict_get(dict1, str1);             // get str from dict1
        assert_gt(s0, 0);                       // str is in dict0
        assert_gt(s1, 0);                       // str is in dict1
    }

    assert_eq(dict_nitems(dict0), n + 1); // we pushed n+1 items in total
    assert_eq(dict_nitems(dict1), 1);     // dict1 has 1 item

    dict_itr_discard_all(itr, free, NULL);
    dict_itr_free(itr);
    dict_free(dict0);
    dict_free(dict1);
}

void dict_test_9(void) {
    dict_t *dict = dict_new();
    int n = 1000;

    char *str = "foo"; // a string that will be used as a key
    for (int i = 0; i < n; i++) {
        char *item = strdup(str);              // make a copy of str
        int rv0 = dict_get(dict, item);        // get str from dict
        assert_eq(rv0, 0);                     // str is not in dict
        int rv1 = dict_push(dict, item, item); // str -> str
        assert_eq(rv1, 1);                     // we pushed a new item
        int rv2 = dict_get(dict, item);        // get str from dict
        assert_gt(rv2, 0);                     // str is in dict
        int rv3 = dict_discard(dict, item);    // discard the item
        assert_eq(rv3, 1);                     // we discarded the item
        int rv4 = dict_get(dict, item);        // get str from dict
        assert_eq(rv4, 0);                     // str is not in dict
        int rv5 = dict_discard(dict, item);    // discard str from dict
        assert_eq(rv5, 0);                     // str is not in dict
        free(item);                            // free the copy of str
    }
    assert_eq(dict_nitems(dict), 0); // dict is empty
    dict_free(dict);
}

void dict_test(void) {
    test_case(dict_test_0);
    test_case(dict_test_1);
    test_case(dict_test_2);
    test_case(dict_test_3);
    test_case(dict_test_4);
    test_case(dict_test_5);
    test_case(dict_test_6);
    test_case(dict_test_7);
    test_case(dict_test_8);
    test_case(dict_test_9);
}

/* main */

int main(void) { dict_test(); }
