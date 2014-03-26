#ifndef _string_algos_h
#define _string_algos_h

#include <ds/bstrlib.h>
#include <ds/darray.h>

typedef struct StringScanner {
    bstring in;
    const unsigned char *haystack;
    ssize_t hlen;
    const unsigned char *needle;
    ssize_t nlen;
    size_t skip_chars[UCHAR_MAX + 1];
} StringScanner;

int String_find(bstring in, bstring to_find);

StringScanner *StringScanner_create(bstring in);

int StringScanner_scan(StringScanner *scan, bstring to_find);

void StringScanner_destroy(StringScanner *scan);

#endif
