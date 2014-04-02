#ifndef _file_stats_h
#define _file_stats_h

#include <stdio.h>

#include <ds/bstrlib.h>

#include <ds/stats.h>
#include <ds/darray.h>
#include <ds/bstr_helpers.h>

/*
 * A tool that samples stats from space delimited header + table files
 */

struct Stat_set {
    bstring header;
    Stats *st;
};

DArray *create_sets(struct bstrList *split);

void output_sets(DArray *sets);

#endif
