#include <test/routes.h>

struct Route our_routes = {
    .cb = DynTest_test_cb,
};

void DynTest_test_cb() {
    debug("Called the handler callback");
}
