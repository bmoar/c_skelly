#ifndef darray_algos_h
#define darray_algos_h

#include <ds/darray.h>

typedef int (*DArray_compare)(const void *a, const void *b);

int DArray_qsort(DArray *array, DArray_compare cmp);

#endif
