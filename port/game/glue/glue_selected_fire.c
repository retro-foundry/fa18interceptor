/* Register flow through the selected-fire record initializer ($C2374C). */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "selected_fire.h"

int glue_C2374C(void) {
    gaddr destination = A(1), selected = A(2);
    gaddr mode = rd_u32(MODE_TABLE), pointer_source, pointer_target, offset_table;
    uint8_t record_class = rd_u8(selected + 0x63) & 0xF0u;
    uint8_t remaining = rd_u8(selected + 0x5F);
    uint8_t used = record_class == 0x30u ? remaining & 15u : remaining & 0xF0u;
    uint32_t pending_warning = rd_u32(WARNING_CAUSES) & 0x4000u;
    int16_t t0, t1, t2;
    int32_t p0, p1, p2;
    uint32_t x, y, z;

    consume_selected_fire_request(destination, selected);
    D(6) = pending_warning;
    SET_B(D(1), remaining);
    SET_B(D(0), record_class);
    SET_B(D(1), used);
    if (!used) return glue_return();

    D(0) = 0xFFFFu; /* MOVEQ #$28, then DBRA over 41 longwords. */
    A(0) = mode;
    A(4) = selected + 164;
    A(5) = destination + 164;
    if (record_class == 0x30u) {
        SET_W(D(0), 0x14u);
    } else {
        SET_W(D(0), 0);
    }
    pointer_source = SCENE_POINTER_TABLE + ((record_class == 0x30u) ? 0x14u : 0);
    pointer_target = SCENE_POINTERS + (gaddr)(int32_t)(int16_t)(rd_s16(STREAM_MODE) * 20);
    A(4) = pointer_source;
    A(5) = pointer_target;
    SET_W(D(3), rd_u16(STREAM_MODE));
    SET_W(D(3), (uint16_t)(D(3) * 20u));
    D(2) = rd_u32(pointer_source);
    D(3) = rd_u32(pointer_source + 4);
    D(4) = rd_u32(pointer_source + 8);
    D(5) = rd_u32(pointer_source + 12);
    D(6) = rd_u32(pointer_source + 16);
    SET_B(D(1), rd_u8(destination + 0x62));
    SET_B(D(1), (uint8_t)D(1) & 0xF0u);
    SET_B(D(1), remaining);
    SET_B(D(2), remaining);
    SET_B(D(0), record_class);
    if (record_class == 0x30u) {
        SET_B(D(1), remaining & 15u);
        SET_B(D(2), remaining & 0xF0u);
        SET_B(D(1), (uint8_t)(D(1) - 1));
        SET_B(D(0), (uint8_t)(D(1) + 2));
    } else {
        SET_B(D(1), remaining & 0xF0u);
        SET_B(D(2), remaining & 15u);
        SET_B(D(1), (uint8_t)(D(1) - 0x10u));
        SET_B(D(0), (uint8_t)D(1) >> 4);
    }
    offset_table = rd_u8(selected + 0x62) == 0x10u ? 0xC23A32u : 0xC23A56u;
    A(4) = offset_table;
    SET_W(D(0), (int8_t)D(0));
    SET_W(D(0), (uint16_t)(D(0) * 6u));
    t0 = rd_s16(offset_table + (gaddr)(int32_t)(int16_t)D(0));
    t1 = rd_s16(offset_table + (gaddr)(int32_t)(int16_t)D(0) + 2);
    t2 = rd_s16(offset_table + (gaddr)(int32_t)(int16_t)D(0) + 4);
    p0 = (int32_t)t0 * rd_s16(selected + 0x92);
    p1 = (int32_t)t0 * rd_s16(selected + 0x98);
    p2 = (int32_t)t0 * rd_s16(selected + 0x9E);
    x = (uint32_t)p0 + (uint32_t)((int32_t)t1 * rd_s16(selected + 0x94))
      + (uint32_t)((int32_t)t2 * rd_s16(selected + 0x96));
    y = (uint32_t)p1 + (uint32_t)((int32_t)t1 * rd_s16(selected + 0x9A))
      + (uint32_t)((int32_t)t2 * rd_s16(selected + 0x9C));
    z = (uint32_t)p2 + (uint32_t)((int32_t)t1 * rd_s16(selected + 0xA0))
      + (uint32_t)((int32_t)t2 * rd_s16(selected + 0xA2));
    D(0) = (uint32_t)((int32_t)x >> 6);
    D(1) = (uint32_t)((int32_t)y >> 6);
    D(2) = (uint32_t)((int32_t)z >> 6);
    D(3) = (uint32_t)p2;
    D(4) = (uint32_t)(int32_t)t1;
    D(5) = (uint32_t)((int32_t)t2 * rd_s16(selected + 0xA2));
    D(6) = (uint32_t)p1;
    D(7) = (uint32_t)((int32_t)t2 * rd_s16(selected + 0x9C));
    SET_W(D(0), 0);
    return glue_return();
}
