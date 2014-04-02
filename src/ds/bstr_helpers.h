#ifndef _bstr_helpers_h
#define _bstr_helpers_h

/*
 * Some helpful wrappers to do common bstr tasks
 */

#include <ds/bstrlib.h>
#include <ds/debug.h>

/*
 * Strip c from a bstring, replacing with r
 */
int bstrip(bstring b, char *c, char *r);

#endif
