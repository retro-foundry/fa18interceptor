/* Fixed-point trigonometry. */
#include "fixed_math.h"

#include "audio.h"
#include "globals.h"
#include "memory.h"

/* sin(i / 10 degrees) for i = 0..900, 2.14 fixed point. */
static Fixed14 quarter_sine(int16_t tenths) {
    return rd_s16(SINE_TABLE + (gaddr)(int32_t)(int16_t)(tenths * 2));
}

void sin_cos(Angle angle, Fixed14 *sine, Fixed14 *cosine) {
    if (angle < 900) {
        *sine = quarter_sine(angle);
        *cosine = quarter_sine((int16_t)(900 - angle));
    } else if (angle < 1800) {
        *sine = quarter_sine((int16_t)(1800 - angle));
        *cosine = (Fixed14)-quarter_sine((int16_t)(angle - 900));
    } else if (angle < 2700) {
        *sine = (Fixed14)-quarter_sine((int16_t)(angle - 1800));
        *cosine = (Fixed14)-quarter_sine((int16_t)(2700 - angle));
    } else {
        *sine = (Fixed14)-quarter_sine((int16_t)(3600 - angle));
        *cosine = quarter_sine((int16_t)(angle - 2700));
    }
}

int16_t attenuate_offset(int16_t x, int16_t y) {
    int16_t sum = (int16_t)(x + y);
    int16_t size = (int16_t)(sum < 0 ? -sum : sum);

    if (size > 4) sum = (int16_t)(sum >> 2);
    else if (size > 2) sum = (int16_t)(sum >> 1);
    return (int16_t)(sum - y);
}

int16_t rounded_divide(int32_t dividend, int16_t divisor) {
    int16_t half = (int16_t)((int16_t)(divisor < 0 ? -divisor : divisor) >> 1);
    int32_t q = dividend / divisor;
    int16_t quotient, remainder;

    if (q >= -32768 && q <= 32767) {
        quotient = (int16_t)q;
        remainder = (int16_t)(dividend % divisor);
    } else {
        quotient = (int16_t)dividend;
        remainder = (int16_t)(dividend >> 16);
    }
    if (remainder < 0) remainder = (int16_t)-remainder;
    if (half > remainder) return quotient;
    return (int16_t)(quotient < 0 ? quotient - 1 : quotient + 1);
}

void divide_rounded(void) {
    wr_s16(DIVIDE_QUOTIENT, rounded_divide(rd_s32(DIVIDE_NUMERATOR), rd_s16(DIVIDE_DENOMINATOR)));
}

void y_rotation_matrix(int16_t angle, gaddr out) {
    Fixed14 s, c;
    int16_t words[9];
    int i;
    sin_cos((int16_t)(angle >> 3), &s, &c);
    words[0] = c; words[1] = 0; words[2] = s;
    words[3] = 0; words[4] = FIXED14_ONE; words[5] = 0;
    words[6] = (int16_t)-s; words[7] = 0; words[8] = c;
    for (i = 0; i < 9; i++) wr_s16(out + (gaddr)(2 * i), words[i]);
}

/* Arithmetic right shift of a word by 0-63 places, as ASR.W does. */
static int16_t asr_word(int16_t v, int shift) {
    shift &= 63;
    if (shift >= 16) return (int16_t)(v < 0 ? -1 : 0);
    return (int16_t)(v >> shift);
}

void decay_toward_zero(gaddr value, int16_t shift) {
    int16_t v = rd_s16(value);
    if (v < -15) wr_s16(value, (int16_t)(v + asr_word((int16_t)-v, shift)));
    else if (v > 15) wr_s16(value, (int16_t)(v - asr_word(v, shift)));
    else if (v < 0) wr_s16(value, (int16_t)(v + 1));
    else wr_s16(value, (int16_t)(v - 1));
}

int16_t five_eighths(int16_t x) { return (int16_t)((x >> 1) + (x >> 3)); }

