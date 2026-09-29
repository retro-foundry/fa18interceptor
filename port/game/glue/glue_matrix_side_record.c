/* $C1342C: matrix-side record update register replay. */
#include "glue.h"
#include "ports_glue.h"

#include "control_records.h"
#include "fixed_math.h"
#include "globals.h"
#include "memory.h"

int glue_C1342C(void) {
    uint32_t saved_d2 = D(2), saved_d3 = D(3);
    gaddr saved_a2 = A(2), saved_a3 = A(3);
    int16_t index = rd_s16(MATRIX_SIDE_RECORD);
    gaddr record = CONTROL_RECORDS + (gaddr)(int32_t)(index * CONTROL_RECORD_BYTES);
    gaddr header = record + 2;
    uint16_t header_before = rd_u16(header);
    int16_t lane_z = rd_s8(record + 0x2A);
    int16_t before_y = rd_s16(record + 0x58);
    int16_t before_z = rd_s16(record + 0x5A);

    /* The original's MOVEM restores these four registers at its epilogue. */
    update_matrix_side_record();
    D(2) = saved_d2;
    D(3) = saved_d3;
    A(2) = saved_a2;
    A(3) = saved_a3;
    A(1) = record + 0x56;

    /* The zero-third-lane route loads this header into D1 before testing +$5A.
     * The nonzero helper routes below replace it with their live results. */
    D(1) = rd_u16(header);
    A(0) = record + 0x5A;

    /* With header bit 7 clear, a nonzero third lane ends through $C13C64.
     * Its preceding EXT.L/NEG.L supplies D0's high word; the helper stores
     * the un-nudged relaxation in D1. */
    if (!(rd_u8(record + 4) & 0x08) && lane_z != 0 && !(header_before & 0x80)) {
        gaddr table = (index == 0 || (header_before & 0x100))
                    ? MATRIX_SIDE_ZERO_TARGETS : MATRIX_SIDE_ALT_TARGETS;
        int16_t target = rd_s16(table + (gaddr)(2 * (lane_z < 0 ? -lane_z : lane_z)));
        int16_t used;
        int shift = rd_u8(record + 0x62) == 0x14 ? 2 : 1;
        int16_t eased;
        if (lane_z < 0) target = (int16_t)-target;
        used = five_eighths((int16_t)-target);
        if (rd_u8(record + 0x20) & 0x04) used = (int16_t)(used >> 1);
        eased = (int16_t)(before_z - (int16_t)((int16_t)(before_z - used) >> shift));
        D(0) = (uint32_t)(-(int32_t)target);
        SET_W(D(0), rd_u16(record + 0x5A));
        D(1) = (uint16_t)eased;
    } else if (!(rd_u8(record + 4) & 0x08) && lane_z != 0 && (header_before & 0x80) &&
               !(rd_u8(record + 4) & 0x02) && rd_s16(record + 0x6E) != 0) {
        gaddr table = (index == 0 || (header_before & 0x100))
                    ? MATRIX_SIDE_ZERO_TARGETS : MATRIX_SIDE_ALT_TARGETS;
        int16_t target, used, eased;
        int16_t angle = rd_s16(record + 0x6A);
        int use_5a_helper;
        if (lane_z < -10) lane_z = -10;
        else if (lane_z > 10) lane_z = 10;
        target = rd_s16(table + (gaddr)(2 * (lane_z < 0 ? -lane_z : lane_z)));
        if (lane_z < 0) target = (int16_t)-target;
        if (angle < 0x50 || angle > 0x7030 || rd_s16(record + 0x6E) <= 0x360 ||
            (angle < 0x3840 ? lane_z >= 0 : lane_z <= 0)) {
            used = five_eighths((int16_t)-target);
            if (rd_u8(record + 0x20) & 0x04) used = (int16_t)(used >> 1);
            eased = (int16_t)(before_z - (int16_t)((int16_t)(before_z - used) >>
                                                     (rd_u8(record + 0x62) == 0x14 ? 2 : 1)));
            D(1) = (uint16_t)eased;
            /* $C138CE/$C138F2/$C13918 call $C13C64 with -target.  The
             * helper's word operations preserve that argument's high word,
             * while its final D0 low word is the entry +$5A value. */
            use_5a_helper = (angle < 0x50 && rd_s16(record + 0x6E) > 0x360) ||
                             (angle < 0x3840 ? lane_z >= 0 : lane_z <= 0);
            if (use_5a_helper) {
                D(0) = (uint32_t)(-(int32_t)target);
                SET_W(D(0), (uint16_t)before_z);
            }
        } else {
            used = five_eighths(target);
            if (rd_u8(record + 0x20) & 0x04) used = (int16_t)(used >> 1);
            eased = (int16_t)(before_y - (int16_t)((int16_t)(before_y - used) >> 2));
            D(1) = (uint16_t)eased;
        }
    }
    /* $C13952 reaches $C13A2A for a zero third lane.  With bit 7 clear,
     * it decays the entry value.  With bit 7 set, $C13B5A first derives
     * +/-$20 from +$6A, then this helper leaves its +/-$10 result in D1. */
    if (lane_z == 0 || (rd_u8(record + 4) & 0x08)) {
        if ((header_before & 0x80) && rd_s16(record + 0x6A) != 0) {
            SET_W(D(1), rd_u16(record + 0x5A));
        } else if (!(header_before & 0x80) && (before_z < -15 || before_z > 15)) {
            SET_W(D(1), rd_u16(record + 0x5A));
        }
    }
    /* The no-gate path ends after the initial header load. */
    if (!(rd_u16(COCKPIT_FLAGS) & 0x40)) {
        uint32_t d0 = (uint32_t)(int32_t)index;
        d0 <<= 9;
        D(0) = (d0 & 0xFFFF0000u) | rd_u16(header);
        A(0) = header;
        D(1) = rd_u16(COCKPIT_FLAGS);
    } else if (index == 0 && rd_s32(MATRIX_SIDE_METRIC) != 0 &&
               (rd_u8(0xC461A4u) & 0x04)) {
        /* $C139C0 reloads the header into A0 before the index-zero status
         * tail returns through $C139CC or $C139FC. */
        A(0) = header;
    }
    return glue_return();
}
