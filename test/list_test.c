#include "minunit.h"
#include <lcthw/list.h>
#include <assert.h>

static List *list = NULL;
static List *list_b = NULL;
char *test1 = "test1 data";
char *test2 = "test2 data";
char *test3 = "test3 data";

char *test_create() {
    list = List_create();
    mu_assert(list != NULL, "Failed to create the list");

    return NULL;
}

char *test_destroy() {
    List_destroy(list);
    List_destroy(list_b);
    return NULL;
}

char *test_push_pop() {
    List_push(list, test1);
    mu_assert(List_last(list) == test1, "Wrong last value");

    List_push(list, test2);
    mu_assert(List_last(list) == test2, "Wrong last value");

    List_push(list, test3);
    mu_assert(List_last(list) == test3, "Wrong last value");
    mu_assert(List_count(list) == 3, "Wrong count for the list");

    char *val = List_pop(list);
    mu_assert(val == test3, "Wrong value on pop");

    val = List_pop(list);
    mu_assert(val == test2, "Wrong value on pop");

    val = List_pop(list);
    mu_assert(val == test1, "Wrong value on pop");
    mu_assert(List_count(list) == 0, "Wrong count for the list");

    return NULL;
}

char *test_unshift() {
    List_unshift(list, test1);
    mu_assert(List_first(list) == test1, "Wrong first value");

    List_unshift(list, test2);
    mu_assert(List_first(list) == test2, "Wrong first value");

    List_unshift(list, test3);
    mu_assert(List_first(list) == test3, "Wrong first value");
    mu_assert(List_count(list) == 3, "Wrong count for the list");

    return NULL;
}

char *test_remove() {
    // only need to test middle case since edge cases are covered in push/pop test

    char *val = List_remove(list, list->first->next);
    mu_assert(val == test2, "Wrong removed element.");
    mu_assert(List_count(list) == 2, "Wrong count for list");
    mu_assert(List_first(list) == test3, "Wrong first after remove.");
    mu_assert(List_last(list) == test1, "Wrong last after remove.");
    return NULL;
}

char *test_shift() {
    mu_assert(List_count(list) != 0, "Wrong count before shift");

    char *val = List_shift(list);
    mu_assert(val == test3, "Wrong value on shift");

    val = List_shift(list);
    mu_assert(val == test1, "Wrong value on shift");
    mu_assert(List_count(list) == 0, "Wrong count for the list");

    return NULL;
}

char *test_copy() {
    mu_assert(List_count(list) == 0, "List should be empty");

    List_push(list, test1);
    List_push(list, test3);
    List_push(list, test2);

    list_b = List_copy(list);
    mu_assert(list_b != NULL, "List_copy should not fail");

    char *val = List_pop(list);
    char *dest_val = List_pop(list_b);
    mu_assert(val == dest_val, "Copied list should be identical");

    val = List_pop(list);
    dest_val = List_pop(list_b);
    mu_assert(val == dest_val, "Copied list should be identical");

    val = List_pop(list);
    dest_val = List_pop(list_b);
    mu_assert(val == dest_val, "Copied list should be identical");

    // error cases
    // null list
    List *tmp = List_copy(NULL);
    mu_assert(tmp == NULL, "Copying NULL list should have failed");

    return NULL;
}

char *test_join() {
    // setup
    mu_assert(List_count(list) == 0, "List should be empty");
    mu_assert(list != NULL, "list shouldn't be null");
    List_push(list, test1);
    List_push(list, test2);
    List_push(list, test3);

    mu_assert(list_b != NULL, "list_b shouldn't be null");
    List_push(list_b, test3);
    List_push(list_b, test2);
    List_push(list_b, test1);

    // normal cases

    List_join(list, list_b);
    mu_assert(List_count(list) == 6, "Wrong count for joined list");
    mu_assert(List_last(list) == test1, "Wrong value after join");

    List_pop(list);
    List_pop(list);
    List_pop(list);
    List_pop(list);
    List_pop(list);
    List_pop(list);

    // error cases
    // null lists
    List_join(NULL, NULL);

    // cleanup
    return NULL;

}

char *test_split() {

    mu_assert(List_count(list) == 0, "List should be empty");

    List_push(list, test1);
    List_push(list, test2);
    List_push(list, test3);

    return NULL;
}


char *all_tests() {
    mu_suite_start();

    mu_run_test(test_create);
    mu_run_test(test_push_pop);
    mu_run_test(test_unshift);
    mu_run_test(test_remove);
    mu_run_test(test_shift);
    mu_run_test(test_copy);
    mu_run_test(test_join);
    mu_run_test(test_split);
    mu_run_test(test_destroy);

    return NULL;
}

RUN_TESTS(all_tests);
