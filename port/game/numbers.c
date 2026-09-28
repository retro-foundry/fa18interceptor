/* Number formatting for the cockpit displays. */
#include "numbers.h"

#include "globals.h"
#include "memory.h"

uint32_t to_packed_bcd(uint32_t value) {
    static const uint32_t place_value[8] = {10000000u, 1000000u, 100000u, 10000u,
                                            1000u, 100u, 10u, 1u};
    uint32_t bcd = 0, digit = 0x10000000u;
    int place;
    for (place = 0; place < 8; place++, digit >>= 4) {
        while (value >= place_value[place]) {
            value -= place_value[place];
            bcd += digit;
        }
    }
    return bcd;
}

void pack_display_value(void) { wr_u32(DISPLAY_VALUE_BCD, to_packed_bcd(rd_u32(DISPLAY_VALUE))); }
