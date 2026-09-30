/* Register flow through the two display-record selector entries. */
#include "glue.h"
#include "ports_glue.h"

#include "clip.h"
#include "display_records.h"
#include "globals.h"
#include "memory.h"

void corner_edges_registers(int apply_stack); /* glue_batch55.c */

static uint32_t swapped(uint32_t v) { return (v << 16) | (v >> 16); }

static void prefix_registers(int wide) {
    gaddr matrix = wide ? DISPLAY_CANDIDATE_MATRIX_WIDE : DISPLAY_CANDIDATE_MATRIX;
    int i;
    A(4) = matrix;
    A(1) = DISPLAY_CANDIDATE_INPUTS;
    A(3) = CORNER_RECORDS;
    for (i = 0; i < 4; ++i) {
        uint32_t sum;
        SET_W(D(2), rd_u16(A(1))); A(1) += 2;
        D(3) = swapped(rd_u32(POSITION_BIAS));
        SET_W(D(3), (uint16_t)((int16_t)D(3) >> 2));
        SET_W(D(4), rd_u16(A(1))); A(1) += 2;
        A(2) = A(4);
        SET_W(D(5), D(2)); SET_W(D(6), D(3)); SET_W(D(7), D(4));
        D(5) = (uint32_t)((int32_t)(int16_t)D(5) * rd_s16(A(2))); A(2) += 2;
        D(6) = (uint32_t)((int32_t)(int16_t)D(6) * rd_s16(A(2))); A(2) += 2;
        D(7) = (uint32_t)((int32_t)(int16_t)D(7) * rd_s16(A(2))); A(2) += 2;
        sum = D(7) + D(6) + D(5);
        D(7) = (uint32_t)((int32_t)sum >> 8);
        if (sum & 0x80u) SET_W(D(7), (uint16_t)(D(7) + 1));
        A(3) += 2;
        SET_W(D(5), D(2)); SET_W(D(6), D(3)); SET_W(D(7), D(4));
        D(5) = (uint32_t)((int32_t)(int16_t)D(5) * rd_s16(A(2))); A(2) += 2;
        D(6) = (uint32_t)((int32_t)(int16_t)D(6) * rd_s16(A(2))); A(2) += 2;
        D(7) = (uint32_t)((int32_t)(int16_t)D(7) * rd_s16(A(2))); A(2) += 2;
        sum = D(7) + D(6) + D(5);
        D(7) = (uint32_t)((int32_t)sum >> 8);
        if (sum & 0x80u) SET_W(D(7), (uint16_t)(D(7) + 1));
        A(3) += 2;
        D(2) = (uint32_t)((int32_t)(int16_t)D(2) * rd_s16(A(2))); A(2) += 2;
        D(3) = (uint32_t)((int32_t)(int16_t)D(3) * rd_s16(A(2))); A(2) += 2;
        D(4) = (uint32_t)((int32_t)(int16_t)D(4) * rd_s16(A(2))); A(2) += 2;
        sum = D(4) + D(3) + D(2);
        D(4) = (uint32_t)((int32_t)sum >> 8);
        if (sum & 0x80u) SET_W(D(4), (uint16_t)(D(4) + 1));
        A(3) += 2;
        SET_W(D(3), D(4));
        A(3) += 0x1Au;
    }
    A(4) = CROSSING_COUNTS + 12;
}

static void project_edges(void *context) {
    int wide = *(int *)context;
    prefix_registers(wide);
    project_corner_edges();
    corner_edges_registers(0);
}

static void selector_registers(int result) {
    int16_t s[8], first = 0, second = 0;
    int branch, i, extended = 0;
    gaddr base = CORNER_RECORDS;

    A(1) = CROSSING_COUNTS;
    for (i = 0; i < 8; ++i) s[i] = rd_s16(CROSSING_COUNTS + (gaddr)(2 * i));
    if (s[1]) {
        if (s[3]) { first = s[5]; second = s[7]; branch = 1; }
        else if (s[0]) { first = s[5]; second = s[4]; branch = 2; }
        else if (s[2]) { first = s[5]; second = s[6]; branch = 3; }
        else branch = 0;
    } else if (s[0]) {
        if (s[3]) { first = s[4]; second = s[7]; branch = 4; }
        else if (s[2]) { first = s[4]; second = s[6]; branch = 5; }
        else branch = 0;
    } else if (s[2]) {
        if (s[3]) { first = s[6]; second = s[7]; branch = 6; }
        else branch = 0;
    } else branch = 7;

    if (branch >= 1 && branch <= 6) {
        SET_W(D(0), first);
        SET_W(D(1), second);
    }
    if (branch) {
        A(1) = base + 2;
        if (branch == 1) {
            if (!rd_u8(CONTEXT_SELECT)) {
                SET_W(D(6), rd_u16(STATUS_CA));
                SET_W(D(6), (uint16_t)D(6) & 2u);
            }
            extended = rd_u8(CONTEXT_SELECT) || !(rd_u16(STATUS_CA) & 2u);
        } else if (branch == 2 || branch == 3 || branch == 4 || branch == 6) {
            SET_W(D(6), rd_u16(STATUS_CA));
            SET_W(D(6), (uint16_t)D(6) & 2u);
            extended = branch == 2 || branch == 4 ? !(rd_u16(STATUS_CA) & 2u) : (rd_u16(STATUS_CA) & 2u) != 0;
        } else if (branch == 5) extended = rd_s16(DISPLAY_SELECTION_THRESHOLD) < 0x3840;

        if (branch <= 6) {
            uint16_t x0, y0, x1, y1;
            SET_W(D(0), (uint16_t)(D(0) << 3));
            SET_W(D(1), (uint16_t)(D(1) << 3));
            A(0) = CORNER_SCREEN;
            x0 = rd_u16(A(0) + (gaddr)(int32_t)(int16_t)D(0));
            y0 = rd_u16(A(0) + (gaddr)(int32_t)(int16_t)D(0) + 2);
            x1 = rd_u16(A(0) + (gaddr)(int32_t)(int16_t)D(1));
            y1 = rd_u16(A(0) + (gaddr)(int32_t)(int16_t)D(1) + 2);
            SET_W(D(2), (uint16_t)(319 - x0));
            SET_W(D(3), (uint16_t)(179 - y0));
            SET_W(D(4), (uint16_t)(319 - x1));
            SET_W(D(5), (uint16_t)(179 - y1));
        } else {
            SET_W(D(0), rd_u16(rd_u8(CONTEXT_SELECT) ? VIEW_PAN : DISPLAY_MODE_ZERO_THRESHOLD));
        }
        if (branch == 2 || branch == 3 || branch == 4 || branch == 6)
            A(1) = base + (extended ? 14 : 22);
        else A(1) = base + 18;
    }
    D(0) = (uint32_t)result;
    flags_logic_l(D(0));
}

static int run(int wide) {
    DisplayRecordHooks hooks = {project_edges, &wide};
    int result = prepare_display_records(wide, &hooks);
    selector_registers(result);
    return glue_return();
}

int glue_C0D74A(void) {
    return run(1);
}

int glue_C0D752(void) {
    return run(0);
}
