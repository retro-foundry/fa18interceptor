/* Renderer span bounds. */
#include "render_span.h"

#include "globals.h"
#include "memory.h"

int16_t bound_span(int16_t *position, int16_t limit, int32_t *cursor) {
    int16_t origin = rd_s16(SPAN_ORIGIN);
    int16_t start = (int16_t)(*position + origin);
    int16_t remaining;

    if ((int32_t)*position + origin < 0) {
        start = (int16_t)-start;
        *position = start;
        if (start >= limit) return -1;
        *cursor += (int16_t)((int16_t)(origin + start) * 2);
        return 0;
    }
    *cursor += (int16_t)(origin * 2);
    *position = 0;
    remaining = (int16_t)(start + limit);
    if ((int32_t)remaining - 20 < 0) return 0;
    remaining = (int16_t)(remaining - 20);
    return remaining >= limit ? -1 : remaining;
}
