#ifndef _DArray_h
#define _DArray_h
#include <stdlib.h>
#include <assert.h>
#include <ds/debug.h>

#define DEFAULT_EXPAND_RATE 300

typedef struct DArray {
    int end;
    int max;
    size_t element_size;
    size_t expand_rate;
    void **contents;
} DArray;

#define DArray_last(A) ((A)->contents[(A)->end - 1])
#define DArray_first(A) ((A)->contents[0])
#define DArray_end(A) ((A)->end)
#define DArray_count(A) DArray_end(A)
#define DArray_max(A) ((A)->max)
#define DArray_free(E) free((E))

typedef int (*DArray_compare)(const void *a, const void *b);
/*
 * Create a new DArray with elements of size_t and the number
 * of max elements set to initial_max
 */
DArray *DArray_create(size_t element_size, size_t initial_max);

/*
 * Free each element of the DArray
 */
void DArray_clear(DArray *array);

/*
 * clear and destroy a DArray
 */
void DArray_clear_destroy(DArray *array);

/*
 * Shrink the DArray by the expand rate
 */
int DArray_contract(DArray *array);

/*
 * Frees a DArray
 */
void DArray_destroy(DArray *array);

/*
 * Expand the DArray to hold more elements
 */
int DArray_expand(DArray *array);

/*
 * Binary search for an element
 * returns the position in the DArray of the element if found,
 * otherwise returns -1
 */
int DArray_find(DArray *array, void *el, DArray_compare cmp);

/*
 * Pop off data from the DArray
 */
void *DArray_pop(DArray *array);

/*
 * Push data onto the DArray at the next available spot
 */
int DArray_push(DArray *array, void *el);

/*
 * Sets the index i in DArray to element el
 */
static inline void DArray_set(DArray *array, int i, void *el) {
    check(i < DArray_max(array), "attempt to set past max");
    check(i >= 0, "attempt to set negative index");

    if(i > DArray_end(array)) {
        DArray_end(array) = i;
    }

    array->contents[i] = el;

error:
    return;
}

/*
 * Gets the element at index i of the DArray
 */
static inline void *DArray_get(DArray *array, int i) {
    check(i < DArray_max(array), "attempt to get past max");
    check(i >= 0, "attempt to get negative index");

    return array->contents[i];

error:
    return NULL;
}

/*
 * Removes the element at index i and NULLs that index
 */
static inline void *DArray_remove(DArray *array, int i) {
    check(i < DArray_max(array), "attempt to remove past max");
    check(i >= 0, "attempt to remove negative index");

    void *el = array->contents[i];
    array->contents[i] = NULL;

    return el;

error:
    return NULL;
}

/*
 * Returns block of memory for a new element of element_size
 */
static inline void *DArray_new(DArray *array) {
    check(array->element_size > 0, "Can't use DArray_new on 0 element_size darrays.");

    return calloc(1, array->element_size);

error:
    return NULL;
}

#endif
