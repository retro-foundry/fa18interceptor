/* The bounded screen projection used by the HUD ($C2EC90). */
#include "projection.h"

#include "fault.h"
#include "globals.h"
#include "memory.h"

int project_view_point(int16_t x, int16_t y, int16_t depth) {
    int16_t sx, sy;
    if (x >= depth || y >= depth || (int16_t)-x >= depth || (int16_t)-y >= depth) {
        wr_u32(PROJECTED_PAIR, 0xFFFFFFFFu);
        return 0;
    }
    if (depth <= 0) {
        wr_u16(ERROR_CODE, 0x1B);
        fault_hook();
        return 0;
    }
    sx = (int16_t)((int32_t)x * 160 / depth + 160);
    sy = (int16_t)((int32_t)y * 90 / depth + 90);
    if (sx < 0) sx = 0;
    else if (sx >= 320) sx = 319;
    if (sy < 0) sy = 0;
    else if (sy >= 180) sy = 179;
    sx = (int16_t)(319 - sx);
    sy = (int16_t)(180 - sy);
    if (sy > rd_s16(LINE_LAST_ROW)) {
        wr_u32(PROJECTED_PAIR, 0xFFFFFFFFu);
        return 0;
    }
    wr_u16(PROJECTED_PAIR, (uint16_t)sx);
    wr_u16(PROJECTED_PAIR + 2, (uint16_t)sy);
    return 1;
}
