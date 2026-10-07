#include "matrix.h"

#include "globals.h"

#include "control_records.h"
#include "fixed_math.h"

/* A 2.14 product, as the 68000 code keeps it: MULS then ASR.L #14. */
static int32_t mul14(int16_t x, int16_t y) {
    return ((int32_t)x * y) >> 14;
}

/* The high word of a 32-bit sum of 2.14 products, shifted down by 4: 2.8. */
static int16_t high8(int32_t sum) {
    return (int16_t)((int16_t)(sum >> 16) >> 4);
}

/* The high word of a 2.14 product, shifted down by 4 more: 2.8. */
static int16_t mul8(int16_t x, int16_t y) {
    return high8((int32_t)x * y);
}

void rotation_matrix(uint16_t a, uint16_t b, uint16_t c, gaddr out) {
    Fixed14 sa, ca, sb, cb, sc, cc;
    int16_t sb_sa, cb_sa;

    sin_cos((Angle)(a >> 3), &sa, &ca);
    sin_cos((Angle)(b >> 3), &sb, &cb);
    sin_cos((Angle)(c >> 3), &sc, &cc);
    sb_sa = (int16_t)mul14(sb, sa);
    cb_sa = (int16_t)mul14(cb, sa);

    wr_s16(out + 0, (int16_t)(((int32_t)cb * cc + (int32_t)sb_sa * sc) >> 14));
    wr_s16(out + 2, (int16_t)-(int16_t)mul14(ca, sc));
    wr_s16(out + 4, (int16_t)(((int32_t)sb * cc - (int32_t)cb_sa * sc) >> 14));
    wr_s16(out + 6, (int16_t)(((int32_t)cb * sc - (int32_t)sb_sa * cc) >> 14));
    wr_s16(out + 8, (int16_t)mul14(ca, cc));
    wr_s16(out + 10, (int16_t)(((int32_t)sb * sc + (int32_t)cb_sa * cc) >> 14));
    wr_s16(out + 12, (int16_t)-(int16_t)mul14(sb, ca));
    wr_s16(out + 14, (int16_t)-sa);
    wr_s16(out + 16, (int16_t)mul14(cb, ca));
}

void rotation_matrix8(uint16_t a, uint16_t b, uint16_t c, gaddr out) {
    Fixed14 sa, ca, sb, cb, sc, cc;
    int16_t sb_sa, cb_sa;

    sin_cos((Angle)(a >> 3), &sa, &ca);
    sin_cos((Angle)(b >> 3), &sb, &cb);
    sin_cos((Angle)(c >> 3), &sc, &cc);
    sb_sa = (int16_t)mul14(sb, sa);
    cb_sa = (int16_t)mul14(cb, sa);

    wr_s16(out + 0, high8((int32_t)cb * cc + (int32_t)sb_sa * sc));
    wr_s16(out + 2, (int16_t)-mul8(ca, sc));
    wr_s16(out + 4, high8((int32_t)sb * cc - (int32_t)cb_sa * sc));
    wr_s16(out + 6, high8((int32_t)cb * sc - (int32_t)sb_sa * cc));
    wr_s16(out + 8, mul8(ca, cc));
    wr_s16(out + 10, high8((int32_t)sb * sc + (int32_t)cb_sa * cc));
    wr_s16(out + 12, (int16_t)-mul8(sb, ca));
    wr_s16(out + 14, (int16_t)-(int16_t)(sa >> 6));
    wr_s16(out + 16, mul8(cb, ca));
}

void alternate_rotation_matrix(uint16_t a, uint16_t b, uint16_t c, gaddr out) {
    Fixed14 sa, ca, sb, cb, sc, cc;
    int16_t sc_sa, cc_sa;

    sin_cos((Angle)(a >> 3), &sa, &ca);
    sin_cos((Angle)(b >> 3), &sb, &cb);
    sin_cos((Angle)(c >> 3), &sc, &cc);
    sc_sa = (int16_t)mul14(sc, sa);
    cc_sa = (int16_t)mul14(cc, sa);

    wr_s16(out + 0, (int16_t)(((int32_t)cc * cb - (int32_t)sc_sa * sb) >> 14));
    wr_s16(out + 2, (int16_t)-(int16_t)(((int32_t)sc * cb + (int32_t)cc_sa * sb) >> 14));
    wr_s16(out + 4, (int16_t)mul14(ca, sb));
    wr_s16(out + 6, (int16_t)mul14(sc, ca));
    wr_s16(out + 8, (int16_t)mul14(cc, ca));
    wr_s16(out + 10, sa);
    wr_s16(out + 12, (int16_t)-(int16_t)(((int32_t)cc * sb + (int32_t)sc_sa * cb) >> 14));
    wr_s16(out + 14, (int16_t)(((int32_t)sc * sb - (int32_t)cc_sa * cb) >> 14));
    wr_s16(out + 16, (int16_t)mul14(ca, cb));
}

