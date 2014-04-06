#include <ds/dynhandler.h>
#include <stdio.h>
#include <dlfcn.h>

static inline int DynHandler_link_handler(Handler *h) {
    h->lib = dlopen(bdata(h->cb_lib), RTLD_NOW);
    check(h->lib != NULL, "Failed to open library %s: %s", bdata(h->cb_lib), dlerror());

    h->module = dlsym(h->lib, bdata(h->module_name));
    check(h->module != NULL, "Failed to find: %s in library %s: %s",
            bdata(h->module_name), bdata(h->cb_lib), dlerror());

    return 0;

error:
    return 1;
}

Handler *DynHandler_create(bstring mapping, bstring cb_lib, bstring module_name) {
    Handler *h = malloc(sizeof(Handler));
    h->mapping = mapping;
    h->cb_lib = cb_lib;
    h->module_name = module_name;

    int rc = DynHandler_link_handler(h);
    check(rc == 0, "Failed to link handler");

    return h;

error:
    if (h) {
        free(h);
    }
    return NULL;
}

int DynHandler_destroy(Handler *h) {
    check(h, "Tried to destroy a NULL Handler");

    int rc = dlclose(h->lib);
    check(rc == 0, "Failed to close %s", bdata(h->cb_lib));
    free(h);

    return 0;

error:
    return 1;
}
