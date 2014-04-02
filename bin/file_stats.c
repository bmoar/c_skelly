#include "file_stats.h"

DArray *create_sets(struct bstrList *split) {
    int i = 0;
    DArray *stat_sets = NULL;
    struct Stat_set *set = NULL;
    Stats *st = NULL;
    stat_sets = DArray_create(sizeof(struct Stat_set *), 20);
    check_mem(stat_sets);

    for (i = 0; i < split->qty; i++) {
        set = malloc(sizeof(struct Stat_set));
        check_mem(set);
        st = Stats_create();
        check_mem(st);

        set->header = split->entry[i];
        set->st = st;
        DArray_push(stat_sets, set);
    }

    return stat_sets;

error:
    if(stat_sets) free(stat_sets);
    if(set) free(set);
    if(st) free(st);
    
    return NULL;
}

void output_sets(DArray *sets) {
    check(sets, "Can't output NULL set");

    int i = 0;
    for(i = 0; i < DArray_count(sets); i++) {
        struct Stat_set *set = DArray_remove(sets, i);

        log_info("Stats for %s", bdata(set->header));
        Stats_dump(set->st);

        free(set->st);
        free(set);
    }

error:
    return;
}

int main(int argc, const char *argv[])
{
    
    bstring path = NULL;
    DArray *stat_sets = NULL;
    ssize_t read = 0;
    size_t len = 0;
    char *buf = NULL;
    int rc = 0;
    int cur_header = 0;
    double val = 0;

    check(argc == 2, "Usage: file_stats NUM_FILE");

    path = bfromcstr(argv[1]);
    FILE *f = fopen(bdata(path), "r");
    check(f, "Couldn't open stats file to parse");

    bstring space = bfromcstr(" ");
    bstring newline = bfromcstr("\n");
    // get the headers from the file
    read = getline(&buf, &len, f);
    check(read >=0, "Failed to get the headers");

    bstring header_buf = bfromcstr(buf);
    rc = bstrip(header_buf, "\n", "");
    check(rc != BSTR_ERR, "Failed to strip newline"); 

    struct bstrList *split = bsplits(header_buf, space);
    stat_sets = create_sets(split);

    // collect samples from files
    while(fscanf(f, "%lf", &val) == 1) {
        struct Stat_set *set = DArray_get(stat_sets, cur_header);
        Stats_sample(set->st, val);
        cur_header++;
        if(cur_header >= DArray_count(stat_sets)) { 
            cur_header = 0;
        }
    }

    fclose(f);

    output_sets(stat_sets);

    DArray_destroy(stat_sets);
    bstrListDestroy(split);
    bdestroy(path);
    bdestroy(space);
    bdestroy(newline);
    bdestroy(header_buf);
    free(buf);
    return 0;

error:
    if(path) {
        bdestroy(path);
    }
    return 1;
}