void two_angle_matrix(uint16_t a, uint16_t b, gaddr out) {
    Fixed14 sa, ca, sb, cb;

    sin_cos((Angle)(a >> 3), &sa, &ca);
    sin_cos((Angle)(b >> 3), &sb, &cb);

    wr_s16(out + 0, (int16_t)(cb >> 6));
    wr_s16(out + 2, 0);
    wr_s16(out + 4, (int16_t)(sb >> 6));
    wr_s16(out + 6, (int16_t)-mul8(sb, sa));
    wr_s16(out + 8, (int16_t)(ca >> 6));
    wr_s16(out + 10, mul8(cb, sa));
    wr_s16(out + 12, (int16_t)-mul8(sb, ca));
    wr_s16(out + 14, (int16_t)((int16_t)-sa >> 6));
    wr_s16(out + 16, mul8(cb, ca));
}

void scale_matrix_rows(gaddr matrix, gaddr scales) {
    int row, col;
    for (row = 0; row < 3; row++) {
        int16_t scale = rd_s16(scales + (gaddr)(2 * row));
        for (col = 0; col < 3; col++) {
            gaddr cell = matrix + (gaddr)(6 * row + 2 * col);
            wr_s16(cell, (int16_t)(((int32_t)rd_s16(cell) * scale) >> 8));
        }
    }
}

uint32_t build_transform_product(gaddr source, uint16_t a, uint16_t b, uint16_t c) {
    int row, col, k;
    uint32_t last_term = 0;
    uint16_t angles[3] = {a, b, c};
    for (k = 0; k < 3; ++k)
        if ((int16_t)angles[k] < 0) angles[k] = (uint16_t)(angles[k] + 0x7080u);
    rotation_matrix(angles[0], angles[1], angles[2], MATRIX_TRANSFORM_ROTATION);
    for (row = 0; row < 3; ++row) {
        for (col = 0; col < 3; ++col) {
            uint32_t sum = 0;
            for (k = 0; k < 3; ++k) {
                int32_t term = (int32_t)rd_s16(source + (gaddr)(6 * k + 2 * col)) *
                               rd_s16(MATRIX_TRANSFORM_ROTATION + (gaddr)(6 * row + 2 * k));
                sum += (uint32_t)term;
                if (row == 2 && col == 2 && k == 2) last_term = (uint32_t)term;
            }
            wr_u32(MATRIX_TRANSFORM_PRODUCT + (gaddr)(12 * row + 4 * col), sum);
        }
    }
    return last_term;
}

/* The routine uses DIVS.W without rounding for a small divisor. For a larger
 * divisor it divides the numerator shifted six places, then rounds the word
 * quotient using the signed remainder. On overflow DIVS leaves D0 intact. */
static uint32_t transform_ratio(int32_t numerator, int16_t *divisor) {
    int32_t dividend = numerator;
    const int rounded = *divisor > 0x147;
    int64_t quotient;
    uint32_t packed;
    /* C2E0EA and the corresponding alternate-axis sites scale D3 itself.
     * Subsequent ratios must use that retained divisor. */
    if (!rounded) *divisor = (int16_t)((uint16_t)*divisor << 6);
    else dividend >>= 6;
    quotient = (int64_t)dividend / *divisor;
    if (quotient < -32768 || quotient > 32767)
        packed = (uint32_t)dividend;
    else
        packed = ((uint32_t)(uint16_t)(int16_t)(dividend % *divisor) << 16) |
                 (uint16_t)(int16_t)quotient;
    if (rounded) {
        int16_t remainder = (int16_t)(packed >> 16);
        int16_t half = (int16_t)((*divisor < 0 ? -(int32_t)*divisor : *divisor) >> 1);
        if (remainder < 0) remainder = (int16_t)-remainder;
        packed = ((uint32_t)(uint16_t)remainder << 16) | (uint16_t)packed;
        if (half <= remainder)
            packed = (packed & 0xFFFF0000u) |
                     (uint16_t)((int16_t)packed < 0 ? (int16_t)packed - 1
                                                  : (int16_t)packed + 1);
    }
    return packed;
}

