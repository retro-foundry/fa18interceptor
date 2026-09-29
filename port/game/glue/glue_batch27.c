/* Glue for update_target_point $C1C2C8, classify_record_range $C24568 and
 * blit_lane $C304FA. */
#include "glue.h"
#include "ports_glue.h"

#include "fixed_math.h"
#include "globals.h"
#include "memory.h"
#include "render_buffers.h"

void magnitude_registers(void); /* glue_batch22.c */

/* $C1C2C8 restores D0-D6; A0 is left at the viewed record when enabled. */
int glue_C1C2C8(void) {
    int enabled = rd_u8(TARGET_ENABLED) != 0;
    update_target_point();
    if (enabled) A(0) = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
    return glue_return();
}

static uint32_t cell_part(gaddr r, int cell, int fine, uint32_t coarse_reg, uint32_t *detail_reg) {
    /* EXT.L, SUB.W (the cell), SWAP, ASR.L #2; the fine part SUB.W then
     * EXT.L into its own register. */
    uint32_t v = (uint32_t)(int32_t)(int16_t)coarse_reg;
    int32_t d;
    SET_W(v, (uint16_t)((int16_t)v - rd_s16(r + (gaddr)cell)));
    v = v << 16 | v >> 16;
    d = (int32_t)v >> 2;
    *detail_reg = (uint32_t)(int32_t)(int16_t)((int16_t)*detail_reg - rd_s16(r + (gaddr)fine));
    d += (int32_t)*detail_reg;
    return (uint32_t)(d < 0 ? -d : d);
}

/* $C24568: A1 record. The caller reads D0, D1, D3 and D4. */
int glue_C24568(void) {
    gaddr r = A(1);
    uint8_t counter = (uint8_t)(rd_u8(r + 0x39) + 0x10);
    uint32_t d0, d1, d2, d3, d4;

    if (rd_s16(r + 0x4A) >= 0x480) {
        int8_t period = rd_s16(r + 0x4A) >= 0x900 ? 0x50 : 0x20;
        D(1) = (uint32_t)(int32_t)period;
        SET_B(D(0), counter & 0xF0);
        if (period >= (int8_t)(counter & 0xF0)) {
            classify_record_range(r);
            return glue_return();
        }
    }
    if (rd_s16(r + 0x2C) < 0) {
        classify_record_range(r);
        return glue_return();
    }
    d2 = D(2); d0 = (uint32_t)rd_u16(r + 0x30); d1 = (uint32_t)rd_u16(r + 0x32);
    d4 = (uint32_t)rd_u16(r + 0x2E);
    d2 = cell_part(r, 0x06, 0x0C, (uint32_t)rd_u16(r + 0x2C), &d0);
    d4 = cell_part(r, 0x08, 0x0E, d4, &d1);
    d3 = (uint32_t)(rd_s32(r + 0x34) - rd_s32(r + 0x10));
    if ((int32_t)d3 < 0) d3 = 0u - d3;
    D(0) = 0x7F00;
    D(1) = d1;
    D(2) = d2;
    D(3) = d3;
    D(4) = d4;
    if ((int32_t)d2 > 0x7F00 || (int32_t)d3 > 0x7F00 || (int32_t)d4 > 0x7F00) {
        classify_record_range(r);
        return glue_return();
    }
    magnitude_registers();
    classify_record_range(r);
    return glue_return();
}

/* $C304FA: D0.w plane offset, D3 bit 0 the minterm choice. The caller reads
 * D4 (C modulo, word), D5 (C pointer), D6, D7 and the high words of D0-D3. */
int glue_C304FA(void) {
    int16_t plane_offset = (int16_t)D(0);
    uint16_t size = rd_u16(POLY_BLIT_SIZE);
    gaddr table = rd_u32(PAGE_PLANE_TABLE);
    uint32_t plane = rd_u32(table + (gaddr)(int32_t)plane_offset);
    int16_t row = (int16_t)(rd_s16(POLY_MAX_Y) - rd_s16(REDRAW_STATE_WORD) - 0xB7);
    int16_t modulo = (int16_t)(3 - (int16_t)(size & 0x3F));
    uint32_t d6 = (uint32_t)(int32_t)(int16_t)(row * 4);
    uint32_t d5 = LANE_PATTERN + d6 - 2;

    blit_lane(plane_offset, (int)(D(3) & 1));
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
    return glue_return();
}
