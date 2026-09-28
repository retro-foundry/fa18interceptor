#ifndef FA18_DISPLAY_RECORD_PROJECTION_H
#define FA18_DISPLAY_RECORD_PROJECTION_H

#include <stdint.h>

typedef struct {
    int16_t x;
    int16_t y;
} FA18DisplayRecordPair;

/* `$C2E9F8-$C2EA59`: projects the accepted `$C45AC6` triplet into the
 * `$C4B990` workspace.  A nonpositive divisor is the source's busy loop and
 * is reported explicitly rather than reproduced in native code. */
int fa18_project_adjusted_display_pair(int16_t x, int16_t y, int16_t divisor,
                                       FA18DisplayRecordPair *pair);

#endif
