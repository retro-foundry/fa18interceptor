/* Fixed-point trigonometry. */
#include "fixed_math.h"

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
