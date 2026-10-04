#include <stdio.h>
#include "../../port/recomp/frame_pacer.h"
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(void) {
    FA18FramePacer p;
    uint64_t now;
    unsigned i;
    fa18_frame_pacer_init(&p, 1000, 10000003);
    now = 1000;
    for (i = 0; i < 5000; ++i) now = fa18_frame_pacer_next(&p, now);
    CHECK(now == 1000 + UINT64_C(1000000300));
    fa18_frame_pacer_init(&p, 0, 1000000);
    CHECK(fa18_frame_pacer_next(&p, 3500) == 20000);
    CHECK(fa18_frame_pacer_next(&p, 41000) == 40000);
    CHECK(fa18_frame_pacer_next(&p, 45000) == 60000);
    CHECK(fa18_frame_pacer_next(&p, 250000) == 250000);
    CHECK(fa18_frame_pacer_next(&p, 253500) == 270000);
    puts("Host pacing: fractional clock, ordinary lateness and stall recovery pass");
    return 0;
}
