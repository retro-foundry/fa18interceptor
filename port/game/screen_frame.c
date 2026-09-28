/* View frame coordinate lists. */
#include "screen_frame.h"

#include "globals.h"

static gaddr frame_point(int16_t index) {
    return FRAME_POINTS + (gaddr)(int32_t)(int16_t)(index * 8);
}

void append_point(gaddr *cursor, int16_t x, int16_t y) {
    wr_s16(*cursor, x);
    wr_s16(*cursor + 2, y);
    *cursor += 4;
}

void append_mirrored_points(gaddr *cursor, int16_t first, int16_t second) {
    gaddr a = frame_point(first), b = frame_point(second);
    append_point(cursor, (int16_t)(VIEW_RIGHT - rd_s16(a)), (int16_t)(VIEW_BOTTOM - rd_s16(a + 2)));
    append_point(cursor, (int16_t)(VIEW_RIGHT - rd_s16(b)), (int16_t)(VIEW_BOTTOM - rd_s16(b + 2)));
}
