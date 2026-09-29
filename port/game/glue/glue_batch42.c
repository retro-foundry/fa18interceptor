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

int glue_C2574A(void) {
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
    return glue_return();
}
