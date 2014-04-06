#ifndef _ds_dynhandler_h
#define _ds_dynhandler_h

#include <ds/bstrlib.h>
#include <ds/tstree.h>
#include <dlfcn.h>
#include <ds/debug.h>

/*
 * dynhandler - A fully dynamic callback handler
 */

typedef struct Handler {
    bstring mapping;
    bstring cb_lib;
    bstring module_name;
    void *lib;
    void *module;
} Handler;

/*
 * Create a new handler
 */
Handler *DynHandler_create(bstring mapping, bstring cb_lib, bstring module_name);

/*
 * Destroy the handler
 */
int DynHandler_destroy(Handler *h);

#endif
