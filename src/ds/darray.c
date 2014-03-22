#include <ds/darray.h>
#include <assert.h>

DArray *DArray_create(size_t element_size, size_t initial_max) {
    DArray *array = NULL;

    array = malloc(sizeof(DArray));
    check_mem(array);
    DArray_max(array) = initial_max;
    check(DArray_max(array) > 0, "Initial max must be > 0");

    array->contents = calloc(initial_max, sizeof(void *));
    check_mem(array->contents);

    DArray_end(array) = 0;
    array->element_size = element_size;
    array->expand_rate = DEFAULT_EXPAND_RATE;

    return array;

error:
    if(array) { free(array); }
    return NULL;
}

void DArray_clear(DArray *array) {
    check(array != NULL, "Can't clear a NULL array");
    int i = 0;
    if(array->element_size > 0) {
        for(i = 0; i < DArray_max(array); i++) {
            if(array->contents[i] != NULL) {
                free(array->contents[i]);
            }
        }
    }

error:
    return;
}

void DArray_clear_destroy(DArray *array) {
    check(array != NULL, "Can't clear_destroy a NULL array");
    DArray_clear(array);
    DArray_destroy(array);

error:
    return;
}

static inline int DArray_resize(DArray *array, size_t newsize) {
    check(newsize > 0, "Can't set newsize of array <= 0");
    check(array != NULL, "Can't resize a NULL DArray");
    DArray_max(array) = newsize;

    //assume realloc doesn't harm the original contents
    void *contents = realloc(array->contents, DArray_max(array) * sizeof(void *));
    check_mem(contents);

    array->contents = contents;

    return 0;
error:
    return -1;
}

int DArray_contract(DArray *array) {
    check(array != NULL, "Can't contract a NULL array");
    int new_size = DArray_end(array) < (int)array->expand_rate ? (int)array->expand_rate : array->end;

    return DArray_resize(array, new_size + 1);

error:
    return -1;
}

void DArray_destroy(DArray *array) {
    check(array != NULL, "Can't destroy a NULL array");
    if(array) {
        if(array->contents) {
            free(array->contents);
        }
        free(array);
    }

error:
    return;
}

int DArray_expand(DArray *array) {
    check(array != NULL, "Can't expand a NULL array");

    size_t old_max = DArray_max(array);
    check(DArray_resize(array, DArray_max(array) + array->expand_rate) == 0,
            "Failed to expand array to new size: %d",
            array->max + (int)array->expand_rate);

    memset(array->contents + old_max, 0, array->expand_rate + 1);
    return 0;

error:
    return -1;
}

int DArray_push(DArray *array, void *el) {
    check(array != NULL, "Can't push to a NULL array");

    array->contents[array->end] = el;
    array->end++;

    if(DArray_end(array) >= DArray_max(array)) {
        return DArray_expand(array);
    } else {
        return 0;
    }

error:
    return -1;
}

void *DArray_pop(DArray *array) {
    check(array != NULL, "Can't pop from a NULL array");
    check(array->end - 1 >= 0, "Can't pop from an empty array");

    void *el = DArray_remove(array, array->end - 1);
    array->end--;

    if(DArray_end(array) > (int)array->expand_rate && DArray_end(array) % array->expand_rate) {
        DArray_contract(array);
    }

    return el;

error:
    return NULL;
}
