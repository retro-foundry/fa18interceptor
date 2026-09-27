#include "line.h"

#include <string.h>

/* Test-only captured packet fixture. It is deliberately not linked into the
 * native executable; the renderer uses data-driven line packets instead. */
int fa18_build_run060_frame7992_area_jobs(FA18AreaBlitJob jobs[4]) {
    static const FA18AreaBlitJob captured[4] = {
        {0x0fce, 0x0000, 0xffff, 0xffff, 3, 1, 133, 0, 1, 1, 5, 4, 18, 12},
        {0x0fce, 0x0000, 0xffff, 0xffff, 2, 1, 133, 1, 1, 1, 5, 4, 18, 12},
        {0x0fce, 0x0000, 0xffff, 0xffff, 1, 1, 133, 2, 1, 1, 5, 4, 18, 12},
        {0x0fce, 0x0000, 0xffff, 0xffff, 0, 1, 133, 3, 1, 1, 5, 4, 18, 12}
    };
    if (!jobs) return -1;
    memcpy(jobs, captured, sizeof captured);
    return 0;
}
