#ifndef _radixmap_h
#define _radixmap_h

#include <stdint.h>

typedef union RMElement {
    uint64_t raw;
    struct {
        uint32_t key;
        uint32_t value;
    } data;
} RMElement;

typedef struct RadixMap {
    size_t max;
    size_t end;
    uint32_t counter;
    RMElement *contents;
    RMElement *temp;
} RadixMap;

/*
 * Intialize a new RadixMap with a max number of elements
 */
RadixMap *RadixMap_create(size_t max);

/*
 * Free a RadixMap map
 */
void RadixMap_destroy(RadixMap *map);

/*
 * Sort a RadixMap increasing order
 */
void RadixMap_sort(RadixMap *map);

/* 
 * Find an element in RadixMap
 */
RMElement *RadixMap_find(RadixMap *map, uint32_t key);

/*
 * add a value to a RadixMap
 */
int RadixMap_add(RadixMap *map, uint32_t key, uint32_t value);

/*
 * delete a value from a RadixMap
 */
int RadixMap_delete(RadixMap *map, RMElement *el);

#endif
