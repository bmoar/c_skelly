#include <ds/bstr_helpers.h>

int bstrip(bstring b, char *c, char *r) {
    const bstring find = bfromcstr(c);
    const bstring replace = bfromcstr(r);

    int rc = bfindreplace(b, find, replace, 0);

    bdestroy(find);
    bdestroy(replace);

    return rc;
}
