/* Number formatting for the cockpit displays. */
#include "numbers.h"

#include "globals.h"
#include "memory.h"
#include "text.h"

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

gaddr format_decimal(gaddr end, uint32_t value, int count, int keep_zeros) {
    uint32_t bcd;
    gaddr p = end;
    int i;

    wr_u32(DISPLAY_VALUE, value);
    pack_display_value();
    bcd = rd_u32(DISPLAY_VALUE_BCD);
    for (i = 0; i < count; i++) {
        wr_u8(--p, (uint8_t)((bcd & 15) + '0'));
        bcd >>= 4;
    }
    if (!keep_zeros) {
        gaddr q = p;
        for (i = 0; i < count - 1 && rd_u8(q) == '0'; i++) wr_u8(q++, ' ');
    }
    return p;
}

void print_number(gaddr field, int16_t offset, uint32_t value, int8_t width) {
    wr_u32(DISPLAY_VALUE, value);
    pack_display_value();
    /* Packed BCD printed as hex digits is the decimal number. */
    format_hex(field + (gaddr)(int32_t)offset, rd_u32(DISPLAY_VALUE_BCD), width);
}

void unpack_display_value(void) {
    static const uint32_t place_value[8] = {10000000u, 1000000u, 100000u, 10000u,
                                            1000u, 100u, 10u, 1u};
    uint32_t value = 0;
    int i;
    for (i = 0; i < 8; i++) {
        uint8_t pair = rd_u8(DISPLAY_VALUE_BCD + (gaddr)(i / 2));
        int digit = (i & 1) ? (pair & 15) : (pair >> 4);
        value += (uint32_t)digit * place_value[i];
    }
    wr_u32(DISPLAY_VALUE, value);
}

void format_date_line(void) {
    uint32_t seconds = rd_u32(rd_u32(MODE_TABLE) + 8);
    uint32_t q = seconds / 3600;
    int16_t days = (int16_t)(q > 0xFFFF ? seconds : q); /* DIVU overflow keeps the dividend */
    int16_t capped = days > 0x7F ? 0x7F : days, day;
    gaddr name = MONTH_NAMES + (gaddr)(9 * ((uint16_t)capped >> 5));
    int i;

    for (i = 0; i < 9; i++) wr_u8(DATE_LINE + 0x14 - (gaddr)i, rd_u8(name + (gaddr)i));
    day = (int16_t)((days & 0x1F) + 1);
    if (day > 30) day = 30;
    print_number(DATE_LINE, 0x15, (uint32_t)(int32_t)day, 2);
}