static int16_t transform_table_angle(int16_t index) {
    return rd_s16(MATRIX_ANGLE_TABLE_FINE + (gaddr)(int32_t)index);
}

static int16_t transform_axis_angle(int32_t main, int32_t companion,
                                    int16_t *divisor, uint32_t *last_d0,
                                    int16_t *raw_angle) {
    uint32_t packed = transform_ratio((int32_t)(0u - (uint32_t)main), divisor);
    int16_t index = (int16_t)((uint16_t)packed << 1);
    int negative = index < 0;
    int16_t angle, other;
    if (index >= 0) {
        if (index <= 0x180) angle = transform_table_angle(index);
        else {
            packed = transform_ratio(companion, divisor);
            other = (int16_t)((uint16_t)packed << 1);
            if (other < 0) other = (int16_t)-other;
            angle = (int16_t)(0x384 - transform_table_angle(other));
        }
    } else {
        index = (int16_t)-index;
        if (index <= 0x180) angle = (int16_t)(0xE10 - transform_table_angle(index));
        else {
            packed = transform_ratio(companion, divisor);
            other = (int16_t)((uint16_t)packed << 1);
            if (other < 0) other = (int16_t)-other;
            angle = (int16_t)(0xA8C + transform_table_angle(other));
        }
    }
    if (last_d0) {
        int16_t final_index = (int16_t)((uint16_t)packed << 1);
        if (index > 0x180) {
            if (final_index < 0) final_index = (int16_t)-final_index;
        } else if (negative) final_index = (int16_t)-final_index;
        *last_d0 = (packed & 0xFFFF0000u) | (uint16_t)final_index;
    }
    if (companion < 0) {
        if (main >= 0 && angle > 0x708) angle = (int16_t)(0x1518 - angle);
        else angle = (int16_t)(0x708 - angle);
    }
    if (raw_angle) *raw_angle = angle;
    return angle < 0 ? (int16_t)-angle : angle;
}

void extract_transform_angles(int16_t out[3], MatrixTransformAngleState *state) {
    int32_t first = (int32_t)(0u - rd_u32(MATRIX_TRANSFORM_PRODUCT + 28));
    uint32_t magnitude = first < 0 ? 0u - (uint32_t)first : (uint32_t)first;
    int16_t index, value, angle, divisor;
    gaddr table;
    if ((int32_t)magnitude < 0x0E210000) {
        uint16_t high = (uint16_t)((uint32_t)first >> 16);
        index = (int16_t)((int16_t)high >> 4);
        if (high & 8u) index = (int16_t)(index + 1);
        table = MATRIX_ANGLE_TABLE_FINE;
    } else {
        uint32_t shifted;
        if (magnitude >= 0x0FF60000u) {
            table = MATRIX_ANGLE_TABLE_COARSE;
            shifted = (magnitude >> 12) + ((magnitude & 0x800u) != 0);
            /* C2E042/C2E048 subtract and clamp as signed longs. Narrowing
             * first turns large products into negative table offsets. */
            int32_t coarse_index = (int32_t)(shifted - 0xFF08u);
            index = coarse_index > 0xF8 ? 0xF8 : (int16_t)coarse_index;
        } else {
            table = MATRIX_ANGLE_TABLE_MID;
            shifted = (magnitude >> 16) + ((magnitude & 0x8000u) != 0);
            index = (int16_t)(shifted - 0xDDBu);
            if (index > 0x225) index = 0x225;
        }
        if (first < 0) index = (int16_t)-index;
    }
    index = (int16_t)((uint16_t)index << 1);
    if (index >= 0) {
        value = rd_s16(table + (gaddr)(int32_t)index);
        angle = value;
    } else {
        value = rd_s16(table + (gaddr)(int32_t)(int16_t)-index);
        angle = (int16_t)(0xE10 - value);
    }
    divisor = (int16_t)((uint16_t)(0x384 - value) << 1);
    divisor = rd_s16(SINE_TABLE + (gaddr)(int32_t)divisor);
    if (divisor < 0x8F) {
        angle = index < 0 ? 0xA82 : 0x38E;
        divisor = (int16_t)0xFEE2;
    }
    if (state) {
        state->primary_index = index;
        state->primary_clears_d2_high = (int32_t)magnitude >= 0x0E210000;
    }
    out[0] = (int16_t)((uint16_t)angle << 3);
    out[1] = (int16_t)((uint16_t)transform_axis_angle(
        rd_s32(MATRIX_TRANSFORM_PRODUCT + 24),
        rd_s32(MATRIX_TRANSFORM_PRODUCT + 32), &divisor, 0,
        state ? &state->secondary_raw : 0) << 3);
    {
        uint32_t final_d0;
        out[2] = (int16_t)((uint16_t)transform_axis_angle(
        rd_s32(MATRIX_TRANSFORM_PRODUCT + 4),
        rd_s32(MATRIX_TRANSFORM_PRODUCT + 16), &divisor, &final_d0, 0) << 3);
        if (state) { state->final_d0 = final_d0; state->divisor = divisor; }
    }
}

