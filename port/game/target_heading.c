/* Heading text prepared from a selected post-input record ($C25070). */
#include "target_heading.h"

#include "globals.h"
#include "matrix.h"
#include "memory.h"
#include "numbers.h"
#include "tracking.h"

static gaddr selected_record(void) {
    gaddr item = rd_u32(POST_INPUT_RECORD_LIST);

    while (rd_s16(item) >= 0) {
        uint16_t index = rd_u16(item + 4);
        int16_t offset = (int16_t)(uint16_t)(index << 9);
        gaddr record = CONTROL_RECORDS + (gaddr)(int32_t)offset;
        uint8_t type = rd_u8(record + 0x62);

        if (type != 0x15 && (type & 0xF0) == 0x10 && (rd_u8(record + 1) & 0x40)) return record;
        item += 10;
    }
    return 0;
}

/* ABCD adds packed bytes with the 68000's decimal corrections, including
 * the deliberate $0A source byte used by $C25136. */
static uint8_t add_decimal_byte(uint8_t source, uint8_t value, int *carry) {
    unsigned sum = (unsigned)source + value + (unsigned)*carry;
    if ((source & 15u) + (value & 15u) + (unsigned)*carry > 9u) sum += 6;
    if (sum > 0x99u) sum += 0x60u;
    *carry = sum > 0xFFu;
    return (uint8_t)sum;
}

int refresh_post_input_heading(void) {
    gaddr record = selected_record();
    int32_t point[3], elevation = 0, azimuth = 0;
    uint32_t dividend, quotient, packed;
    uint8_t digit;

    if (!record) return -1;
    local_to_world(record, record + RECORD_INVERSE, 0, 0, 0x1F4, point);
    track_direction(&elevation, &azimuth,
                    (int32_t)(point[0] - rd_s32(CONTROL_RECORDS + 0x14)) >> 8,
                    0,
                    (int32_t)(point[2] - rd_s32(CONTROL_RECORDS + 0x1C)) >> 8,
                    -1);

    dividend = (uint32_t)(int32_t)rd_s16(TRACKED_HEADING);
    quotient = dividend / 0x50u;
    wr_u32(DISPLAY_VALUE, (uint32_t)(int32_t)(int16_t)(quotient <= 0xFFFFu ? quotient : dividend));
    pack_display_value();
    packed = rd_u32(DISPLAY_VALUE_BCD);
    digit = (uint8_t)(packed & 15u);
    wr_u8(DISPLAY_VALUE_BCD + 3, (uint8_t)(rd_u8(DISPLAY_VALUE_BCD + 3) & 0xF0u));
    if (digit >= 5) {
        int carry = 0;
        wr_u32(POSTFLIGHT_BCD_TICK, 10);
        wr_u8(DISPLAY_VALUE_BCD + 3,
              add_decimal_byte(rd_u8(POSTFLIGHT_BCD_STEP - 1), rd_u8(DISPLAY_VALUE_BCD + 3), &carry));
        wr_u8(DISPLAY_VALUE_BCD + 2,
              add_decimal_byte(rd_u8(POSTFLIGHT_BCD_STEP - 2), rd_u8(DISPLAY_VALUE_BCD + 2), &carry));
        if (rd_s32(DISPLAY_VALUE_BCD) >= 0x360) wr_u16(DISPLAY_VALUE_BCD + 2, 0);
    }
    wr_u8(POST_INPUT_HEADING_TEXT, (uint8_t)((rd_u8(DISPLAY_VALUE_BCD + 2) & 15u) + '0'));
    wr_u8(POST_INPUT_HEADING_TEXT + 1, (uint8_t)((rd_u8(DISPLAY_VALUE_BCD + 3) >> 4) + '0'));
    wr_u8(POST_INPUT_HEADING_TEXT + 2, (uint8_t)((rd_u8(DISPLAY_VALUE_BCD + 3) & 15u) + '0'));
    return 0;
}
