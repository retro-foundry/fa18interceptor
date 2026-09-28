/* Cockpit numeric readouts. */
#include "readouts.h"

#include "globals.h"
#include "memory.h"

void update_readout(void) {
    if (rd_s32(READOUT_SOURCE_VALID) >= 0) {
        int32_t divisor = rd_s32(READOUT_DIVISOR);
        int16_t value;
        if (divisor > 0x7FFF) {
            value = 9999;
        } else {
            /* DIVU.W: an overflowing quotient leaves the dividend's low word. */
            uint32_t q = 800000u / (uint16_t)divisor;
            value = (int16_t)(q > 0xFFFF ? 800000u & 0xFFFF : q);
        }
        if (value < rd_s16(READOUT_MINIMUM)) wr_s16(READOUT_MINIMUM, value);
        if (value > rd_s16(READOUT_MAXIMUM)) wr_s16(READOUT_MAXIMUM, value);
        wr_s16(READOUT_VALUE, value);
    }
    wr_u32(READOUT_SOURCE_VALID, rd_u32(READOUT_SAMPLE));
    wr_u32(READOUT_SOURCE_VALID + 4, rd_u32(READOUT_SAMPLE + 4));
}