static uint32_t replace_low_word(uint32_t value, int16_t word) {
    return (value & 0xFFFF0000u) | (uint16_t)word;
}

static void matrix_depth_sin_cos(int16_t angle, uint32_t *d6, Fixed14 *sine,
                                 Fixed14 *cosine) {
    int16_t offset = (int16_t)(angle + angle);

    sin_cos(angle, sine, cosine);
    if (offset < 0x708) *d6 = replace_low_word(*d6, (int16_t)(0x708 - offset));
    else if (offset < 0x1518) *d6 = replace_low_word(*d6, 0x708);
    else *d6 = replace_low_word(*d6, (int16_t)(0x1518 - offset));
}

void adjust_matrix_record_depth(gaddr record, uint32_t d3, uint32_t d5,
                                uint32_t d6, uint32_t d7,
                                MatrixDepthAdjustment *out) {
    int16_t d4, d1, scale;
    Fixed14 sine, cosine;

    if (rd_u8(record) & 0x80) {
        out->d3 = d3 & 0xFFFF0000u;
        out->d5 = d5 & 0xFFFF0000u;
        out->d6 = d6;
        out->d7 = d7;
        if (rd_s16(MATRIX_SIDE_RECORD) == 0) wr_u16(LOAD_TRIM, 0);
        return;
    }

    d4 = (int16_t)((int16_t)d7 >> 3);
    if (d4 < 0x384) {
        settle_record(record);
        matrix_depth_sin_cos(d4, &d6, &sine, &cosine);
        d4 = sine;
        d5 = replace_low_word(d5, cosine);
    } else if (d4 < 0x708) {
        mark_record_pending(record);
        d4 = (int16_t)(0x708 - d4);
        matrix_depth_sin_cos(d4, &d6, &sine, &cosine);
        d4 = sine;
        d5 = replace_low_word(d5, (int16_t)-cosine);
    } else if (d4 < 0xA8C) {
        mark_record_pending(record);
        d4 = (int16_t)(0xE10 - d4);
        matrix_depth_sin_cos(d4, &d6, &sine, &cosine);
        d4 = sine;
        d5 = replace_low_word(d5, (int16_t)-cosine);
    } else {
        settle_record(record);
        d4 = (int16_t)(d4 - 0x708);
        matrix_depth_sin_cos(d4, &d6, &sine, &cosine);
        d4 = sine;
        d5 = replace_low_word(d5, cosine);
    }

    wr_s16(record + 0x22, d4);
    wr_u16(record + 0x24, (uint16_t)d5);

    d3 = replace_low_word(d3, d4);
    if (d4 < 0) d4 = (int16_t)-d4;
    d1 = rd_s16(record + 0x54);
    d7 = replace_low_word(d7, d1);
    if ((int16_t)d5 < 0) d4 = (int16_t)-d4;
    d1 = (int16_t)(d1 - d4);
    if (d1 != 0) {
        if (d1 > 0) d1 = d1 > 7 ? (int16_t)(d1 >> 3) : 1;
        else d1 = d1 < -7 ? (int16_t)(d1 >> 3) : -1;
    }
    d7 = replace_low_word(d7, (int16_t)((int16_t)d7 - d1));
    wr_u16(record + 0x54, (uint16_t)d7);

    d4 = (int16_t)d7;
    if (d4 < 0) {
        d4 = (int16_t)-d4;
        if ((int16_t)d5 < 0) {
            wr_u16(record + 0x52, rd_u16(record + 0x50));
        } else if ((int16_t)d5 >= 0x100) {
            d6 = replace_low_word(d6, rd_s16(record + 0x50));
            d7 = replace_low_word(d7, rd_s16(record + 0x52));
            if (((int16_t)d6 ^ (int16_t)d7) >= 0) {
                d3 = replace_low_word(d3, (int16_t)-(int16_t)d3);
                d5 = replace_low_word(d5, (int16_t)-(int16_t)d5);
            }
        }
    } else if ((int16_t)d5 > 0) {
        wr_u16(record + 0x52, rd_u16(record + 0x50));
    } else if ((int16_t)d5 <= -0x100) {
        d6 = replace_low_word(d6, rd_s16(record + 0x50));
        d7 = replace_low_word(d7, rd_s16(record + 0x52));
        if (((int16_t)d6 ^ (int16_t)d7) >= 0) {
            d3 = replace_low_word(d3, (int16_t)-(int16_t)d3);
            d5 = replace_low_word(d5, (int16_t)-(int16_t)d5);
        }
    }

    if (d4 > rd_s16(record + 0x7E)) d4 = rd_s16(record + 0x7E);
    d4 = (int16_t)(d4 - (int16_t)(d4 >> 2));
    d3 = (uint32_t)((((-(int32_t)(int16_t)d3 * 128) >> 13) * d4) >> 13);
    d5 = (uint32_t)((((-(int32_t)(int16_t)d5 * 128) >> 13) * d4) >> 13);

    d1 = rd_s16(record + 0x66);
    if (d1 >= 0x3840) d1 = (int16_t)(0x7080 - d1);
    if (d1 > 0xE10) {
        d3 &= 0xFFFF0000u;
        d5 &= 0xFFFF0000u;
    } else {
        d1 = (int16_t)((int16_t)(d1 >> 8) * 2);
        scale = rd_s16(MATRIX_DEPTH_SCALE_TABLE + (gaddr)d1);
        d3 = (uint32_t)((int32_t)(int16_t)d3 * scale >> 8);
        d5 = (uint32_t)((int32_t)(int16_t)d5 * scale >> 8);
    }
    out->d3 = d3;
    out->d5 = d5;
    out->d6 = d6;
    out->d7 = 13;
    if (rd_s16(MATRIX_SIDE_RECORD) == 0) wr_u16(LOAD_TRIM, (uint16_t)d3);
}

