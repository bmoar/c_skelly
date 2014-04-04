#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include "minunit.h"
#include <ds/ringbuffer.h>

static RingBuffer *buffer = NULL;
const int smallest_buf = 1;
const int medium_buf = 512;
const int large_buf = 65536;

char *test_create_small() {
    buffer = RingBuffer_create(smallest_buf);
    mu_assert(buffer, "Failed to create buffer");

    return NULL;
}

char *test_create_medium() {
    buffer = RingBuffer_create(medium_buf);
    mu_assert(buffer, "Failed to create buffer");

    return NULL;
}

char *test_create_large() {
    buffer = RingBuffer_create(large_buf);
    mu_assert(buffer, "Failed to create buffer");

    return NULL;
}

char *test_destroy() {
    mu_assert(buffer != NULL, "Should not have destroyed buffer");
    RingBuffer_destroy(buffer);

    return NULL;
}

char *test_errors() {
    bstring too_big = bfromcstr("You can't handle a bstr of this magnitude!");
    int rc = 0;
    char *val = NULL;

    RingBuffer *not_exist = RingBuffer_create(-1);
    mu_assert(not_exist == NULL, "Shouldn't be able to create a negative sized buffer");
    not_exist = RingBuffer_create(-1);
    mu_assert(not_exist == NULL, "Shouldn't be able to create a negative sized buffer");

    rc = RingBuffer_write(buffer, bdata(too_big), blength(too_big));
    mu_assert(rc == -1, "Should not be able to write over max buffer length");

    rc = RingBuffer_read(buffer, val, 2000);
    mu_assert(rc == -1, "Should not be able to read over max buffer length");

    bstring temp = RingBuffer_gets(buffer, 2000);
    mu_assert(temp == NULL, "Should not be able to get over max buffer length");

    bdestroy(too_big);
    return NULL;
}

char *test_read_write() {
    bstring data[100] = {NULL};
    bstring gets = NULL;
    srand((unsigned int)time(NULL));
    int rc = 0;
    int i = 0;

    for (i = 0; i < 100; i++) {
        int num = rand();
        data[i] = bformat("%d", num);
        rc = RingBuffer_write(buffer, bdata(data[i]), blength(data[i]));
        mu_assert(rc == blength(data[i]), "wrote the wrong size to RingBuffer");

        gets = RingBuffer_gets(buffer, blength(data[i]));
        mu_assert(bstrcmp(gets, data[i]) == 0, "Data did not match");

        bdestroy(data[i]);
    }


    return NULL;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_create_small);
    mu_run_test(test_errors);
    mu_run_test(test_destroy);

    mu_run_test(test_create_medium);
    mu_run_test(test_read_write);
    mu_run_test(test_destroy);

    mu_run_test(test_create_large);
    mu_run_test(test_read_write);
    mu_run_test(test_destroy);

    return NULL;
}

RUN_TESTS(all_tests);
