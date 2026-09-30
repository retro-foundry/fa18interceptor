/* Selected-fire record initialization ($C2374C-$C23A24). */
#include "selected_fire.h"

#include "fixed_math.h"
#include "globals.h"

static uint32_t transformed(gaddr selected, gaddr table, int row) {
    uint32_t sum = 0;
    int column;
    for (column = 0; column < 3; ++column)
        sum += (uint32_t)((int32_t)rd_s16(table + (gaddr)(2 * column))
             * rd_s16(selected + 0x92 + (gaddr)(6 * row + 2 * column)));
    return (uint32_t)((int32_t)sum >> 6);
}

void consume_selected_fire_request(gaddr destination, gaddr selected) {
    gaddr mode = rd_u32(MODE_TABLE), pointer_source, pointer_target, offset_table;
    uint8_t record_class = rd_u8(selected + 0x63) & 0xF0u;
    uint8_t remaining = rd_u8(selected + 0x5F);
    uint8_t used, selector;
    int i;

    wr_u8(SPACE_COMMAND_LATCH, 0);
    wr_u8(FIRE_RECORD_PENDING, 1);
    if (rd_u32(WARNING_CAUSES) & 0x4000u) {
        wr_u32(WARNING_CAUSES, rd_u32(WARNING_CAUSES) & ~0x4000u);
        wr_u32(EVENT_BITS, rd_u32(EVENT_BITS) | 8u);
    }
    used = record_class == 0x30u ? remaining & 15u : remaining & 0xF0u;
    if (!used) {
        wr_u8(destination + 1, rd_u8(destination + 1) & (uint8_t)~0x40u);
        return;
    }
    if ((uint16_t)(selected - CONTROL_RECORDS) == rd_u16(VIEW_RECORD)) {
        wr_u8(FIRE_ALERT_COUNTDOWN, 8);
        wr_u8(FIRE_STATE, 0xFB);
    }
    for (i = 0; i < 41; ++i) wr_u32(destination + (gaddr)(4 * i), rd_u32(selected + (gaddr)(4 * i)));
    wr_u8(destination + 5, 0);

    if (record_class == 0x30u) {
        wr_u8(destination + 0x62, 1);
        pointer_source = SCENE_POINTER_TABLE + 0x14u;
        if (selected == CONTROL_RECORDS) {
            wr_u16(mode + 0x3E, (uint16_t)(rd_u16(mode + 0x3E) + 1));
            wr_u8(MODE_TABLE_CHANGED, 1);
        }
        selector = (uint8_t)((remaining & 15u) - 1u);
        used = (uint8_t)(selector + 2u);
        remaining = (remaining & 0xF0u) | selector;
    } else {
        wr_u8(destination + 0x62, 0);
        pointer_source = SCENE_POINTER_TABLE;
        if (selected == CONTROL_RECORDS) {
            wr_u16(mode + 0x42, (uint16_t)(rd_u16(mode + 0x42) + 1));
            wr_u8(MODE_TABLE_CHANGED, 1);
        }
        selector = (uint8_t)((remaining & 0xF0u) - 0x10u);
        used = selector >> 4;
        remaining = (remaining & 15u) | selector;
    }
    pointer_target = SCENE_POINTERS + (gaddr)(int32_t)(int16_t)(rd_s16(STREAM_MODE) * 20);
    for (i = 0; i < 5; ++i)
        wr_u32(pointer_target + (gaddr)(4 * i), rd_u32(pointer_source + (gaddr)(4 * i)));
    wr_u16(destination + 0x56, 0);
    wr_u16(destination + 0x58, 0);
    wr_u16(destination + 0x5A, 0);
    wr_u8(destination + 0x64, 0);
    wr_u8(destination + 3, rd_u8(destination + 3) & (uint8_t)~4u);
    wr_u8(selected + 0x5F, remaining);
    wr_u8(STORES_REDRAWS, 3);
    wr_u8(STORES_REDRAWS + 1, 3);

    offset_table = rd_u8(selected + 0x62) == 0x10u ? 0xC23A32u : 0xC23A56u;
    offset_table += (gaddr)(6u * used);
    wr_u32(destination + 0x14, rd_u32(destination + 0x14) + transformed(selected, offset_table, 0));
    wr_u32(destination + 0x18, rd_u32(destination + 0x18) + transformed(selected, offset_table, 1));
    wr_u32(destination + 0x1C, rd_u32(destination + 0x1C) + transformed(selected, offset_table, 2));
    wr_u16(destination + 0x4C, rd_u8(destination + 0x62) ? 0xC8u : 0x96u);
    wr_u16(destination + 0x26, 0x14);
    wr_u16(destination, rd_u16(destination) | 0x11C2u);
    wr_u16(destination + 0x76, 0);
    if (rd_u16(destination + 2) & 0x80u) wr_u32(destination + 0x42, 0);
    wr_u16(destination + 2, rd_u16(destination + 2) & (uint16_t)~0x80u);
    wr_u16(destination + 0x2C, 0xFFFFu);
    wr_u8(UPDATE_MASK, 0x8Cu);
    wr_u8(destination + 0x38, rd_u8(selected + 0x38));

    if ((rd_u8(destination + 0x62) & 0xF0u) == 0x30u) {
        int16_t x = rd_s16(selected + 0x94) >> 2;
        int16_t y = rd_s16(selected + 0x9A) >> 2;
        int16_t z = rd_s16(selected + 0xA0) >> 2;
        if (rd_u8(destination + 0x62) != 0x30u) {
            x = (int16_t)-x;
            y = (int16_t)-y;
            z = (int16_t)-z;
        }
        normalize_vector(0x3C0, x, y, z);
        wr_u32(destination + 0x3E, rd_u32(destination + 0x3E) + (int32_t)rd_s16(NORMALIZED));
        wr_u32(destination + 0x42, rd_u32(destination + 0x42) + (int32_t)rd_s16(NORMALIZED + 2));
        wr_u32(destination + 0x46, rd_u32(destination + 0x46) + (int32_t)rd_s16(NORMALIZED + 4));
        wr_u16(destination + 0x4C, 0xFFECu);
    }
}