int32_t random_bit(void) {
    uint32_t seed = rd_u32(RANDOM_SEED);
    int32_t bit = (int32_t)((seed ^ ((int32_t)seed >> 3)) & 1);
    seed = ((uint32_t)((int32_t)seed >> 1)) & 0x7FFFFFFFu;
    if (bit) seed += 0x80000000u;
    wr_u32(RANDOM_SEED, seed);
    return bit;
}

void decay_outside_limit(gaddr value, int16_t limit, int16_t shift) {
    int32_t v = rd_s16(value), lim = limit;
    if (v > lim || v < -lim) {
        int16_t w = (int16_t)v;
        wr_s16(value, (int16_t)(w - asr_word(w, shift)));
    } else {
        wr_u16(value, 0);
    }
}

void y_rotation_matrix8(int16_t angle, gaddr out) {
    Fixed14 s, c;
    int16_t words[9];
    int i;
    sin_cos((int16_t)(angle >> 3), &s, &c);
    words[0] = (int16_t)(c >> 6); words[1] = 0; words[2] = (int16_t)(s >> 6);
    words[3] = 0; words[4] = 0x100; words[5] = 0;
    words[6] = (int16_t)-(s >> 6); words[7] = 0; words[8] = (int16_t)(c >> 6);
    for (i = 0; i < 9; i++) wr_s16(out + (gaddr)(2 * i), words[i]);
}

int32_t cell_step(int16_t cells) { return (int32_t)((uint32_t)(uint16_t)cells << 16) >> 2; }

int32_t random_bits(int32_t count) {
    int32_t bits = 0;
    while (count-- > 0) bits = bits * 2 + random_bit();
    return bits;
}

/* DIVU.W quotient: on overflow the dividend's low word stays. */
static uint16_t divu_word(uint32_t dividend, uint16_t divisor) {
    uint32_t q = dividend / divisor;
    return (uint16_t)(q > 0xFFFF ? dividend : q);
}

static uint16_t newton_sqrt(uint32_t x) {
    uint16_t guess = (uint16_t)(divu_word(x, 200) + 2);
    for (;;) {
        uint16_t q;
        if (guess == 0) return 0; /* the original would take a divide-by-zero trap */
        q = divu_word(x, guess);
        int16_t diff = (int16_t)(q - guess);
        if (diff >= -1 && diff <= 1) return q;
        guess = (uint16_t)((uint16_t)(guess + q) >> 1);
    }
}

void square_root(void) {
    uint32_t x = rd_u32(SQRT_INPUT);
    uint16_t root;
    if ((int32_t)x <= 0x63F000) root = newton_sqrt(x);
    else if ((int32_t)x <= 0x63F0000) root = (uint16_t)(newton_sqrt(x >> 4) << 2);
    else root = (uint16_t)(newton_sqrt(x >> 8) << 4);
    wr_u16(SQRT_RESULT, root);
}

int32_t long_divide(int32_t dividend, int32_t divisor, int32_t *remainder) {
    uint32_t n, d, q, r;
    if (divisor == 0 || dividend == 0) {
        *remainder = 0;
        return 0;
    }
    n = dividend < 0 ? 0u - (uint32_t)dividend : (uint32_t)dividend;
    d = divisor < 0 ? 0u - (uint32_t)divisor : (uint32_t)divisor;
    q = n / d;
    r = n % d;
    if ((dividend ^ divisor) < 0) q = 0u - q;
    if (dividend < 0) r = 0u - r;
    *remainder = (int32_t)r;
    return (int32_t)q;
}

/* 68000 DIVU.W: quotient in the low word, remainder in the high word; on
 * overflow the dividend is left as it was. */
static uint32_t divu_w(uint32_t dividend, uint16_t divisor) {
    uint32_t q = dividend / divisor;
    if (q > 0xFFFF) return dividend;
    return (dividend % divisor) << 16 | q;
}

/* sqrt(1 + (small/large)^2) from the table, for a table index made from the
 * ratio. */
