#ifndef _urlfor_routes_h
#define _urlfor_routes_h

#include <ds/debug.h>
#include <ds/bstrlib.h>

typedef void (*Route_log)(bstring entry);

typedef struct Route {
    Route_log cb;
} Route;

/*
 * Write the route data to stdout
 */
void Route_stdout(bstring entry);

#endif
