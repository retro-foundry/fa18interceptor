/* Glue for magnitude3 ($C1D974). */
#include "glue.h"
#include "ports_glue.h"

#include "fixed_math.h"
#include "globals.h"
#include "memory.h"

static uint32_t divu_w(uint32_t dividend, uint16_t divisor) {
    uint32_t q = dividend / divisor;
    if (q > 0xFFFF) return dividend;
    return (dividend % divisor) << 16 | q;
}

/* $C1D974: D2.w, D3.w, D4.w components -> D1 the magnitude. The caller
 * reads D3 and D4 as the two table lookups leave them (D2 the last divisor),
 * and the flags of the final MOVE.W D1 store. */
void magnitude_registers(void);
void magnitude_registers(void) {
    uint32_t d2 = D(2), d3 = D(3), d4 = D(4), t;
    int32_t result;

    if ((int16_t)d3 > (int16_t)d2) {
        t = d2;
        d2 = d3;
        d3 = t;
    }
    d3 = (uint32_t)(int32_t)(int16_t)d3;
    if (d3 != 0) {
        d3 <<= 8;
        if ((uint16_t)d2 == 0) d3 = 0;
        else {
            d3 = divu_w(d3, (uint16_t)d2);
            SET_W(d3, (uint16_t)(d3 * 2));
        }
    }
    SET_W(d3, rd_u16(MAGNITUDE_TABLE + (gaddr)(int32_t)(int16_t)d3));
    d2 = (uint32_t)(uint16_t)d3 * (uint16_t)d2;
    d4 = (uint32_t)((int32_t)(int16_t)d4 << 14);
    if ((int32_t)d4 > (int32_t)d2) {
        t = d2;
        d2 = d4;
        d4 = t;
    }
    d2 = (uint32_t)((int32_t)d2 >> 14);
    if ((uint16_t)d2 == 0) d4 = 0;
    else {
        d4 = divu_w(d4, (uint16_t)d2);
        SET_W(d4, (uint16_t)((int16_t)d4 >> 6));
        SET_W(d4, (uint16_t)(d4 * 2));
    }

    result = magnitude3((int16_t)D(2), (int16_t)D(3), (int16_t)D(4));
    D(1) = (uint32_t)result;
    D(2) = d2;
    D(3) = d3;
    D(4) = d4;
    flags_logic_w(D(1));
}

int glue_C1D974(void) {
    magnitude_registers();
    return glue_return();
}
