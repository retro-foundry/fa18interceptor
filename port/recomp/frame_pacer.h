/* Host presentation clock only: no guest cycles or game state. */
#ifndef FA18_FRAME_PACER_H
#define FA18_FRAME_PACER_H
#include <stdint.h>

typedef struct FA18FramePacer {
    uint64_t deadline, frequency;
    unsigned remainder;
} FA18FramePacer;

static void fa18_frame_pacer_init(FA18FramePacer *p, uint64_t now, uint64_t frequency) {
    p->deadline = now;
    p->frequency = frequency;
    p->remainder = 0;
}

/* Keep fractional ticks so the clock does not drift on unusual host clocks.
 * Drop host scheduling debt after a missed period; execute every guest frame. */
static uint64_t fa18_frame_pacer_next(FA18FramePacer *p, uint64_t now) {
    uint64_t period = p->frequency / 50;
    p->remainder += (unsigned)(p->frequency % 50);
    if (p->remainder >= 50) { ++period; p->remainder -= 50; }
    p->deadline += period;
    if (now > p->deadline && now - p->deadline >= period) {
        p->deadline = now;
        p->remainder = 0;
    }
    return p->deadline;
}
#endif
