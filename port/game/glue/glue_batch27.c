/* Glue for update_target_point $C1C2C8 and blit_lane $C304FA. */
#include "glue.h"
#include "ports_glue.h"

#include "fixed_math.h"
#include "globals.h"
#include "memory.h"
#include "render_buffers.h"

/* $C1C2C8 restores D0-D6; A0 is left at the viewed record when enabled. */
int glue_C1C2C8(void) {
    int enabled = rd_u8(TARGET_ENABLED) != 0;
    update_target_point();
    if (enabled) A(0) = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
    return glue_return();
}

/* $C304FA: D0.w plane offset, D3 bit 0 the minterm choice. The caller reads
 * D4 (C modulo, word), D5 (C pointer), D6, D7 and the high words of D0-D3. */
void blit_lane_registers(void) {
    int16_t plane_offset = (int16_t)D(0);
    uint16_t size = rd_u16(POLY_BLIT_SIZE);
    gaddr table = rd_u32(PAGE_PLANE_TABLE);
    uint32_t plane = rd_u32(table + (gaddr)(int32_t)plane_offset);
    int16_t row = (int16_t)(rd_s16(POLY_MAX_Y) - rd_s16(REDRAW_STATE_WORD) - 0xB7);
    int16_t modulo = (int16_t)(3 - (int16_t)(size & 0x3F));
    uint32_t d6 = (uint32_t)(int32_t)(int16_t)(row * 4);
    uint32_t d5 = LANE_PATTERN + d6 - 2;

    if (modulo != 1) {
        SET_W(d6, (uint16_t)(rd_s16(POLY_MIN_X) >> 4));
        SET_W(D(7), (uint16_t)(rd_s16(SPAN_ORIGIN) + 12));
        if ((int16_t)d6 == (int16_t)D(7)) d5 -= 2;
    }
    SET_W(D(0), size);
    D(1) = rd_u32(POLY_PLANE_OFFSET) + plane;
    D(2) = rd_u32(POLY_MASK_SOURCE);
    SET_W(D(4), (uint16_t)modulo);
    D(5) = d5;
    D(6) = d6;
    A(0) = 0xDFF000u;
    A(2) = table;
}

int glue_C304FA(void) {
    int16_t plane_offset = (int16_t)D(0);
    int pattern = (int)(D(3) & 1);
    blit_lane(plane_offset, pattern);
    blit_lane_registers();
    return glue_return();
}