static uint16_t magnitude_factor(int16_t index) {
    return rd_u16(MAGNITUDE_TABLE + (gaddr)(int32_t)(int16_t)(index * 2));
}

int32_t magnitude3(int16_t x, int16_t y, int16_t z) {
    int16_t large = x, small = y, index = 0;
    int32_t planar, height, big, other, result;

    if (small > large) {
        large = y;
        small = x;
    }
    if (small != 0 && large != 0)
        index = (int16_t)divu_w((uint32_t)((int32_t)small << 8), (uint16_t)large);
    planar = (int32_t)((uint32_t)(uint16_t)large * magnitude_factor(index));

    height = (int32_t)z << 14;
    big = planar;
    other = height;
    if (height > planar) {
        big = height;
        other = planar;
    }
    big >>= 14;
    index = 0;
    if ((int16_t)big != 0)
        index = (int16_t)((int16_t)divu_w((uint32_t)other, (uint16_t)big) >> 6);
    result = (int32_t)((uint32_t)magnitude_factor(index) * (uint16_t)big) >> 14;
    if (result > 0x7FFF) result = (int32_t)(((uint32_t)result & 0xFFFF0000u) | 0x7FFF);
    wr_u16(MAGNITUDE, (uint16_t)result);
    return result;
}

/* NEG.W when negative: -32768 stays. */
static int16_t asr16(int16_t v, int count) {
    count &= 63;
    return count >= 16 ? (int16_t)(v < 0 ? -1 : 0) : (int16_t)(v >> count);
}

int32_t target_distance(int16_t x, int16_t y, int16_t z) {
    int shift = rd_s16(BOUND_SHIFT) & 63;
    int16_t dx = (int16_t)(x + asr16(rd_s16(PROJECTION_WORDS), shift));
    int16_t dz = (int16_t)(z + asr16(rd_s16(PROJECTION_WORDS + 4), shift));
    int32_t dy;

    if (dx < 0) dx = (int16_t)-dx;
    if (rd_u8(POSITION_VALID)) dy = rd_s32(POSITION_LEVEL) >> 8;
    else dy = (int32_t)((uint32_t)(int32_t)y + (uint32_t)(shift >= 32 ? (rd_s32(PROJECTION_Y) < 0 ? -1 : 0)
                                                                     : rd_s32(PROJECTION_Y) >> shift));
    if (dy < 0) dy = (int32_t)(0u - (uint32_t)dy);
    if (dy >= 0x7FFF0) {
        wr_u16(MAGNITUDE, 0x7FFF);
        return 0x7FFF;
    }
    if (dz < 0) dz = (int16_t)-dz;
    return magnitude3((int16_t)(dx >> 4), (int16_t)(dy >> 4), (int16_t)(dz >> 4));
}

static int16_t abs16(int16_t v) {
    return v < 0 ? (int16_t)-v : v;
}

void normalize_vector(int32_t scale, int32_t x, int32_t y, int32_t z) {
    int16_t size = (int16_t)scale, rx = 0, ry = 0, rz = 0;
    int32_t length = 0, factor;
    int shift;

    if (size != 0) length = magnitude3((int16_t)abs16((int16_t)x), (int16_t)abs16((int16_t)y), (int16_t)abs16((int16_t)z));
    if (size != 0 && (int16_t)length != 0) {
        if (size < 0) size = (int16_t)-size;
        /* A power-of-four multiple of the scale just above the length. */
        factor = size;
        shift = 8;
        while (factor <= length) {
            factor <<= 2;
            shift += 2;
        }
        do {
            factor >>= 2;
            shift -= 2;
        } while (shift > 1 && factor > length);
        factor <<= 2;
        shift += 2;
        factor = (int32_t)divu_w((uint32_t)factor << 8, (uint16_t)length);
        rx = (int16_t)(((int32_t)(int16_t)x * (int16_t)factor) >> shift);
        ry = (int16_t)(((int32_t)(int16_t)y * (int16_t)factor) >> shift);
        rz = (int16_t)(((int32_t)(int16_t)z * (int16_t)factor) >> shift);
        if ((int16_t)((uint32_t)scale >> 16) < 0) {
            rx = (int16_t)-rx;
            ry = (int16_t)-ry;
            rz = (int16_t)-rz;
        }
    }
    wr_s16(NORMALIZED, rx);
    wr_s16(NORMALIZED + 2, ry);
    wr_s16(NORMALIZED + 4, rz);
}

