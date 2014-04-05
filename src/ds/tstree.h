#ifndef _ds_TSTree_h
#define _ds_TSTree_h

#include <stdlib.h>
#include <ds/darray.h>

typedef struct TSTree {
    char splitchar;
    struct TSTree *low;
    struct TSTree *equal;
    struct TSTree *high;
    void *value;
} TSTree;

void *TSTree_search(TSTree *root, const char *key, size_t len);

void *TSTree_search_prefix(TSTree *root, const char *key, size_t len);

TSTree *TSTree_insert(TSTree *node, const char *key, size_t len, void *value);

typedef void (*TSTree_traverse_cb)(void *value, void *data);

void TSTree_traverse(TSTree *node, TSTree_traverse_cb cb, void *data);

void TSTree_destroy(TSTree *node);

#endif
