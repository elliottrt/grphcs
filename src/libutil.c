#include "include/libutil.h"

#include <time.h>
#include <stdio.h>

static struct timespec ts_start = {0};
static struct timespec ts_end   = {0};
static double timing_total = 0;
static size_t timing_count = 0;
static bool _silent = false;

void timing_start(void) { 
    timespec_get(&ts_start, TIME_UTC); 
}

void timing_end(const char *label) {
	timespec_get(&ts_end, TIME_UTC);
	if (!label) {
		fprintf(stderr, "Label not provided to 'timing_end(const char *label)'.\n");
		return;
	};
	time_t secs = ts_end.tv_sec - ts_start.tv_sec;
	long nsecs = ts_end.tv_nsec - ts_start.tv_nsec;
	if (nsecs < 0) {
		if (!_silent) fprintf(stderr, "Overflow in timing_end.\n");
		return;
	}
	timing_count++;
	timing_total += secs;
	timing_total += (double) nsecs / (double) (1000000000);
	if (!_silent) printf("Timing '%s' took %li.%09li seconds.\n", label, secs, nsecs);
}

void timing_avg_reset(void) {
	timing_total = 0;
	timing_count = 0;
}

void timing_avg_results(const char *label) {
	double average = timing_total / timing_count;
	printf("Timing '%s' took an average of %f seconds.\n", label, average);
}

void timing_toggle_silent(bool silent) { _silent = silent; }
