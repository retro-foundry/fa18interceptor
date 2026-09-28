#include "projected_segment_clip.h"

#include <limits.h>

static int16_t add_word(int16_t left, int16_t right) {
    return (int16_t)((uint16_t)left + (uint16_t)right);
}

static int16_t sub_word(int16_t left, int16_t right) {
    return (int16_t)((uint16_t)left - (uint16_t)right);
}

static int16_t neg_word(int16_t value) {
    return (int16_t)(UINT16_C(0) - (uint16_t)value);
}

static int divs_word(int32_t dividend, int16_t divisor, int16_t *quotient) {
    int32_t result;
    if (!divisor) return -1;
    result = dividend / divisor;
    if (result < INT16_MIN || result > INT16_MAX) return -1;
    *quotient = (int16_t)result;
    return 0;
}

static int result_gate(int16_t d0, int16_t d1, int16_t d2) {
    int16_t d5 = d2;
    int16_t d6;

    /* `$C2F18E-$C2F1B6`. */
    if (d5 < 0) return 1;
    d6 = d2;
    if (d0 > d5 || neg_word(d0) > d5 || d1 > d6 || neg_word(d1) > d6)
        return 1;
    return 0;
}

int fa18_clip_projected_segment(const FA18ViewVertex endpoints[2],
                                FA18ProjectedSegmentClipEntry entry,
                                FA18ViewVertex *candidate, int *outside) {
    int16_t d0, d1, d2, d3, d4, d5, d6, quotient;
    int d3_second, d4_path;

    if (!endpoints || !candidate || !outside ||
        entry < FA18_PROJECTED_SEGMENT_CLIP_D3_FIRST_C2F0C6 ||
        entry > FA18_PROJECTED_SEGMENT_CLIP_D4_SECOND_C2F156)
        return -1;
    d0 = endpoints[1].x;
    d1 = endpoints[1].y;
    d2 = endpoints[1].depth;
    d3 = endpoints[0].x;
    d4 = endpoints[0].y;
    d5 = endpoints[0].depth;
    d3_second = entry == FA18_PROJECTED_SEGMENT_CLIP_D3_SECOND_C2F0F4;
    d4_path = entry == FA18_PROJECTED_SEGMENT_CLIP_D4_FIRST_C2F128 ||
              entry == FA18_PROJECTED_SEGMENT_CLIP_D4_SECOND_C2F156;

    if (!d4_path) {
        if (d3_second) {
            d0 = neg_word(d0);
            d3 = neg_word(d3);
        }
        d6 = sub_word(d2, d5);
        d0 = neg_word(sub_word(d2, d0));
        d3 = add_word(sub_word(d3, d5), d0);
        if (!d3) return -1; /* source's deliberate self-branch */
        d4 = neg_word(sub_word(d1, d4));
        if (divs_word((int32_t)d0 * d4, d3, &quotient) != 0) return -1;
        d1 = sub_word(d1, quotient);
        if (divs_word((int32_t)d0 * d6, d3, &quotient) != 0) return -1;
        d2 = sub_word(d2, quotient);
        d0 = d2;
        if (d3_second) d0 = neg_word(d0);
    } else {
        const int d4_second = entry == FA18_PROJECTED_SEGMENT_CLIP_D4_SECOND_C2F156;
        if (d4_second) {
            d1 = neg_word(d1);
            d4 = neg_word(d4);
        }
        d6 = sub_word(d2, d5);
        d1 = neg_word(sub_word(d2, d1));
        d4 = add_word(sub_word(d4, d5), d1);
        if (!d4) return -1; /* source's deliberate self-branch */
        d3 = neg_word(sub_word(d0, d3));
        if (divs_word((int32_t)d1 * d3, d4, &quotient) != 0) return -1;
        d0 = sub_word(d0, quotient);
        if (divs_word((int32_t)d1 * d6, d4, &quotient) != 0) return -1;
        d2 = sub_word(d2, quotient);
        d1 = d2;
        if (d4_second) d1 = neg_word(d1);
    }
    *candidate = (FA18ViewVertex){d0, d1, d2};
    *outside = result_gate(d0, d1, d2);
    return 0;
}
