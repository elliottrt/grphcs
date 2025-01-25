#include "util.hpp"

#include <ctime>
#include <cstdio>

static struct timespec ts_start = { 0, 0 };
static struct timespec ts_end   = { 0, 0 };

void grphcsTimingStart(void) {
    timespec_get(&ts_start, TIME_UTC);
}

void grphcsTimingEnd(char const * label) {
    timespec_get(&ts_end, TIME_UTC);

    time_t secs = ts_end.tv_sec - ts_start.tv_sec;
	long nsecs = ts_end.tv_nsec - ts_start.tv_nsec;

    if (nsecs < 0) {
        nsecs += 1000000000;
        secs--;
    }

    if (label) printf("Timing '%s' took %li.%09li seconds.\n", label, secs, nsecs);
    else printf("Timing took %li.%09li seconds.\n", secs, nsecs);
}