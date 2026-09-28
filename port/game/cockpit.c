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