/* |a - b| >> 8, components of two long positions. */
static int16_t distance_part(int32_t a, int32_t b) {
    int32_t d = a - b;
    if (d < 0) d = -d;
    return (int16_t)(d >> 8);
}

int flagged_slot_in_range(void) {
    int i;
    for (i = SLOT_COUNT - 1; i >= 0; i--) {
        gaddr slot = SLOT_TABLE + (gaddr)(SLOT_SIZE * i), record, observer = OBSERVER;
        int16_t near, far;
        if (!(rd_u8(slot + 0x27) & 0x01)) continue;
        if (!(rd_u8(slot + 0x26) & 0x20) && !rd_u8(CONTEXT_SELECT)) return 0;
        record = CONTROL_RECORDS + (gaddr)(int32_t)(int16_t)(rd_s16(slot + 0x2E) << 9);
        near = (int16_t)magnitude3(distance_part(rd_s32(record + 0x14), rd_s32(observer + 0x0C)),
                                   distance_part(rd_s32(record + 0x18), rd_s32(observer + 0x10)),
                                   distance_part(rd_s32(record + 0x1C), rd_s32(observer + 0x14)));
        far = (int16_t)magnitude3(
            distance_part(rd_s32(slot) + (int32_t)((uint32_t)(int32_t)rd_s16(slot + 0x30) << 22), rd_s32(observer + 0x0C)),
            distance_part(rd_s32(slot + 4), rd_s32(observer + 0x10)),
            distance_part(rd_s32(slot + 8) + (int32_t)((uint32_t)(int32_t)rd_s16(slot + 0x32) << 22), rd_s32(observer + 0x14)));
        return near > far ? 0 : 1;
    }
    return 0;
}

void update_target_point(void) {
    gaddr record, observer = OBSERVER;
    int32_t d[3];
    int16_t length, beyond;
    int negative[3], i;

    if (!rd_u8(TARGET_ENABLED)) {
        for (i = 0; i < 3; i++) wr_s32(TARGET_POINT + (gaddr)(4 * i), rd_s32(observer + (gaddr)(4 * i)));
        return;
    }
    record = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
    d[0] = (rd_s32(record + 0x14) & 0x3FFFFF) + rd_s32(observer);
    d[1] = rd_s32(record + 0x18) + rd_s32(observer + 4);
    d[2] = (rd_s32(record + 0x1C) & 0x3FFFFF) + rd_s32(observer + 8);
    for (i = 0; i < 3; i++) {
        negative[i] = d[i] < 0;
        if (negative[i]) d[i] = -d[i];
        d[i] >>= 8;
    }
    length = (int16_t)magnitude3((int16_t)d[0], (int16_t)d[1], (int16_t)d[2]);
    beyond = (int16_t)(length - 0x200);
    if (beyond <= 0) {
        for (i = 0; i < 3; i++) d[i] = 0;
    } else if (beyond < (int16_t)(length >> 1)) {
        uint16_t ratio = (uint16_t)divu_w((uint32_t)((int32_t)beyond << 8), (uint16_t)length);
        for (i = 0; i < 3; i++) {
            d[i] = (int32_t)((uint32_t)ratio * (uint16_t)d[i]);
            if (!negative[i]) d[i] = -d[i];
        }
    } else {
        int32_t near = (int32_t)(int16_t)(length - beyond);
        uint16_t ratio = (uint16_t)divu_w((uint32_t)near << 16 | ((uint32_t)near >> 16), (uint16_t)length);
        for (i = 0; i < 3; i++) {
            uint32_t scaled = (uint32_t)ratio * (uint16_t)d[i];
            d[i] = ((int32_t)scaled >> 8) + (int32_t)((scaled >> 7) & 1); /* ASR.L, rounded */
            if (negative[i]) d[i] = -d[i];
        }
        d[0] -= rd_s32(record + 0x14) & 0x3FFFFF;
        d[1] -= rd_s32(record + 0x18);
        d[2] -= rd_s32(record + 0x1C) & 0x3FFFFF;
        for (i = 0; i < 3; i++) wr_s32(TARGET_POINT + (gaddr)(4 * i), d[i]);
        return;
    }
    for (i = 0; i < 3; i++) wr_s32(TARGET_POINT + (gaddr)(4 * i), d[i] + rd_s32(observer + (gaddr)(4 * i)));
}

