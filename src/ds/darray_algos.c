#include <ds/darray_algos.h>
#include <stdlib.h>

int DArray_qsort(DArray *array, DArray_compare cmp) {
    check(array != NULL, "Can't qsort a NULL DArray");
    check(cmp != NULL, "Can't qsort with a NULL cmp function");

    qsort(array->contents, DArray_count(array), sizeof(void *), cmp);
    return 0;

error:
    return 1;
}
