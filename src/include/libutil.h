#ifndef _LIB_ONLY_UTIL_H
#define _LIB_ONLY_UTIL_H

#include <stdbool.h>

void timing_start(void);
void timing_end(const char *label);
void timing_avg_reset(void);
void timing_avg_results(const char *label);
void timing_toggle_silent(bool silent);

#endif