/* |record's cell coordinate - the other's, as 16.16 / 4 + fine offset|. */
static int32_t cell_distance(gaddr record, int cell, int fine, int16_t coarse, int16_t detail) {
    /* EXT.L, SUB.W, SWAP: the sign-extension word lands in the low half. */
    uint32_t swapped = (uint32_t)(uint16_t)(coarse - rd_s16(record + (gaddr)cell)) << 16 | (coarse < 0 ? 0xFFFFu : 0);
    int32_t d = (int32_t)swapped >> 2;
    d += (int16_t)(detail - rd_s16(record + (gaddr)fine));
    return d < 0 ? -d : d;
}

static void classify_record_range_point(gaddr r, int16_t band_x, int16_t band_z,
                                        int16_t fine_x, int16_t fine_z, int32_t height) {
    int16_t distance;
    int32_t dx, dy, dz;

    dx = cell_distance(r, 0x06, 0x0C, band_x, fine_x);
    dz = cell_distance(r, 0x08, 0x0E, band_z, fine_z);
    dy = height - rd_s32(r + 0x10);
    if (dy < 0) dy = -dy;
    if (dx > 0x7F00 || dy > 0x7F00 || dz > 0x7F00) {
        wr_u16(r + 0x4A, 0x7FFF);
        if (rd_u16(SCRIPT_RECORD)) wr_u8(r + 0x63, (uint8_t)(rd_u8(r + 0x63) | 0xF0));
        return;
    }
    distance = (int16_t)magnitude3((int16_t)dx, (int16_t)dy, (int16_t)dz);
    wr_s16(r + 0x4A, distance);
    wr_u8(r + 0x04, (uint8_t)(rd_u8(r + 0x04) | 0x01));
    if (distance > 0x36C0) {
        if (rd_u16(SCRIPT_RECORD)) wr_u8(r + 0x63, (uint8_t)(rd_u8(r + 0x63) | 0xF0));
        return;
    }
    if (distance > 0x1E00) {
        if (rd_u8(r + 0x7A) == 4) wr_u8(r + 0x7A, 3);
    } else if (rd_u8(r + 0x7A) == 3) {
        wr_u8(r + 0x7A, 4);
    }
    if (!rd_u16(SCRIPT_RECORD)) return;
    if (distance > 0x1800) {
        wr_u16(r + 0x4A, 0x7FFF);
        wr_u8(r + 0x63, (uint8_t)(rd_u8(r + 0x63) | 0xF0));
        return;
    }
    if (distance > 0x300) {
        uint8_t band = distance > 0xC00 ? 0x30 : 0x20;
        wr_u8(r + 0x63, (uint8_t)((rd_u8(r + 0x63) & 0x0F) | band));
        return;
    }
    wr_u8(r + 0x63, (uint8_t)((rd_u8(r + 0x63) & 0x0F) | 0x10));
}

