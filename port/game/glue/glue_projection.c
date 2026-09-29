/* Register replay of the bounded HUD projection. */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "glue_text.h"
#include "memory.h"
#include "projection.h"

void pair_registers(void);
void block_registers(void);
void filled_circle_registers(void);

#define W(n) ((int16_t)D(n))

static void divs_word(int n, int16_t divisor) {
    int32_t dividend = (int32_t)D(n);
    int32_t quotient = dividend / divisor;
    D(n) = ((uint32_t)(uint16_t)(dividend % divisor) << 16) | (uint16_t)quotient;
}

static int projection_mode(int16_t mode, int entry) {
    int16_t x = W(0), y = W(1), depth = W(2);
    int16_t size = rd_s16(A(6) - 0x28);
    int16_t radius = W(6);
    if (entry == 0) project_view_point(x, y, depth);
    else project_view_point_mode(x, y, depth, mode, size, radius);
    if (entry == 0) D(7) = 0xFFFFFFFBu;
    else if (entry == 1) SET_W(D(7), (uint16_t)mode);
    else D(7) = (uint32_t)(int32_t)mode;
    if (x >= depth || y >= depth) goto rejected;
    SET_W(D(3), (uint16_t)-x);
    if (W(3) >= depth) goto rejected;
    SET_W(D(3), (uint16_t)-y);
    if (W(3) >= depth) goto rejected;
    if (depth <= 0) {
        D(0) = 0;
        flags_logic_l(D(0));
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
    if (mode >= 0) {
        int count = mode & 63;
        SET_W(D(2), (uint16_t)((int16_t)0x30 >> (count >= 16 ? 15 : count)));
        if (W(2) >= size) goto block;
        SET_W(D(2), (uint16_t)((int16_t)0x50 >> (count >= 16 ? 15 : count)));
        if (W(2) >= size) goto pair;
        goto pixel;
    }
    SET_W(D(7), (uint16_t)(W(7) + 1));
    if (W(7) >= 0) goto pixel;
    SET_W(D(7), (uint16_t)(W(7) + 1));
    if (W(7) >= 0) goto pair;
    SET_W(D(7), (uint16_t)(W(7) + 1));
    if (W(7) >= 0) goto block;
    SET_W(D(7), (uint16_t)(W(7) + 1));
    if (W(7) >= 0) {
        filled_circle_registers();
        goto drawn;
    }
    flags_logic_w(D(0));
    return glue_return();
pixel:
    plot_registers(PIXEL_MASKS, PLOT_ROWS_1);
    goto drawn;
pair:
    pair_registers();
    goto drawn;
block:
    block_registers();
drawn:
    D(0) = 1;
    flags_logic_l(D(0));
    return glue_return();
rejected:
    D(0) = 0;
    flags_logic_l(0xFFFFFFFFu); /* the invalid pair's MOVE.L */
    return glue_return();
}

int glue_C2EC90(void) { return projection_mode(-5, 0); }
int glue_C2EC94(void) { return projection_mode(rd_s16(0xC45AB8u), 1); }
int glue_C2EC9C(void) { return projection_mode(-4, 2); }
int glue_C2ECA4(void) { return projection_mode(-2, 2); }
