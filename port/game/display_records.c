/* Display-record candidate preparation and pair selection ($C0D74A/$C0D752). */
#include "display_records.h"

#include "clip.h"
#include "globals.h"
#include "memory.h"

static int16_t rounded_product(int16_t x, int16_t y, int16_t z, gaddr row) {
    uint32_t sum = (uint32_t)((int32_t)x * rd_s16(row));
    sum += (uint32_t)((int32_t)y * rd_s16(row + 2));
    sum += (uint32_t)((int32_t)z * rd_s16(row + 4));
    return (int16_t)((int32_t)sum >> 8) + (int16_t)((sum & 0x80u) != 0);
}

static void prepare_candidates(gaddr matrix) {
    int16_t y = (int16_t)(rd_s16(POSITION_BIAS) >> 2);
    int i, row;
    for (i = 0; i < 4; ++i) {
        gaddr input = DISPLAY_CANDIDATE_INPUTS + (gaddr)(i * 4);
        gaddr out = CORNER_RECORDS + (gaddr)(i * 0x20);
        int16_t x = rd_s16(input), z = rd_s16(input + 2);
        for (row = 0; row < 3; ++row)
            wr_s16(out + (gaddr)(row * 2),
                   rounded_product(x, y, z, matrix + (gaddr)(row * 6)));
    }
    for (i = 0; i < 4; ++i) wr_u32(CROSSING_COUNTS + (gaddr)(i * 4), 0);
}

static void output_pair(gaddr *at, int16_t x, int16_t y) {
    wr_s16(*at, x);
    wr_s16(*at + 2, y);
    *at += 4;
}

static void relative_pairs(gaddr *at, int16_t first, int16_t second) {
    gaddr a = CORNER_SCREEN + (gaddr)(int32_t)(int16_t)(first * 8);
    gaddr b = CORNER_SCREEN + (gaddr)(int32_t)(int16_t)(second * 8);
    output_pair(at, (int16_t)(319 - rd_s16(a)), (int16_t)(179 - rd_s16(a + 2)));
    output_pair(at, (int16_t)(319 - rd_s16(b)), (int16_t)(179 - rd_s16(b + 2)));
}

static int select_pairs(void) {
    int16_t s[8], first = 0, second = 0;
    uint16_t flags = rd_u16(STATUS_CA);
    int branch, i;
    gaddr at;

    for (i = 0; i < 8; ++i) s[i] = rd_s16(CROSSING_COUNTS + (gaddr)(2 * i));
    if (s[1]) {
        if (s[3]) { first = s[5]; second = s[7]; branch = 1; }
        else if (s[0]) { first = s[5]; second = s[4]; branch = 2; }
        else if (s[2]) { first = s[5]; second = s[6]; branch = 3; }
        else goto reject;
    } else if (s[0]) {
        if (s[3]) { first = s[4]; second = s[7]; branch = 4; }
        else if (s[2]) { first = s[4]; second = s[6]; branch = 5; }
        else goto reject;
    } else if (s[2]) {
        if (s[3]) { first = s[6]; second = s[7]; branch = 6; }
        else goto reject;
    } else branch = 7;

    at = CORNER_RECORDS + 2;
    wr_u16(CORNER_RECORDS, (uint16_t)((branch == 1 || branch == 5 || branch == 7) ? 4 : 5));
    if (branch != 7) relative_pairs(&at, first, second);
    switch (branch) {
    case 1:
        output_pair(&at, 319, 179);
        output_pair(&at, 0, 179);
        if (rd_u8(CONTEXT_SELECT) || !(flags & 2u)) {
            at = CORNER_RECORDS + 10;
            output_pair(&at, 319, 0);
            output_pair(&at, 0, 0);
        }
        break;
    case 2:
        output_pair(&at, 319, 0);
        output_pair(&at, 319, 179);
        output_pair(&at, 0, 179);
        if (!(flags & 2u)) {
            wr_u16(CORNER_RECORDS, 3);
            at = CORNER_RECORDS + 10;
            output_pair(&at, 0, 0);
        }
        break;
    case 3:
        output_pair(&at, 319, 179);
        output_pair(&at, 319, 0);
        output_pair(&at, 0, 0);
        if (flags & 2u) {
            wr_u16(CORNER_RECORDS, 3);
            at = CORNER_RECORDS + 10;
            output_pair(&at, 0, 179);
        }
        break;
    case 4:
        output_pair(&at, 319, 179);
        output_pair(&at, 0, 179);
        output_pair(&at, 0, 0);
        if (!(flags & 2u)) {
            wr_u16(CORNER_RECORDS, 3);
            at = CORNER_RECORDS + 10;
            output_pair(&at, 319, 0);
        }
        break;
    case 5:
        output_pair(&at, 0, 179);
        output_pair(&at, 0, 0);
        if (rd_s16(DISPLAY_SELECTION_THRESHOLD) < 0x3840) {
            at = CORNER_RECORDS + 10;
            output_pair(&at, 319, 179);
            output_pair(&at, 319, 0);
        }
        break;
    case 6:
        output_pair(&at, 319, 0);
        output_pair(&at, 0, 0);
        output_pair(&at, 0, 179);
        if (flags & 2u) {
            wr_u16(CORNER_RECORDS, 3);
            at = CORNER_RECORDS + 10;
            output_pair(&at, 319, 179);
        }
        break;
    default:
        output_pair(&at, 0, 0);
        output_pair(&at, 319, 0);
        output_pair(&at, 319, 179);
        output_pair(&at, 0, 179);
        if (rd_s16(rd_u8(CONTEXT_SELECT) ? VIEW_PAN : DISPLAY_MODE_ZERO_THRESHOLD) <= 0x3840)
            goto reject;
        break;
    }
    wr_u16(DISPLAY_SELECTION_WORD_A, 1);
    wr_u16(DISPLAY_SELECTION_WORD_B, 1);
    wr_u32(DISPLAY_SELECTION_LONG, 0);
    wr_u8(DISPLAY_SELECTION_FLAG, 1);
    return 0;
reject:
    wr_u8(DISPLAY_SELECTION_FLAG, 0);
    return 1;
}

int prepare_display_records(int wide, const DisplayRecordHooks *hooks) {
    prepare_candidates(wide ? DISPLAY_CANDIDATE_MATRIX_WIDE : DISPLAY_CANDIDATE_MATRIX);
    if (hooks && hooks->project_edges) hooks->project_edges(hooks->context);
    else project_corner_edges();
    return select_pairs();
}