void classify_record_range(gaddr r) {
    int16_t band_x;
    wr_u8(r + 0x39, (uint8_t)(rd_u8(r + 0x39) + 0x10));
    if (rd_s16(r + 0x4A) >= 0x480) {
        int8_t period = rd_s16(r + 0x4A) >= 0x900 ? 0x50 : 0x20;
        if (period >= (int8_t)(rd_u8(r + 0x39) & 0xF0)) return;
    }
    wr_u8(r + 0x39, (uint8_t)(rd_u8(r + 0x39) & 0x0F));
    band_x = rd_s16(r + 0x2C);
    if (band_x < 0) return;
    classify_record_range_point(r, band_x, rd_s16(r + 0x2E),
                                rd_s16(r + 0x30), rd_s16(r + 0x32), rd_s32(r + 0x34));
}

void classify_selected_record_range(gaddr r) {
    int16_t selected;
    if (rd_s16(r + 0x6C) >= 0x1200 && !(rd_u8(r + 0x7C) & 0x70)) {
        if (!rd_u8(BAR_REDRAWS_F)) {
            wr_u8(BAR_REDRAWS_F, 1);
            play_context_tone_4(4);
        }
    } else {
        wr_u8(BAR_REDRAWS_F, 0);
    }
    selected = rd_s16(SELECTED_RECORD);
    if (selected <= 0) return;
    wr_u8(r + 0x39, (uint8_t)(rd_u8(r + 0x39) + 0x10));
    if (rd_s16(r + 0x4A) >= 0x480) {
        int8_t period = rd_s16(r + 0x4A) >= 0x900 ? 0x50 : 0x20;
        if (period >= (int8_t)(rd_u8(r + 0x39) & 0xF0)) return;
    }
    wr_u8(r + 0x39, (uint8_t)(rd_u8(r + 0x39) & 0x0F));
    {
        gaddr source = CONTROL_RECORDS + (gaddr)(int32_t)selected;
        classify_record_range_point(r, rd_s16(source + 0x06), rd_s16(source + 0x08),
                                    rd_s16(source + 0x0C), rd_s16(source + 0x0E),
                                    rd_s32(source + 0x10));
    }
}

void seed_projection(void) {
    int32_t d[3];
    int i;

    if (rd_u8(CONTEXT_SELECT)) {
        update_target_point();
        for (i = 0; i < 3; i++) wr_s32(PROJECTION_ORIGIN + (gaddr)(4 * i), rd_s32(OBSERVER + 0x0C + (gaddr)(4 * i)));
        for (i = 0; i < 3; i++) d[i] = rd_s32(TARGET_POINT + (gaddr)(4 * i));
    } else {
        gaddr r = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
        int16_t eye[3] = {0, 4, 18};
        uint8_t type = rd_u8(r + 0x62);
        if (type == 0x30) { eye[1] = 1; eye[2] = -5; }
        else if (type == 0x11) { eye[1] = 5; eye[2] = 20; }
        for (i = 0; i < 3; i++) {
            gaddr m = r + 0x92 + (gaddr)(6 * i);
            int32_t sum = (int32_t)eye[0] * rd_s16(m) + (int32_t)eye[1] * rd_s16(m + 2) + (int32_t)eye[2] * rd_s16(m + 4);
            d[i] = sum >> 6;
            wr_s32(PROJECTION_ORIGIN + (gaddr)(4 * i), rd_s32(r + 0x14 + (gaddr)(4 * i)) + d[i]);
        }
        d[0] = -(d[0] + (rd_s32(r + 0x14) & 0x3FFFFF));
        d[1] = -(d[1] + rd_s32(r + 0x18));
        d[2] = -(d[2] + (rd_s32(r + 0x1C) & 0x3FFFFF));
        for (i = 0; i < 3; i++) wr_s32(TARGET_POINT + (gaddr)(4 * i), d[i]);
    }
    for (i = 0; i < 3; i++) wr_s16(PROJECTION_WORDS + (gaddr)(2 * i), (int16_t)(d[i] >> 8));
    wr_s32(PROJECTION_Y, d[1] >> 8);
}

