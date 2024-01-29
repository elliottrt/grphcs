#ifndef _GRPHCS_UTIL_H
#define _GRPHCS_UTIL_H

#ifndef UNUSED
#define UNUSED(X) (void)(X)
#endif

void grphcsTimingStart(void);
void grphcsTimingEnd(char const * label);

#endif
