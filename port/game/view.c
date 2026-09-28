/* View zoom. */
#include "view.h"

#include "globals.h"
#include "memory.h"

#define ZOOM_MAXIMUM 0x80

void set_zoom_maximum(void) {
    wr_u8(ZOOM_FLAGS, (uint8_t)(rd_u8(ZOOM_FLAGS) | 0x80));
    wr_u16(ZOOM_SCALE, ZOOM_MAXIMUM);
    wr_u8(DISPLAY_UPDATE, 3);
}