/* Look `key` up in the -1-terminated word list at `list`, followed by a
 * table of word offsets from `base`; 0 when it is absent. */
static gaddr keyed_entry(gaddr list, gaddr base, int16_t key) {
    gaddr p = list;
    int16_t index = -2, value;
    do {
        index = (int16_t)(index + 2);
        value = rd_s16(p);
        p += 2;
        if (value < 0) return 0;
    } while (value != key);
    while (rd_s16(p) >= 0) p += 2;
    p += 2;
    return base + (gaddr)(int32_t)rd_s16(p + (gaddr)(int32_t)index);
}

int condition_table_scan(gaddr table, int *last_byte) {
    gaddr p = keyed_entry(table, table, rd_s16(CONDITION_KEY_B));
    int32_t value;
    int8_t want;
    *last_byte = -1;
    if (!p) return 0;
    p = keyed_entry(p, table, rd_s16(CONDITION_KEY_A));
    if (!p) return 0;
    value = -rd_s32(CONDITION_VALUE);
    for (;;) {
        int16_t mode = rd_s16(p);
        int32_t threshold;
        if (mode < 0) return 0;
        threshold = rd_s32(p + 2);
        p += 6;
        if (mode == 0 ? value > threshold : value < threshold) return 0;
        want = (int8_t)rd_u8(CONDITION_BYTE_A);
        *last_byte = (uint8_t)want;
        /* (byte, byte list) pairs, each list ended by a negative byte. A
         * matched first byte switches the comparison to CONDITION_BYTE_B
         * for the rest of the entry, as the original leaves it. */
        for (;;) {
            int8_t b = (int8_t)rd_u8(p++);
            if (b < 0) break;
            if (b == want) {
                want = (int8_t)rd_u8(CONDITION_BYTE_B);
                *last_byte = (uint8_t)want;
                for (;;) {
                    int8_t c = (int8_t)rd_u8(p++);
                    if (c < 0) break;
                    if (c == want) return 1;
                }
            } else {
                while ((int8_t)rd_u8(p++) >= 0) {}
            }
        }
        if (p & 1) p++;
    }
}

int condition_table_matches(gaddr table) {
    int last_byte;
    return condition_table_scan(table, &last_byte);
}

/* The component and bound $C1FC42 compares; 1 when bound < component. */
static int bound_less(uint16_t selector, int16_t offset) {
    gaddr r = rd_u32(BOUND_RECORD);
    int shift = rd_u8(r + 6) & 15;
    switch ((selector & 0x0C00) >> 10) {
    case 1:
        return -rd_s32(CONDITION_VALUE) < (int32_t)(int16_t)(rd_s16(r + (gaddr)(int32_t)offset + 0x0C) >> shift);
    case 2: {
        int16_t c = (int16_t)((rd_s16(r + (gaddr)(int32_t)offset + 0x0A) >> shift) +
                              (int16_t)(rd_s16(BOUND_OFFSET_X) << (rd_u16(BOUND_SHIFT) & 63)));
        return (int16_t)-rd_s16(PROJECTION_WORDS) < c;
    }
    default: {
        int16_t c = (int16_t)((rd_s16(r + (gaddr)(int32_t)offset + 0x0E) >> shift) +
                              (int16_t)(rd_s16(BOUND_OFFSET_Z) << (rd_u16(BOUND_SHIFT) & 63)));
        return (int16_t)-rd_s16(PROJECTION_WORDS + 4) < c;
    }
    }
}

int component_beyond_bound(uint16_t selector, int16_t offset) {
    return bound_less(selector, offset) != ((selector & 0x1000) != 0);
}

void update_condition_a(void) {
    wr_u8(CONDITION_MET_A, (uint8_t)condition_table_matches(CONDITIONS_A));
}

void update_condition_b(void) {
    wr_u8(CONDITION_MET_B, (uint8_t)condition_table_matches(CONDITIONS_B));
}
