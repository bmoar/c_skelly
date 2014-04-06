#ifndef _test_routes_h
#define _test_routes_h

#include <ds/debug.h>

typedef void (*DynTest_dump_cb)();

typedef struct Route {
    DynTest_dump_cb cb;
} Route;

void DynTest_test_cb();

#endif