/* A full turn in native angle units. */
#define FULL_TURN 0x7080

static uint16_t negated_angle(uint16_t angle) {
    return angle ? (uint16_t)(FULL_TURN - angle) : 0;
}

void inverse_orientation_matrix(gaddr record, uint16_t x, uint16_t y, uint16_t z) {
    alternate_rotation_matrix(negated_angle(x), negated_angle(y), negated_angle(z), record + 0x92);
}

void set_record_orientation(gaddr record, uint16_t x, uint16_t y, uint16_t z) {
    wr_u16(record + 0x66, x);
    wr_u16(record + 0x68, y);
    wr_u16(record + 0x6A, z);
    rotation_matrix(x, y, z, record + 0x80);
    inverse_orientation_matrix(record, x, y, z);
}

void local_to_world(gaddr record, gaddr matrix, int16_t x, int16_t y, int16_t z, int32_t out[3]) {
    int row;
    for (row = 0; row < 3; row++) {
        gaddr m = matrix + (gaddr)(6 * row);
        /* Original ADD.L wraps before ASR.L; three signed products can
         * overflow even though each MULS.W product fits in a long. */
        uint32_t sum = (uint32_t)((int32_t)x * rd_s16(m)) +
                       (uint32_t)((int32_t)y * rd_s16(m + 2)) +
                       (uint32_t)((int32_t)z * rd_s16(m + 4));
        out[row] = (int32_t)((uint32_t)((int32_t)sum >> 4) +
                   rd_u32(record + RECORD_POSITION + (gaddr)(4 * row)));
    }
}
