#include "matrix.h"

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
