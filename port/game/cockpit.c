/* Cockpit display redraw requests. */
#include "cockpit.h"

#include "globals.h"
#include "memory.h"

#define REDRAW_PASSES 3 /* enough for both display pages */

void request_cockpit_redraw(void) {
    static const uint8_t displays[] = {0x1, 0x0, 0x3, 0x4, 0x8, 0x9, 0xA, 0xB, 0x5, 0x7, 0xD, 0xE, 0xF, 0x6};
    unsigned i;
    for (i = 0; i < sizeof displays; i++) wr_u8(REDRAW_FIRST + displays[i], REDRAW_PASSES);
    if (!rd_u8(REDRAW_KEEP_STATE)) {
        wr_u16(REDRAW_STATE_WORD, 0);
        wr_u32(REDRAW_STATE_LONG, 0);
    }
}

void finish_scene_setup(void) {
    wr_u16(LINE_LAST_ROW, 0x90);
    request_cockpit_redraw();
}

void step_cockpit_slide(void) {
    int8_t animation = (int8_t)rd_u8(SLIDE_ANIMATION);
    int16_t origin = rd_s16(SPAN_ORIGIN);
    gaddr entry;

    if (!animation) return;
    if ((origin < 0 ? -origin : origin) > 14) {
        wr_u8(SLIDE_ANIMATION, 0);
        return;
    }
    entry = SLIDE_TABLE + (gaddr)(int32_t)(int16_t)((animation - 1) * 8);
    if ((int8_t)rd_u8(rd_u32(entry)) > (int8_t)rd_u8(SLIDE_STEP)) {
        int16_t rows;
        if (!(rd_u16(COCKPIT_FLAGS) & 0x400)) {
            wr_u8(SLIDE_STEP, 0);
            wr_u16(COCKPIT_FLAGS, (uint16_t)(rd_u16(COCKPIT_FLAGS) | 0x400));
        }
        rows = rd_s8(rd_u32(entry + 4) + (gaddr)(int32_t)(int8_t)rd_u8(SLIDE_STEP));
        wr_u8(SLIDE_STEP, (uint8_t)(rd_u8(SLIDE_STEP) + 1));
        wr_s16(LINE_LAST_ROW, (int16_t)(rows + 0x90));
        wr_s16(REDRAW_STATE_WORD, rows);
        wr_s32(REDRAW_STATE_LONG, (int16_t)(rows * 40));
    } else {
        wr_u8(SLIDE_ANIMATION, 0);
        wr_u8(SLIDE_STEP, 0);
        wr_u16(COCKPIT_FLAGS, (uint16_t)(rd_u16(COCKPIT_FLAGS) & ~0x400));
        wr_u32(REDRAW_STATE_LONG, 0);
        wr_u16(LINE_LAST_ROW, 0x90);
    }
    request_cockpit_redraw();
}
