#include "minunit.h"
#include <ds/dynhandler.h>
#include <ds/debug.h>
#include <urlfor/routes.h>

Handler *handler = NULL;
TSTree *map = NULL;
struct tagbstring mapping = bsStatic("/test");
struct tagbstring cb_lib = bsStatic("build/libcskelly.so");
struct tagbstring module_name = bsStatic("our_routes");

char *test_create() {
    handler = DynHandler_create(&mapping, &cb_lib, &module_name);
    mu_assert(handler != NULL, "Should not fail to create");

    return NULL;
}

char *test_destroy() {
    int rc = DynHandler_destroy(handler);
    mu_assert(rc == 0, "Should not fail to destroy handler");

    return NULL;
}

char *test_call() {
    bstring test_data = bfromcstr("TEST");
    Route *r = handler->module;
    r->cb(test_data);

    return NULL;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_create);
    mu_run_test(test_call);
    mu_run_test(test_destroy);

    return NULL;
}

RUN_TESTS(all_tests);
