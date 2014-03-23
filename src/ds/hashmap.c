#undef NDEBUG
#include <stdint.h>
#include <ds/hashmap.h>
#include <ds/debug.h>
#include <ds/bstrlib.h>

static int default_compare(void *a, void *b) {
    return bstrcmp((bstring)a, (bstring)b);
}

/*
 * Bob Jenkins hash from wikipedia
 */
static uint32_t default_hash(void *a) {
    size_t len = blength((bstring)a);
    char *key = bdata((bstring)a);
    uint32_t hash = 0;
    uint32_t i = 0;

    for(hash = i = 0; i < len; i++) {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

uint32_t SuperFashHash(void *a) {
    uint32_t len = blength((bstring)a);
    char *data = bdata((bstring)a);
    uint32_t hash = 0;
    uint32_t tmp = 0;
    int rem;

    if (len <= 0 || data == NULL) return 0;

    rem = len & 3;
    len >>= 2;

    /* Main loop */
    for (;len > 0; len--) {
        hash  += get16bits (data);
        tmp    = (get16bits (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2*sizeof (uint16_t);
        hash  += hash >> 11;
    }

    /* Handle end cases */
    switch (rem) {
        case 3: hash += get16bits (data);
                hash ^= hash << 16;
                hash ^= ((signed char)data[sizeof (uint16_t)]) << 18;
                hash += hash >> 11;
                break;
        case 2: hash += get16bits (data);
                hash ^= hash << 11;
                hash += hash >> 17;
                break;
        case 1: hash += (signed char)*data;
                hash ^= hash << 10;
                hash += hash >> 1;
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}

Hashmap *Hashmap_create(Hashmap_compare compare, Hashmap_hash hash) {
    Hashmap *map = calloc(1, sizeof(Hashmap));
    check_mem(map);

    map->compare = compare == NULL ? default_compare : compare;
    map->hash = hash == NULL ? default_hash : hash;
    map->buckets = DArray_create(sizeof(DArray *), DEFAULT_NUMBER_OF_BUCKETS);
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

static inline int Hashmap_get_node(Hashmap *map, uint32_t hash, DArray *bucket, void *key) {
    int i = 0;

    for(i = 0; i < DArray_end(bucket); i++) {
        HashmapNode *node = DArray_get(bucket, i);
        if(node->hash == hash && map->compare(node->key, key) == 0) {
            return i;
        }
    }

    return -1;
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
    int rc = Hashmap_get_node(map, hash, bucket, key);

    if(rc >= 0) {
        // key already in hashmap
        HashmapNode *node = DArray_get(bucket, rc);
        node->data = data;
        node->hash = hash;
    } else {
        HashmapNode *node = HashmapNode_node_create(hash, key, data);
        check_mem(node);
        DArray_push(bucket, node);
    }

    return 0;

error:
    return -1;
}

void *Hashmap_get(Hashmap *map, void *key) {
    uint32_t hash = 0;
    DArray *bucket = Hashmap_find_bucket(map, key, 0, &hash);
    if(!bucket) {
        return NULL;
    }

    int i = Hashmap_get_node(map, hash, bucket, key);
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
    uint32_t hash = 0;
    DArray *bucket = Hashmap_find_bucket(map, key, 0, &hash);
    if(!bucket) {
        return NULL;
    }

    int i = Hashmap_get_node(map, hash, bucket, key);
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

    return data;
}
