/* Glue for the depth sort, record flags and pairs, view matrix, compass,
 * cockpit redraw and post-input context stage. */
#include "glue.h"
#include "ports_glue.h"

#include "cockpit.h"
#include "control_records.h"
#include "fixed_math.h"
#include "globals.h"
#include "memory.h"
#include "post_input.h"
#include "stages.h"

/* $C1E4A6: D7.w key count; saves and restores everything else. */
int glue_C1E4A6(void) {
    sort_by_depth((int16_t)D(7));
    return glue_return();
}

/* $C1CA82: leaves the flag byte in D7.b and the workspace records in A0. */
int glue_C1CA82(void) {
    flag_all_records();
    SET_B(D(7), 0x10);
    A(0) = WORKSPACE_RECORDS;
    return glue_return();
}

/* $C1EC3A: A1 entry; writes the pair into the caller's locals at -$1C(A6)
 * (high) and -$1E(A6) (low). */
int glue_C1EC3A(void) {
    int16_t high, low;
    read_record_pair(A(1), &high, &low);
    wr_s16(A(6) - 0x1C, high);
    wr_s16(A(6) - 0x1E, low);
    return glue_return();
}

/* sin_cos leftovers of y_rotation_matrix($C2E370) for `angle` (1/80 deg). */
static void rotation_leftovers(int16_t angle) {
    int16_t tenths = (int16_t)(angle >> 3), off = (int16_t)(tenths * 2);
    Fixed14 s, c;
    sin_cos(tenths, &s, &c);
    SET_W(D(4), -s);
    SET_W(D(5), c);
    SET_W(D(7), off);
    if (off < 0x708) {
        SET_W(D(6), 0x708 - off);
    } else if (off < 0xE10) {
        SET_W(D(6), 0x708);
        SET_W(D(7), off - 0x708);
    } else if (off < 0x1518) {
        SET_W(D(6), 0x1518 - off);
    } else {
        SET_W(D(6), 0x1518);
        SET_W(D(7), off - 0x1518);
    }
}

/* $C2DAF2: leaves the record in A0 and $C2E370's leftovers. */
int glue_C2DAF2(void) {
    gaddr record = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
    int16_t heading = rd_s16(record + 0x68);
    int16_t angle = heading ? (int16_t)(0x7080 - heading) : 0;

    update_view_matrix();

    rotation_leftovers(angle);
    SET_W(D(1), heading ? angle : 0x7080);
    A(0) = SINE_TABLE; /* sin_cos's table, left by $C2E370 */
    A(1) = VIEW_MATRIX + 18;
    return glue_return();
}

/* $C310AA: leaves the record base in A0, the record offset in D1.w and the
 * tape position in D0.w (high word: the product's bits 24-31, zero). */
int glue_C310AA(void) {
    update_compass();
    A(0) = CONTROL_RECORDS;
    SET_W(D(1), rd_u16(VIEW_RECORD));
    D(0) = (uint32_t)rd_u16(COMPASS_TAPE);
    return glue_return();
}

/* $C082B8/$C082B0: leave 3 in D4. */
int glue_C082B8(void) {
    request_cockpit_redraw();
    D(4) = 3;
    return glue_return();
}

int glue_C082B0(void) {
    finish_scene_setup();
    D(4) = 3;
    return glue_return();
}

/* $C11B0E: compiled C; leaves the last cleared address in A0. */
int glue_C11B0E(void) {
    gaddr table = rd_u32(LONG_TABLE);
    clear_long_table();
    A(0) = table + 60;
    return glue_return();
}
