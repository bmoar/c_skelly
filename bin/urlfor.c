#include <ds/tstree.h>
#include <ds/bstrlib.h>
#include <urlfor/routes.h>
#include <ds/dynhandler.h>

TSTree *add_route_data(TSTree *routes, bstring line) {
    struct bstrList *data = bsplit(line, ' ');
    check(data->qty == 2, "Line '%s' does not have two columns",
            bdata(line));

    bstring cb_lib = bfromcstr("build/libcskelly.so");
    bstring module_name = bfromcstr("our_routes");

    Handler *h = DynHandler_create(data->entry[0], cb_lib, module_name);
    h->data = bstrcpy(data->entry[1]);

    routes = TSTree_insert(routes, bdata(data->entry[0]), blength(data->entry[0]), h);

    bstrListDestroy(data);

    return routes;

error:
    return NULL;
}

TSTree *load_routes(const char *file) {
    TSTree *routes = NULL;
    bstring line = NULL;
    FILE *routes_map = NULL;

    routes_map = fopen(file, "r");
    check(routes_map != NULL, "Failed to open routes: %s", file);

    while((line = bgets((bNgetc)fgetc, routes_map, '\n')) != NULL) {
        check(btrimws(line) == BSTR_OK, "Failed to trim line.");
        routes = add_route_data(routes, line);
        check(routes != NULL, "Failed to add route");
        bdestroy(line);
    }

    fclose(routes_map);
    return routes;

error:
    if(routes_map) {
        fclose(routes_map);
    }

    if(line) {
        bdestroy(line);
    }

    return NULL;
}

Handler *match_url(TSTree *routes, bstring url) {
    Handler *route = TSTree_search(routes, bdata(url), blength(url));

    if(route == NULL) {
        printf("No exact match found, trying prefix.\n");
        route = TSTree_search_prefix(routes, bdata(url), blength(url));
    }

    return route;
}

bstring read_line(const char *prompt) {
    printf("%s", prompt);

    bstring result = bgets((bNgetc)fgetc, stdin, '\n');
    check_debug(result != NULL, "stdin closed.");

    check(btrimws(result) == BSTR_OK, "Failed to trim");

    return result;

error:
    return NULL;
}

void bdestroy_cb(void *value, void *ignored) {
    (void)ignored;
    Handler *h = value;
    bdestroy(h->data);
    DynHandler_destroy(h);
}

void destroy_routes(TSTree *routes) {
    TSTree_traverse(routes, bdestroy_cb, NULL);
    TSTree_destroy(routes);
}

int main(int argc, const char *argv[])
{
    bstring url = NULL;
    TSTree *routes = NULL;
    check(argc == 2, "USAGE: urlfor <urlfile>");

    routes = load_routes(argv[1]);
    check(routes != NULL, "Your route file has an error");

    while(1) {
        url = read_line("URL> ");
        check_debug(url != NULL, "goodbye.");

        Handler *h = match_url(routes, url);

        if(h) {
            Route *r = h->module;
            r->cb(h->data);
        } else {
            printf("FAIL: %s\n", bdata(url));
        }

        bdestroy(url);
    }

    destroy_routes(routes);
    return 0;

error:
    destroy_routes(routes);
    return 1;
}
