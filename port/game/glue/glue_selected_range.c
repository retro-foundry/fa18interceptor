/* $C244E2: register effects of the selected record range classification. */
#include "glue.h"
#include "ports_glue.h"

#include "fixed_math.h"
#include "globals.h"
#include "memory.h"

void magnitude_registers(void); /* glue_batch22.c */

static uint32_t cell_part(gaddr r, int cell, int fine, uint32_t coarse, uint32_t *detail) {
    uint32_t v = (uint32_t)(int32_t)(int16_t)coarse;
    int32_t d;
    SET_W(v, (uint16_t)((int16_t)v - rd_s16(r + (gaddr)cell)));
    v = (v << 16) | (v >> 16);
    d = (int32_t)v >> 2;
    *detail = (uint32_t)(int32_t)(int16_t)((int16_t)*detail - rd_s16(r + (gaddr)fine));
    d += (int32_t)*detail;
    return (uint32_t)(d < 0 ? -d : d);
}

int glue_C244E2(void) {
    gaddr r = A(1), source;
    int16_t selected = rd_s16(SELECTED_RECORD);
    int16_t prior_distance = rd_s16(r + 0x4A);
    uint8_t counter = (uint8_t)(rd_u8(r + 0x39) + 0x10);
    int gate = rd_s16(r + 0x6C) >= 0x1200;
    int tone = gate && !(rd_u8(r + 0x7C) & 0x70) && !rd_u8(BAR_REDRAWS_F);
    uint32_t d0, d1, d2, d3, d4;

    if (gate) {
        SET_B(D(1), rd_u8(r + 0x7C));
        SET_B(D(1), (uint8_t)D(1) & 0x70);
    }
    classify_selected_record_range(r);
    /* $C3316E saves and restores its caller's registers.  It was entered
     * after MOVEQ #4,D0, so the following MOVE.W starts with D0's high word
     * clear whenever this call was made. */
    if (tone) D(0) = 4;
    SET_W(D(0), (uint16_t)selected);
    if (selected <= 0) return glue_return();
    source = CONTROL_RECORDS + (gaddr)(int32_t)selected;
    A(2) = source;
    if (prior_distance >= 0x480) {
        int8_t period = prior_distance >= 0x900 ? 0x50 : 0x20;
        D(1) = (uint32_t)(int32_t)period;
        SET_B(D(0), counter & 0xF0);
        if (period >= (int8_t)(counter & 0xF0)) return glue_return();
    }
    SET_W(D(0), rd_u16(source + 0x0C));
    SET_W(D(1), rd_u16(source + 0x0E));
    SET_W(D(2), rd_u16(source + 0x06));
    SET_W(D(4), rd_u16(source + 0x08));
    D(3) = rd_u32(source + 0x10);
    d0 = D(0); d1 = D(1); d2 = D(2); d4 = D(4);
    d2 = cell_part(r, 0x06, 0x0C, d2, &d0);
    d4 = cell_part(r, 0x08, 0x0E, d4, &d1);
    d3 = D(3) - rd_u32(r + 0x10);
    if ((int32_t)d3 < 0) d3 = 0u - d3;
    D(0) = 0x7F00;
    D(1) = d1;
    D(2) = d2;
    D(3) = d3;
    D(4) = d4;
    if ((int32_t)d2 <= 0x7F00 && (int32_t)d3 <= 0x7F00 && (int32_t)d4 <= 0x7F00)
        magnitude_registers();
    return glue_return();
}
