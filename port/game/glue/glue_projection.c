/* Register replay of the bounded HUD projection. */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "projection.h"

#define W(n) ((int16_t)D(n))

static void divs_word(int n, int16_t divisor) {
    int32_t dividend = (int32_t)D(n);
    int32_t quotient = dividend / divisor;
    D(n) = ((uint32_t)(uint16_t)(dividend % divisor) << 16) | (uint16_t)quotient;
}

int glue_C2EC90(void) {
    int16_t x = W(0), y = W(1), depth = W(2);
    project_view_point(x, y, depth);
    D(7) = 0xFFFFFFFBu;
    if (x >= depth || y >= depth) goto rejected;
    SET_W(D(3), (uint16_t)-x);
    if (W(3) >= depth) goto rejected;
    SET_W(D(3), (uint16_t)-y);
    if (W(3) >= depth) goto rejected;
    if (depth <= 0) {
        D(0) = 0;
        return glue_return();
    }
    D(0) = (uint32_t)((int32_t)x * 160);
    divs_word(0, depth);
    SET_W(D(0), (uint16_t)(W(0) + 160));
    if (W(0) < 0) SET_W(D(0), 0);
    else if (W(0) >= 320) SET_W(D(0), 319);
    D(1) = (uint32_t)((int32_t)y * 90);
    divs_word(1, depth);
    SET_W(D(1), (uint16_t)(W(1) + 90));
    if (W(1) < 0) SET_W(D(1), 0);
    else if (W(1) >= 180) SET_W(D(1), 179);
    SET_W(D(0), (uint16_t)(319 - W(0)));
    SET_W(D(1), (uint16_t)(180 - W(1)));
    if (W(1) > rd_s16(LINE_LAST_ROW)) goto rejected;
    D(7) = 0xFFFFFFFFu;
    return glue_return();
rejected:
    D(0) = 0;
    return glue_return();
}
