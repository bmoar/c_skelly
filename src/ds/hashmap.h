#ifndef _ds_Hashmap_h
#define _ds_Hashmap_h

#include <stdint.h>
#include <ds/darray.h>
#include <ds/algos.h>

#define DEFAULT_NUMBER_OF_BUCKETS 100

typedef int (*Hashmap_compare)(void *a, void *b);
typedef uint32_t (*Hashmap_hash)(void *key);

typedef struct Hashmap {
    DArray *buckets;
    Hashmap_compare compare;
    Hashmap_hash hash;
} Hashmap;

typedef struct HashmapNode {
    void *key;
    void *data;
    uint32_t hash;
} HashmapNode;

typedef int (*Hashmap_traverse_cb)(HashmapNode *node);

/*
 * Return a pointer to a new Hashmap.
 * Hashmap_compare is a function that returns an int. Default is bstrcmp.
 * Hashmap_hash is a hashing function. Default is SuperFastHash (SFH)
 * buckets is the number of buckets to start with. If NULL, default is 100 buckets
 */
Hashmap *Hashmap_create(Hashmap_compare compare, Hashmap_hash hash, int buckets);

/*
 * Frees all { key : value }, buckets, and the hashmap itself
 */
void Hashmap_destroy(Hashmap *map);

/*
 * Set the key to data, adds the key to the hashmap
 * if it doesn't exist. Returns 0 on success, -1 on error
 */
int Hashmap_set(Hashmap *map, void *key, void *data);

/*
 * Get the value stored for key
 * returns a void * to the data if found
 * returns NULL if not found or error
 */
void *Hashmap_get(Hashmap *map, void *key);

/*
 * Iterate through each { key : value } in the hashmap
 * passing each node to the traverse_cb callback function
 */
int Hashmap_traverse(Hashmap *map, Hashmap_traverse_cb traverse_cb);

/*
 * Remove a { key : value } from the map
 * returns a pointer to the value on success,
 * NULL on error
 */
void *Hashmap_delete(Hashmap *map, void *key);

#endif
