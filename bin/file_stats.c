#include <ds/stats.h>
#include <stdio.h>
#include <ds/bstrlib.h>
#include <ds/darray.h>

int main(int argc, const char *argv[])
{
    
    bstring path = NULL;
    DArray *stats = DArray_create(sizeof(Stats *), 20);
    check_mem(stats);
    DArray *headers = DArray_create(sizeof(bstring), 20);
    check_mem(headers);
    double val = 0;
    int cur_header = 0;
    int i = 0;
    ssize_t read = 0;
    size_t len = 0;
    char *buf = NULL;

    bstring space = bfromcstr(" ");
    bstring newline = bfromcstr("\n");
    check(argc == 2, "Usage: file_stats NUM_FILE");

    path = bfromcstr(argv[1]);
    FILE *f = fopen(bdata(path), "r");
    check(f, "Couldn't open stats file to parse");

    // get the headers from the file
    read = getline(&buf, &len, f);
    check(read >=0, "Failed to get the headers");

    bstring header_buf = bfromcstr(buf);
    struct bstrList *split = bsplits(header_buf, space);
    for (i = 0; i < split->qty; i++) {
        if(bstrcmp(split->entry[i], space) != 0 && 
            bstrcmp(split->entry[i], newline) != 0) {

            DArray_push(headers, split->entry[i]);
            Stats *st = Stats_create();
            check_mem(st);
            DArray_push(stats, st);
        }
    }

    // scan each header
    while(fscanf(f, "%lf", &val) == 1) {
        Stats *st = DArray_get(stats, cur_header);
        Stats_sample(st, val);
        cur_header++;
        if(cur_header >= DArray_count(headers)) { 
            cur_header = 0;
        }
    }
    fclose(f);

    for(i = 0; i < DArray_count(headers); i++) {
        bstring header = DArray_remove(headers, i);
        log_info("Stats for %s", bdata(header));
        Stats *st = DArray_get(stats, i);
        Stats_dump(st);
        free(st);
    }

    DArray_destroy(stats);
    DArray_destroy(headers);
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
