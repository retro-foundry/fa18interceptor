/* Glue for $C2574A, normalize_vector's register entry: D0.w is the scale
 * (its own sign signs the result) and D5-D7 the vector. Every register is
 * live after it: the magnitude's leftovers in D1-D4, the power-of-four
 * search's shift in D2 and the DIVU result in D0. */
#include "glue.h"
#include "ports_glue.h"

#include "fixed_math.h"
#include "globals.h"
#include "memory.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define W(n) ((int16_t)D(n))

void magnitude_registers(void); /* glue_batch22.c */

static uint32_t divu_w(uint32_t dividend, uint16_t divisor) {
    uint32_t q = dividend / divisor;
    if (q > 0xFFFF) return dividend;
    return (dividend % divisor) << 16 | q;
}

static uint16_t abs_word(uint32_t v) { return (int16_t)v < 0 ? (uint16_t)-(int16_t)v : (uint16_t)v; }

/* normalize_vector entered with registers (D0.w scale, D5-D7 vector):
 * runs it and leaves its registers. Idempotent on the same inputs. */
void normalize_registers(void);
void normalize_registers(void) {
    int16_t scale = W(0);

    normalize_vector((int32_t)scale, (int16_t)D(5), (int16_t)D(6), (int16_t)D(7));
    if (scale) {
        SET_W(D(0), abs_word(D(0)));
        SET_W(D(2), abs_word(D(5)));
        SET_W(D(3), abs_word(D(6)));
        SET_W(D(4), abs_word(D(7)));
        magnitude_registers();
        if ((uint16_t)D(1)) {
            int32_t factor = W(0), length = (int32_t)D(1);
            D(2) = 8;
            while (factor <= length) {
                factor <<= 2;
                SET_W(D(2), W(2) + 2);
            }
            do {
                factor >>= 2;
                SET_W(D(2), W(2) - 2);
            } while (W(2) > 1 && factor > length);
            factor <<= 2;
            SET_W(D(2), W(2) + 2);
            D(0) = divu_w((uint32_t)factor << 8, (uint16_t)D(1));
        }
    }
    D(5) = SEXT(rd_u16(NORMALIZED));
    D(6) = SEXT(rd_u16(NORMALIZED + 2));
    D(7) = SEXT(rd_u16(NORMALIZED + 4));
}

int glue_C2574A(void) {
    normalize_registers();
    return glue_return();
}

/* $C1D91A: D2-D4 the point. Every register is live after it: D6 the
 * shifted target height, D1-D4 as magnitude3 leaves them (or the
 * saturated height's), D5 and D7 restored. */
int glue_C1D91A(void) {
    int16_t x = W(2), y = W(3), z = W(4), shift = rd_s16(BOUND_SHIFT);
    int count = shift & 63;

    target_distance(x, y, z);
    SET_W(D(1), rd_u16(PROJECTION_WORDS));
    D(6) = rd_u32(PROJECTION_Y);
    SET_W(D(1), (uint16_t)(count >= 16 ? (W(1) < 0 ? -1 : 0) : W(1) >> count));
    D(6) = (uint32_t)(count >= 32 ? ((int32_t)D(6) < 0 ? -1 : 0) : (int32_t)D(6) >> count);
    SET_W(D(2), W(2) + W(1));
    if (W(2) < 0) SET_W(D(2), (uint16_t)-W(2));
    if (rd_u8(POSITION_VALID)) D(3) = (uint32_t)(rd_s32(POSITION_LEVEL) >> 8);
    else D(3) = SEXT(D(3)) + D(6);
    if ((int32_t)D(3) < 0) D(3) = 0u - D(3);
    if ((int32_t)D(3) >= 0x7FFF0) {
        SET_W(D(1), 0x7FFF);
        return glue_return();
    }
    {
        int16_t pz = rd_s16(PROJECTION_WORDS + 4);
        SET_W(D(4), W(4) + (count >= 16 ? (pz < 0 ? -1 : 0) : pz >> count));
    }
    if (W(4) < 0) SET_W(D(4), (uint16_t)-W(4));
    SET_W(D(2), (uint16_t)(W(2) >> 4));
    D(3) = (uint32_t)((int32_t)D(3) >> 4);
    SET_W(D(4), (uint16_t)(W(4) >> 4));
    magnitude_registers();
    return glue_return();
}
