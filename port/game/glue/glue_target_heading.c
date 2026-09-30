/* Register flow through the post-input heading formatter ($C25070). */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "numbers.h"
#include "target_heading.h"
#include "glue_text.h"

int glue_C25070(void) {
    gaddr item = rd_u32(POST_INPUT_RECORD_LIST);
    uint8_t type;
    uint32_t packed;
    int32_t x, y = 0, z;
    int found = 0;

    A(1) = CONTROL_RECORDS;
    A(2) = item;
    while (rd_s16(item) >= 0) {
        int16_t offset;
        SET_W(D(3), rd_u16(item + 4));
        A(2) += 10;
        item += 10;
        SET_W(D(3), (uint16_t)D(3) << 8);
        SET_W(D(3), (uint16_t)(D(3) + D(3)));
        offset = (int16_t)D(3);
        type = rd_u8(A(1) + (gaddr)(int32_t)offset + 0x62);
        SET_B(D(1), type);
        if (type == 0x15) continue;
        SET_B(D(1), type & 0xF0u);
        if ((uint8_t)D(1) != 0x10) continue;
        if (!(rd_u8(A(1) + (gaddr)(int32_t)offset + 1) & 0x40)) continue;
        A(1) += (gaddr)(int32_t)offset;
        found = 1;
        break;
    }
    if (!found) {
        (void)refresh_post_input_heading();
        D(0) = 0xFFFFFFFFu;
        return glue_return();
    }

    (void)refresh_post_input_heading();
    D(3) = 0;
    D(4) = 0;
    SET_W(D(5), 0x1F4);
    world_registers(A(1), A(1) + RECORD_INVERSE);
    D(4) = D(2);
    D(2) = (uint32_t)((int32_t)(D(0) - rd_u32(CONTROL_RECORDS + 0x14)) >> 8);
    D(4) = (uint32_t)((int32_t)(D(4) - rd_u32(CONTROL_RECORDS + 0x1C)) >> 8);
    D(3) = 0;
    D(5) = 0xFFFFFFFFu;
    D(0) = 0;
    D(1) = 0;
    x = (int32_t)D(2);
    z = (int32_t)D(4);
    track_direction_registers(0, 0, 0, &x, &y, &z, -1, 1);

    packed = to_packed_bcd(rd_u32(DISPLAY_VALUE));
    if ((packed & 15u) >= 5) A(2) = POSTFLIGHT_BCD_STEP - 2;
    A(1) = POST_INPUT_HEADING_TEXT + 3;
    A(3) = DISPLAY_VALUE_BCD + 3;
    SET_B(D(1), (rd_u8(DISPLAY_VALUE_BCD + 3) & 15u) + '0');
    D(0) = 0;
    return glue_return();
}
