#ifndef ds_stats_h
#define ds_stats_h

#include <math.h>
#include <stdlib.h>

#include <ds/debug.h>

/*
 * Rolling stats calculations made easy.
 * Find "interesting things" by collecting summary stats
 * without needing to store all the samples.
 */

/*
 * First way to use:
 * Collect stats on users, errors, w/e and just dump them out
 *
 * Second way:
 * Collect stats on the stats of #1 above. This gives us
 * 
 * mean of means across all collected stats
 *
 * mean of stddev across all collected stats
 *
 * Very useful...
 *
 * EX: (thx Zed Shaw)
 *
    User John logs into and out of server A. Grab server A's stats, update them.
    Grab the mean of means stats, and take A's mean and add it as a sample. I'll call this m_of_m.
    Grab the mean of stddevs stats, and add A's stddev to it as a sample. I'll call this m_of_s.
    If A's mean is outside of m_of_m.mean + 2 * m_of_m.stddev then flag it as possibly having a problem.
    If A's stddev is outside of m_of_s.mean + 2 * m_of_s.stddev then flag it as possible behaving too erratically.
    Finally, if John's login time is outside of A's range, or A's m_of_m range, then flag it as interesting.
 */

typedef struct Stats {
    double sum;
    double sumsq;
    unsigned long n;
    double min;
    double max;
} Stats;

Stats *Stats_recreate(double sum, double sumsq, unsigned long n, double min, double max);

Stats *Stats_create();

static inline double Stats_mean(Stats *st)  {
    return st->sum / st->n;
}

static inline double Stats_stddev(Stats *st) {
    return sqrt((st->sumsq - ( st->sum * st->sum / st->n)) / (st->n - 1));
}
void Stats_sample(Stats *st, double s);

void Stats_dump(Stats *st);

#endif
