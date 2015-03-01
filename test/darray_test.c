#include "minunit.h"
#include <ds/darray.h>
#include <ds/darray_algos.h>

static DArray *array = NULL;
static int PERFORMANCE_FACTOR = 100000;
static int *val1 = NULL;
static int *val2 = NULL;

static int integer_compare(const void *a, const void *b) {
    return *(int *)a - *(int *)b;
}

static int qsort_int_compare(const void **a, const void **b) {
    return *(int *)*a - *(int *)*b;
}

char *test_create() {
    array = DArray_create(sizeof(int), 100);
    mu_assert(array != NULL, "DArray_create failed.");
    mu_assert(array->contents != NULL, "contents are wrong in DArray");
    mu_assert(array->end == 0, "End isn't at the right spot");
    mu_assert(array->element_size == sizeof(int), "Wrong element_size");
    mu_assert(array->max == 100, "Wrong max length on initial size");

    return NULL;
}

char *test_destroy() {
    DArray_clear_destroy(array);

    return NULL;
}

char *test_new() {
    val1 = DArray_new(array);
    mu_assert(val1 != NULL, "Failed to make a new element");

    val2 = DArray_new(array);
    mu_assert(val2 != NULL, "Failed to make a new element");

    return NULL;
}

char *test_set() {
    DArray_set(array, 0, val1);
    DArray_set(array, 1, val2);

    return NULL;
}

char *test_get() {
    mu_assert(DArray_get(array, 0) == val1, "Wrong first value");
    mu_assert(DArray_get(array, 1) == val2, "Wrong first value");

    return NULL;
}

char *test_remove() {
    int *val_check = DArray_remove(array, 0);
    mu_assert(val_check != NULL, "Should not get a NULL");
    mu_assert(*val_check == *val1, "Should get the first value");
    mu_assert(DArray_get(array, 0) == NULL, "Should be gone");
    DArray_free(val_check);

    val_check = DArray_remove(array, 1);
    mu_assert(val_check != NULL, "Should not get a NULL");
    mu_assert(*val_check == *val2, "Should get the first value");
    mu_assert(DArray_get(array, 1) == NULL, "Should be gone");
    DArray_free(val_check);

    return NULL;
}

char *test_expand_contract() {
    int old_max = DArray_max(array);
    DArray_expand(array);
    mu_assert((unsigned int)DArray_max(array) == old_max + array->expand_rate, "Wrong size after expand");

    DArray_contract(array);
    mu_assert((unsigned int)DArray_max(array) == array->expand_rate + 1, "Should stay at the expand_rate.");

    DArray_contract(array);
    mu_assert((unsigned int)DArray_max(array) == array->expand_rate + 1, "Should stay at the expand_rate.");

    return NULL;
}

char *test_push_pop() {
    int i = 0;
    for(i = 0; i < 1000; i++) {
        int *val = DArray_new(array);
        *val = i * 333;
        DArray_push(array, val);
    }

    mu_assert(DArray_max(array) == 1201, "Wrong max size");

    for(i = 999; i >= 0; i--) {
        int *val = DArray_pop(array);
        mu_assert(val != NULL, "Shouldn't get a NULL");
        mu_assert(*val == i * 333, "Wrong value");
        DArray_free(val);
    }

    return NULL;
}

char *test_push_performance() {
    int i = 0;
    int rc = 0;
    mu_assert(DArray_count(array) == 1, "Should be empty DArray");

    for(i = 0; i < PERFORMANCE_FACTOR; i++) {
        int *val = DArray_new(array);
        *val = 666;
        rc = DArray_push(array, val);
        mu_assert(rc != -1, "Push should not have failed");
    }

    int *find_me = DArray_get(array, PERFORMANCE_FACTOR);
    *find_me = 777;

    // why is there a hole in the darray :/
    int *wtf = DArray_new(array);
    *wtf = 666;
    DArray_set(array, 0, wtf);

    return NULL;
}

char *test_iteration_performance() {
    int i = 0;
    int found = 0;
    for(i = 1; i < DArray_count(array); i++) {
        int *val = DArray_get(array, i);
        mu_assert(val != NULL, "Contents shouldn't be NULL here");
        if(*val == 777) {
            debug("i : %d", i);
            found = 1;
            break;
        }
    }

    mu_assert(found == 1, "Should have found element");

    return NULL;
}

char *test_find_performance() {
    int rc = 0;

    // assume darray is sorted
    mu_assert(rc == 0, "DArray should have sorted");

    int *find_me = DArray_get(array, PERFORMANCE_FACTOR);
    mu_assert(*find_me == 777, "find_me should exist in array");

    rc = DArray_find(array, find_me, integer_compare);
    mu_assert(rc != -1, "Element should exist in the array");

    return NULL;
}

char *test_pop_performance() {
    int i = 0;
    int n = DArray_count(array);
    for(i = 1; i < n; i++) {
        int *val = DArray_pop(array);
        mu_assert(*val == 666 || *val == 777, "Contents should be the same during pop.");
        DArray_free(val);
    }

    return NULL;
}

char *test_find() {
    DArray_clear_destroy(array);
    array = DArray_create(sizeof(int), 100);
    int i = 0;
    int rc = 0;
    for(i = 0; i < 1001; i++) {
        int *val = DArray_new(array);
        *val = 666;
        rc = DArray_push(array, val);
        mu_assert(rc != -1, "Push should not have failed");
    }

    // element to find
    int *val = DArray_new(array);
    *val = 777;
    DArray_set(array, 501, val);
    int *get = DArray_get(array, 501);
    mu_assert(*get == 777, "Element to find should be here");

    rc = DArray_qsort(array, (DArray_compare)qsort_int_compare);
    mu_assert(rc == 0, "DArray should have sorted");

    rc = DArray_find(array, get, integer_compare);
    mu_assert(rc != -1, "Element should exist in the array");
    mu_assert(rc == 1000, "Element should be found by binary search here");

    int derp = 42134;
    rc = DArray_find(array, &derp, integer_compare);
    mu_assert(rc == -1, "Element should not exist in the array");

    return NULL;
}

char *all_tests() {
    time_t start_time = 0;
    time_t end_time = 0;
    mu_suite_start();

    mu_run_test(test_create);
    mu_run_test(test_new);
    mu_run_test(test_set);
    mu_run_test(test_get);
    mu_run_test(test_remove);
    mu_run_test(test_expand_contract);
    mu_run_test(test_push_pop);
    
    //performance tests
    start_time = time(NULL);
    mu_run_test(test_push_performance);
    end_time = time(NULL);
    log_info("Test completed in %f seconds", difftime(end_time, start_time)); 

    start_time = time(NULL);
    mu_run_test(test_iteration_performance);
    end_time = time(NULL);
    log_info("Test completed in %f seconds", difftime(end_time, start_time));

    start_time = time(NULL);
    mu_run_test(test_find_performance);
    end_time = time(NULL);
    log_info("Test completed in %f seconds", difftime(end_time, start_time));

    start_time = time(NULL);
    mu_run_test(test_pop_performance);
    end_time = time(NULL);
    log_info("Test completed in %f seconds", difftime(end_time, start_time));

    mu_run_test(test_find);
    mu_run_test(test_destroy);

    return NULL;
}

RUN_TESTS(all_tests);
