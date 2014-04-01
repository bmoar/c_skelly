#include <ds/stats.h>
#include <stdio.h>
#include <ds/bstrlib.h>

int main(int argc, const char *argv[])
{
    
    bstring path = NULL;
    check(argc == 2, "Usage: file_stats NUM_FILE");

    path = bfromcstr(argv[1]);

    FILE *f = fopen(bdata(path), "r");
    check(f, "Couldn't open stats file to parse");

    Stats *st = Stats_create();
    check_mem(st);

    double val = 0;
    while(fscanf(f, "%lf", &val) == 1) {
        Stats_sample(st, val);
    }

    fclose(f);
    Stats_dump(st);

    bdestroy(path);
    return 0;

error:
    if(path) {
        bdestroy(path);
    }
    return 1;
}
