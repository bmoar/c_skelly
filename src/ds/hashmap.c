#undef NDEBUG
#include <stdint.h>
#include <ds/hashmap.h>
#include <ds/debug.h>
#include <ds/bstrlib.h>

static int default_compare(void *a, void *b) {
    return bstrcmp((bstring)a, (bstring)b);
}

static int qsort_default_compare(void **a, void **b) {
    // a and b are pointers to HashMapNode pointers
    HashmapNode *node_a = (HashmapNode *)*a;
    HashmapNode *node_b = (HashmapNode *)*b;
    bstring key_a = (bstring)node_a->key;
    bstring key_b = (bstring)node_b->key;
    return bstrcmp(key_a, key_b);
}

static int bucket_compare(void *a, void *b) {
    // a is a bstring, b is a HashmapNode
    HashmapNode *node_b = (HashmapNode *)b;
    bstring key_a = (bstring)a;
    bstring key_b = (bstring)node_b->key;

    return bstrcmp(key_a, key_b);
}

Hashmap *Hashmap_create(Hashmap_compare compare, Hashmap_hash hash, int buckets) {
    Hashmap *map = calloc(1, sizeof(Hashmap));
    check_mem(map);

    size_t num_buckets = buckets <= 0 ? DEFAULT_NUMBER_OF_BUCKETS : buckets;

    map->compare = compare == NULL ? default_compare : compare;
    map->hash = hash == NULL ? SuperFashHash : hash;
    map->buckets = DArray_create(sizeof(DArray *), num_buckets);
    map->buckets->end = map->buckets->max; // fake out expanding it
    check_mem(map->buckets);

    return map;

error:
    if(map) {
        Hashmap_destroy(map);
    }

    return NULL;
}

void Hashmap_destroy(Hashmap *map) {
    int i = 0;
    int j = 0;

    if(map) {
        if(map->buckets) {
            for(i = 0; i < DArray_count(map->buckets); i++) {
                DArray *bucket = DArray_get(map->buckets, i);
                if(bucket) {
                    for(j = 0; j < DArray_count(bucket); j++) {
                        free(DArray_get(bucket, j));
                    }
                    DArray_destroy(bucket);
                }
            }
            DArray_destroy(map->buckets);
        }
        free(map);
    }
}

static inline HashmapNode *HashmapNode_node_create(int hash, void *key, void *data) {
    HashmapNode *node = calloc(1, sizeof(HashmapNode));
    check_mem(node);

    node->hash = hash;
    node->key = key;
    node->data = data;

    return node;

error:
    return NULL;
}

static inline DArray *Hashmap_find_bucket(Hashmap *map, void *key, int create, uint32_t *hash_out) {
    uint32_t hash = map->hash(key);
    int bucket_n = hash % DEFAULT_NUMBER_OF_BUCKETS;
    check(bucket_n >= 0, "Invalid bucket found: %d", bucket_n);
    *hash_out = hash; // store for the caller

    DArray *bucket = DArray_get(map->buckets, bucket_n);

    if(!bucket && create) {
        // make a new bucket
        bucket = DArray_create(sizeof(void *), DEFAULT_NUMBER_OF_BUCKETS);
        check_mem(bucket);
        DArray_set(map->buckets, bucket_n, bucket);
    }

    return bucket;

error:
    return NULL;
}

static inline int Hashmap_get_node(DArray *bucket, void *key) {
    int rc = 0;

    rc = DArray_find(bucket, key, (DArray_compare)bucket_compare);
    return rc;
}

int Hashmap_set(Hashmap *map, void *key, void *data) {
    check(map, "Can't set in a NULL hashmap");
    check(key, "Can't set a NULL key in hashmap");
    if(data == NULL) {
        log_warn("Setting data for key %s to NULL", bdata((bstring)key));
    }

    uint32_t hash = 0;
    DArray *bucket = Hashmap_find_bucket(map, key, 1, &hash);
    check(bucket, "Error can't create bucket");

    // check if key already exists in Hashmap
    int rc = Hashmap_get_node(bucket, key);

    if(rc >= 0) {
        // key already in hashmap
        HashmapNode *node = DArray_get(bucket, rc);
        node->data = data;
        node->hash = hash;
    } else {
        HashmapNode *node = HashmapNode_node_create(hash, key, data);
        check_mem(node);
        DArray_push(bucket, node);
        qsort(bucket->contents, DArray_count(bucket), sizeof(void *), (DArray_compare)qsort_default_compare);
    }


    return 0;

error:
    return -1;
}

void *Hashmap_get(Hashmap *map, void *key) {
    check(map, "Can't get from a NULL map");
    check(key, "Can't get a NULL key from map");

    uint32_t hash = 0;
    DArray *bucket = Hashmap_find_bucket(map, key, 0, &hash);
    if(!bucket) {
        return NULL;
    }

    int i = Hashmap_get_node(bucket, key);
    if(i < 0) {
        return NULL;
    }

    HashmapNode *node = DArray_get(bucket, i);
    check(node != NULL, "Failed to get node from bucket when it should exist");

    return node->data;

error:
    return NULL;
}

int Hashmap_traverse(Hashmap *map, Hashmap_traverse_cb traverse_cb) {
    check(map, "Can't traverse a NULL hashmap");
    check(traverse_cb, "Can't have a NULL traverse_cb function");
    int i = 0;
    int j = 0;
    int rc = 0;

    for(i = 0; i < DArray_count(map->buckets); i++) {
        DArray *bucket = DArray_get(map->buckets, i);
        if(bucket) {
            for(j = 0; j < DArray_count(bucket); j++) {
                HashmapNode *node = DArray_get(bucket, j);
                rc = traverse_cb(node);
                if(rc != 0) {
                    return rc;
                }
            }
        }
    }

    return 0;

error:
    return 1;
}

void *Hashmap_delete(Hashmap *map, void *key) {
    check(map, "Can't delete from a NULL hashmap");
    check(key, "Can't delete a NULL key from map");

    uint32_t hash = 0;
    DArray *bucket = Hashmap_find_bucket(map, key, 0, &hash);
    if(!bucket) {
        return NULL;
    }

    int i = Hashmap_get_node(bucket, key);
    if(i == -1) {
        return NULL;
    }

    HashmapNode *node = DArray_get(bucket, i);
    void *data = node->data;
    free(node);

    // swap the last node with the deleted node
    // to reduce the number of nodes in the bucket
    HashmapNode *ending = DArray_pop(bucket);
    if(ending != node) {
        DArray_set(bucket, i, ending);
    }

    qsort(bucket->contents, DArray_count(bucket), sizeof(void *), (DArray_compare)qsort_default_compare);
    return data;

error:
    return NULL;
}
