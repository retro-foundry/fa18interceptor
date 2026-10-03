/* Glue for local_to_world ($C091E0, $C091CE, $C091A8), the shown-record
 * vertices $C0D334 and the slot scan $C265E8. */
#include "glue.h"
#include "ports_glue.h"

#include "fixed_math.h"
#include "globals.h"
#include "matrix.h"
#include "memory.h"
#include "vertex_tail.h"
#include "control_records.h"

void magnitude_registers(void);   /* glue_batch22.c */
void vertex_tail_registers(void); /* glue_batch20.c */

/* The transform's leftovers: D0-D2 the result, D3 = x * m6, D5 = z * m8,
 * D6 = x * m3, D7 = z * m5 (full products); D4 is only read. */
void world_registers(gaddr record, gaddr matrix) {
    int16_t x = (int16_t)D(3), y = (int16_t)D(4), z = (int16_t)D(5);
    int32_t out[3];
    local_to_world(record, matrix, x, y, z, out);
    D(0) = (uint32_t)out[0];
    D(1) = (uint32_t)out[1];
    D(2) = (uint32_t)out[2];
    D(3) = (uint32_t)((int32_t)x * rd_s16(matrix + 12));
    D(5) = (uint32_t)((int32_t)z * rd_s16(matrix + 16));
    D(6) = (uint32_t)((int32_t)x * rd_s16(matrix + 6));
    D(7) = (uint32_t)((int32_t)z * rd_s16(matrix + 10));
}

static gaddr viewed_record(void) {
    return CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
}

/* $C091E0: A1 record, D3-D5.w point, its inverse orientation. */
int glue_C091E0(void) {
    world_registers(A(1), A(1) + RECORD_INVERSE);
    return glue_return();
}

/* $C091CE: the viewed record's inverse orientation. */
int glue_C091CE(void) {
    gaddr record = viewed_record();
    world_registers(record, record + RECORD_INVERSE);
    return glue_return();
}

/* $C091A8: rebuilds VIEW_MATRIX first; D3-D5 come back sign-extended from
 * the MOVEM.W that saved them. */
int glue_C091A8(void) {
    D(3) = (uint32_t)(int32_t)(int16_t)D(3);
    D(4) = (uint32_t)(int32_t)(int16_t)D(4);
    D(5) = (uint32_t)(int32_t)(int16_t)D(5);
    update_view_matrix();
    world_registers(viewed_record(), VIEW_MATRIX);
    return glue_return();
}

/* $C0D334: A2 stream. Leaves the second workspace's vertex-tail registers,
 * D0 = 0 (MOVEQ's flags), D1/D2 = its +$62/+$64 and D3-D5 the midpoint
 * (high words from the +$72 triple the MOVEM.W loaded), A3 the workspace. */
int glue_C0D334(void) {
    gaddr w = WORKSPACES + (gaddr)(int32_t)rd_s16(A(2));
    int16_t hx = rd_s16(w + 0x72), hy = rd_s16(w + 0x74), hz = rd_s16(w + 0x76);
    A(2) = derive_shown_vertices(A(2));
    A(3) = w;
    vertex_tail_registers();
    D(1) = (uint32_t)(int32_t)rd_s16(w + 0x62);
    D(2) = (uint32_t)(int32_t)rd_s16(w + 0x64);
    D(3) = ((uint32_t)(int32_t)hx & 0xFFFF0000u) | rd_u16(w + 0x294);
    D(4) = ((uint32_t)(int32_t)hy & 0xFFFF0000u) | rd_u16(w + 0x296);
    D(5) = ((uint32_t)(int32_t)hz & 0xFFFF0000u) | rd_u16(w + 0x298);
    D(0) = 0;
    flags_logic_l(0);
    return glue_return();
}

/* |a - b| >> 8 as the long arithmetic leaves it. */
static uint32_t part(int32_t a, int32_t b) {
    int32_t d = a - b;
    if (d < 0) d = -d;
    return (uint32_t)(d >> 8);
}

/* $C265E8: every register is live after it. */
int glue_C265E8(void) {
    int i, result = flagged_slot_in_range();
    for (i = SLOT_COUNT - 1; i >= 0; i--) {
        gaddr slot = SLOT_TABLE + (gaddr)(SLOT_SIZE * i), record;
        uint16_t first;
        int32_t offset;
        if (!(rd_u8(slot + 0x27) & 0x01)) continue;
        A(0) = slot;
        if (!(rd_u8(slot + 0x26) & 0x20) && !rd_u8(CONTEXT_SELECT)) {
            SET_W(D(1), (uint16_t)(i << 6));
            D(0) = 0;
            return glue_return();
        }
        record = CONTROL_RECORDS + (gaddr)(int32_t)(int16_t)(rd_s16(slot + 0x2E) << 9);
        A(1) = record;
        A(2) = OBSERVER;
        D(2) = part(rd_s32(record + 0x14), rd_s32(OBSERVER + 0x0C));
        D(3) = part(rd_s32(record + 0x18), rd_s32(OBSERVER + 0x10));
        D(4) = part(rd_s32(record + 0x1C), rd_s32(OBSERVER + 0x14));
        magnitude_registers();
        first = (uint16_t)D(1);
        offset = (int32_t)((uint32_t)(int32_t)rd_s16(slot + 0x30) << 22);
        D(2) = part(rd_s32(slot) + offset, rd_s32(OBSERVER + 0x0C));
        D(3) = part(rd_s32(slot + 4), rd_s32(OBSERVER + 0x10));
        D(5) = (uint32_t)(int32_t)((uint32_t)(int32_t)rd_s16(slot + 0x32) << 22);
        D(4) = part(rd_s32(slot + 8) + (int32_t)D(5), rd_s32(OBSERVER + 0x14));
        magnitude_registers();
        SET_W(D(2), first);
        D(0) = (uint32_t)result;
        return glue_return();
    }
    A(0) = SLOT_TABLE;
    SET_W(D(1), 0);
    D(0) = 0;
    return glue_return();
}
